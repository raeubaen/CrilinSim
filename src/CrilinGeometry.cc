#include "CrilinGeometry.hh"
#include "G4ThreeVector.hh"


G4ThreeVector CrilinGeometry::GetCrystalCenter(G4int i,
                                               G4int j,
                                               G4int k) const
{
    G4double x = fX0 + i * fPitch;
    G4double y = fY0 + j * fPitch;
    G4double z = k * fLayerStep + fCrystalLength / 2.0 + fZ0;

    return G4ThreeVector(x, y, z);
}
