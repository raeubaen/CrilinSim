import numpy as np
import uproot
import awkward as ak
import argparse
import ROOT
import tensorflow as tf
from tensorflow.keras import layers, Model

def parse_args():
    parser = argparse.ArgumentParser(
        description="Pion gun analysis"
    )

    parser.add_argument(
        "--file",
        required=True,
        help="name of the root file"
    )

    parser.add_argument(
        "--pe",
        type=float,
        default=0.2, #pe
        help="photo-electrons for Poissonian smearing"
    )

    parser.add_argument(
        "--noise",
        type=float,
        default=0.,
        help="Gaussian noise value"
    )

    parser.add_argument(
        "--train",
        type=bool,
        default=False, 
        help=" Call it if you want to train again"
    )

    return parser.parse_args()



class EdgeConv(layers.Layer):
    def __init__(self, k, filters):
        super().__init__()
        self.k = k
        self.filters = filters
        self.conv1 = layers.Conv2D(filters, 1, activation='relu')
        self.bn1 = layers.BatchNormalization()
        self.conv2 = layers.Conv2D(filters, 1, activation='relu')
        self.bn2 = layers.BatchNormalization()
    
    def call(self, x):
        k = self.k

        # Compute mask from zeros
        mask = tf.reduce_any(tf.not_equal(x, 0.0), axis=-1)  # (B, hits)
        mask = tf.cast(mask, x.dtype)

        # Pairwise distance
        xx = tf.reduce_sum(tf.square(x), axis=-1, keepdims=True)
        pairwise_distance = xx - 2 * tf.matmul(x, x, transpose_b=True) + tf.transpose(xx, [0,2,1])
        mask_matrix = mask[:, :, None] * mask[:, None, :]
        pairwise_distance += (1.0 - mask_matrix) * 1e6

        # KNN
        idx = tf.math.top_k(-pairwise_distance, k=k).indices
        neighbors = tf.gather(x, idx, batch_dims=1)

        # Broadcast mask properly
        mask_expanded = tf.expand_dims(mask, axis=2)        # (B, hits, 1)
        mask_expanded = tf.tile(mask_expanded, [1,1,k])     # (B, hits, k)
        mask_expanded = tf.expand_dims(mask_expanded, axis=-1)  # (B, hits, k, 1)
        neighbors *= mask_expanded

        # Central features
        central = tf.expand_dims(x, 2)
        central = tf.tile(central, [1,1,k,1])

        # Edge features
        edge = tf.concat([central, neighbors - central], axis=-1)

        # Conv layers
        h = self.conv1(edge)
        h = self.bn1(h)
        h = self.conv2(h)
        h = self.bn2(h)

        # Max over neighbors
        h = tf.reduce_max(h, axis=2)
        return h

def knn(x, mask, k):
    """
    Compute k-nearest neighbors while ignoring masked (zero) entries.
    
    x: (batch, hits, features)
    mask: (batch, hits), 1 for valid hits, 0 for padding
    k: number of neighbors
    """
    # Compute pairwise squared distances
    xx = tf.reduce_sum(tf.square(x), axis=-1, keepdims=True)  # (B, hits, 1)
    pairwise_distance = xx - 2 * tf.matmul(x, x, transpose_b=True) + tf.transpose(xx, [0,2,1])

    # Mask out padded hits by setting distance to large number
    mask = tf.cast(mask, x.dtype)
    mask_matrix = mask[:, :, None] * mask[:, None, :]  # (B, hits, hits)
    pairwise_distance = pairwise_distance + (1.0 - mask_matrix) * 1e6

    # Get k smallest distances (negative for top_k)
    idx = tf.math.top_k(-pairwise_distance, k=k).indices  # (B, hits, k)
    return idx



def build_particlenet(maxhits):

    inputs = layers.Input(shape=(maxhits, 4))
    x = EdgeConv(k=50, filters=32)(inputs)
    x = EdgeConv(k=50, filters=32)(x)
    x = EdgeConv(k=50, filters=64)(x)

    x = layers.GlobalAveragePooling1D()(x)
    x = layers.Dense(128, activation='relu')(x)
    x = layers.BatchNormalization()(x)
    x = layers.Dropout(0.3)(x)
    x = layers.Dense(64, activation='relu')(x)
    x = layers.BatchNormalization()(x)
    output = layers.Dense(1)(x)

    model = tf.keras.Model(inputs, output)

    return model

def main():
    gpus = tf.config.list_physical_devices('GPU')
    tf.config.set_visible_devices(gpus, 'GPU')
    args = parse_args()
    
    try:
        tree = uproot.open(f"{args.file}:events")
        #arrays = tree.arrays(["EventID","VD_energy","Hit_x","Hit_y","Hit_z","Hit_E"], library="np")
        arrays = tree.arrays()
        EventID   = arrays["EventID"].to_numpy()
        VD_energy = arrays["VD_energy"].to_numpy()/1e3
        PrimaryEnergy = arrays["PrimaryEnergy"].to_numpy()/1e3
        max_hits = 7*7*5 #nchannels

        # pad events to same length
        Hit_x_pad = ak.pad_none(arrays["Hit_x"], max_hits)
        Hit_y_pad = ak.pad_none(arrays["Hit_y"], max_hits)
        Hit_z_pad = ak.pad_none(arrays["Hit_z"], max_hits)
        Hit_N_pad = ak.pad_none(arrays["Hit_NCherenkov"], max_hits)
        x = ak.to_numpy(ak.fill_none(Hit_x_pad, 0))
        y = ak.to_numpy(ak.fill_none(Hit_y_pad, 0))
        z = ak.to_numpy(ak.fill_none(Hit_z_pad, 0))
        N = ak.to_numpy(ak.fill_none(Hit_N_pad, 0))

        print(VD_energy, x.shape)

    except (KeyError, ValueError, IndexError):
        print("Tree not found")
        exit(1)
    
    #optical transport and quantum efficiency, calibrated with electrons for having 1 pe/MeV
    eff = 0.0451516

    N = (np.random.poisson(eff * N))
    underthre_mask = N<50.0 #to cut the MIPs
    N[underthre_mask]=0
    x[underthre_mask]=0
    y[underthre_mask]=0
    z[underthre_mask]=0

    N_sum = np.sum(N, axis=1)
    print("N sum shape", N_sum.shape)
    
    X = np.stack([x, y, z, N], axis=-1)[N_sum > 500.0, :, :]   #
    y_target = (PrimaryEnergy-VD_energy)[N_sum > 500.0]
    model = build_particlenet(maxhits=max_hits)
    if args.train == True:
        model.compile( optimizer=tf.keras.optimizers.Adam(1e-4), loss='mse', metrics=['mae'])
        model.summary()
        checkpoint = tf.keras.callbacks.ModelCheckpoint(
            "particlenet_weights.weights.h5",
            monitor="val_loss",
            save_best_only=True,
            save_weights_only=True,
            verbose=1
        )
        history = model.fit(X, y_target, batch_size=64, epochs=30,validation_split=0.2, shuffle=True, callbacks=[checkpoint])
    else:
        print("Loading weights from particlenet_weights.weights.h5")
        model.load_weights("particlenet_weights.weights.h5")

    
    #resolution = np.std((pred.flatten()-y_target)/y_target)
    #print("Energy resolution:", resolution)
    pred = model.predict(X).flatten()

    resolution = np.std((pred - y_target)/y_target)
    print("Energy resolution:", resolution)
    
    
    pred_to_write = np.zeros((N.shape[0],))
    pred_to_write[N_sum > 500.0] = pred

    events_ak = ak.zip({
        "EventID": EventID,
        "VD_energy": VD_energy,
        "PrimaryEnergy": PrimaryEnergy,
        "Hit_x": arrays["Hit_x"],   # jagged
        "Hit_y": arrays["Hit_y"],   # jagged
        "Hit_z": arrays["Hit_z"],   # jagged
        "Hit_NCherenkov": arrays["Hit_NCherenkov"],   # jagged
        "pred_energy": pred_to_write  # flat array aligned with events
    })

    with uproot.recreate("prediction.root") as fout:
        fout["events"] = events_ak

    # h = ROOT.TH1D("h", "Npe; N_{pe};Entries",2000, 0, 20000)
    # weights = np.ones_like(E, dtype=np.float64)
    # h.FillN(len(E), E.astype(np.float64), weights)
    # h.SaveAs("npe.root")
    

if __name__ == "__main__":
    main()
