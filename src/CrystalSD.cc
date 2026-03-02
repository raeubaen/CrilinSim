#include "CrystalSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "CrystalHitsCollection.hh" // Includi il file corretto
#include "CrystalHit.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4ios.hh"
#include "G4VTouchable.hh"
#include "G4Track.hh"

// Costruttore della classe CrystalSD
CrystalSD::CrystalSD(const G4String& name)
    : G4VSensitiveDetector(name), fHitsCollection(nullptr) {
    // Registrazione del nome della raccolta di hit
    // Correctly add the first (and only) hits collection name
    collectionName.push_back("CrystalHitsCollection");
}

// Distruttore della classe CrystalSD
CrystalSD::~CrystalSD() {
    // Pulizia della memoria, se necessario
   if(fHitsCollection) {
        //delete fHitsCollection;
        fHitsCollection = nullptr;
   }
}

// Metodo Initialize per l'inizializzazione della raccolta di hit
void CrystalSD::Initialize(G4HCofThisEvent* hce) {
    // Creazione di una nuova raccolta di hit
    fHitsCollection = new CrystalHitsCollection(SensitiveDetectorName, collectionName[0]);

    // Ottieni l'ID della raccolta di hit
    G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    
    // Aggiunta della raccolta di hit all'evento corrente
    hce->AddHitsCollection(hcID, fHitsCollection);
}

// Metodo ProcessHits per la gestione degli hit
G4bool CrystalSD::ProcessHits(G4Step* step, G4TouchableHistory* history) {
    // Energia depositata nel passo
    G4double edep = step->GetTotalEnergyDeposit();
    if (edep == 0.0) return false;

    // Creazione di un nuovo hit
    CrystalHit* newHit = new CrystalHit();

    // Identificazione del volume sensibile
    const G4TouchableHistory* touchable = static_cast<const G4TouchableHistory*>(step->GetPreStepPoint()->GetTouchable());
    G4int detID = touchable->GetReplicaNumber(0);  // Ottieni l'ID del volume
    
    // Assegna l'ID al hit
    newHit->SetID(detID);

    // Set dell'energia depositata nel nuovo hit
    newHit->AddEnergy(edep);

    // Aggiunta del nuovo hit alla raccolta di hit
    fHitsCollection->insert(newHit);

    return true;
}

// Metodo EndOfEvent per la gestione di fine evento
void CrystalSD::EndOfEvent(G4HCofThisEvent* hce) {
    // Verifica che la raccolta di hit esista
    if (!fHitsCollection) {
        G4ExceptionDescription msg;
        msg << "No hits collection found in EndOfEvent.";
        G4Exception("CrystalSD::EndOfEvent", "MyCode0001", FatalException, msg);
    }

    // Gestione degli hit alla fine dell'evento
    G4int nofHits = fHitsCollection->entries();
    if (nofHits > 0) {
        //G4cout << "Number of hits: " << nofHits << G4endl;
    }
}
