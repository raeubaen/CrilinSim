// CrystalSD.cc
#include "CrystalSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "CrystalHit.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VTouchable.hh"
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include <vector>

CrystalSD::CrystalSD(const G4String& name, G4int nx, G4int ny, G4int nz)
    : G4VSensitiveDetector(name),
      fHitsCollection(nullptr),
      fNcryX(nx),
      fNcryY(ny),
      fNlayer(nz)
{
    collectionName.push_back("CrystalHitsCollection");

}

CrystalSD::~CrystalSD()
{
    fHitsCollection = nullptr;  // SDManager owns the hits
}

void CrystalSD::Initialize(G4HCofThisEvent* hce)
{
    // Create hits collection
    fHitsCollection = new CrystalHitsCollection(SensitiveDetectorName, collectionName[0]);

    // Register the hits collection with the SD manager
    G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    hce->AddHitsCollection(hcID, fHitsCollection);

    // Clear map
    fCrystalHitMap.clear();

    // Pre-create hits for all crystals in this SD
    for (G4int iz = 0; iz < fNlayer; ++iz) {
        for (G4int iy = 0; iy < fNcryY; ++iy) {
            for (G4int ix = 0; ix < fNcryX; ++ix) {
                G4int id = ix + iy * fNcryX + iz * fNcryX * fNcryY;
                CrystalHit* hit = new CrystalHit();
                hit->SetIndices(ix, iy, iz);

                // Absolute position in world coordinates
                const G4VTouchable* touchable = nullptr;  // placeholder, real pos filled in ProcessHits
                hit->SetPos(G4ThreeVector(0.,0.,0.));

                fHitsCollection->insert(hit);
                fCrystalHitMap.push_back(hit);
            }
        }
    }
}

G4bool CrystalSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    G4double edep = step->GetTotalEnergyDeposit();
    if (edep == 0.) return false;

    const G4VTouchable* touchable = step->GetPreStepPoint()->GetTouchable();
    G4int crystalID = touchable->GetReplicaNumber(0);

    if (crystalID < 0 || crystalID >= static_cast<G4int>(fCrystalHitMap.size())) return false;

    // Add energy
    fCrystalHitMap[crystalID]->AddEnergy(edep);

    // Set absolute position (center of the step)
    G4ThreeVector stepPos = step->GetPreStepPoint()->GetPosition();
    fCrystalHitMap[crystalID]->SetPos(stepPos);

    return true;
}
