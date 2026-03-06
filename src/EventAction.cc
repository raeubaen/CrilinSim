#include "EventAction.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "CrystalHit.hh"
#include "VirtualDetector.hh"

EventAction::EventAction(RunAction* runAction)
: G4UserEventAction(), fRunAction(runAction), fCrystalHCID(-1), fVDEnergy(0.0)
{
 G4SDManager* sdman = G4SDManager::GetSDMpointer();
    G4VSensitiveDetector* sd = sdman->FindSensitiveDetector("CrystalSD");
    fCrystalSD = dynamic_cast<CrystalSD*>(sd);


    if (fCrystalSD) {
        fNcryX = fCrystalSD->GetNcryX();
        fNcryY = fCrystalSD->GetNcryY();
        fNlayer = fCrystalSD->GetNlayer();
        G4cout << "EventAction: found CrystalSD with " 
               << fNcryX << " x " << fNcryY << " x " << fNlayer << G4endl;
    } else {
        G4Exception("EventAction::EventAction()",
                    "MyCode0002", JustWarning,
                    "CrystalSD not found! Geometry info unavailable.");
    }

}

void EventAction::BeginOfEventAction(const G4Event* event)
{
    fHit_ix.clear();
    fHit_iy.clear();
    fHit_iz.clear();
    fHit_x.clear();
    fHit_y.clear();
    fHit_z.clear();
    fHit_E.clear();
    fVDEnergy = 0.0;

    if(fCrystalHCID == -1)
        fCrystalHCID = G4SDManager::GetSDMpointer()->GetCollectionID("CrystalHitsCollection");
}

void EventAction::EndOfEventAction(const G4Event* event)
{
    auto hc = event->GetHCofThisEvent();
    if (!hc) return;

    // --- Crystal hits
    auto hitsCollection = static_cast<G4THitsCollection<CrystalHit>*>(hc->GetHC(fCrystalHCID));
    std::map<int, CrystalHit*> crystalMap;

    if (hitsCollection) {
        for (int i = 0; i < hitsCollection->entries(); i++) {
            auto hit = (*hitsCollection)[i];
            if (hit->GetEnergyDep() <= 0.) continue;

            // Compute unique crystal ID
            int id = hit->GetIx() + hit->GetIy() * fNcryX + hit->GetIz() * fNcryX * fNcryY;

            // Only one hit per crystal
            if (crystalMap.find(id) == crystalMap.end()) {
                // Clone the first hit
                CrystalHit* newHit = new CrystalHit(*hit);
                crystalMap[id] = newHit;
            } else {
                // Accumulate energy for repeated hits
                crystalMap[id]->AddEnergy(hit->GetEnergyDep());
            }
        }

        // Fill event vectors from the map
        for (const auto& [id, hit] : crystalMap) {
            fHit_ix.push_back(hit->GetIx());
            fHit_iy.push_back(hit->GetIy());
            fHit_iz.push_back(hit->GetIz());

            G4ThreeVector center = fCrystalSD->GetCrystalCenter(hit->GetIx(), hit->GetIy(), hit->GetIz());
            fHit_x.push_back(center.x());
            fHit_y.push_back(center.y());
            fHit_z.push_back(center.z());

            fHit_E.push_back(hit->GetEnergyDep() / CLHEP::MeV);

            delete hit;  // free the cloned hit
        }
    }

    // --- Virtual Detector
    auto vd = static_cast<VirtualDetector*>(G4SDManager::GetSDMpointer()->FindSensitiveDetector("VD"));
    fVDEnergy = vd ? vd->GetTotalEnergy() / CLHEP::MeV : 0.;

    // --- Fill the tree via RunAction
    fRunAction->fEventID = event->GetEventID();
    fRunAction->fHit_ix = fHit_ix;
    fRunAction->fHit_iy = fHit_iy;
    fRunAction->fHit_iz = fHit_iz;

    fRunAction->fHit_x = fHit_x;
    fRunAction->fHit_y = fHit_y;
    fRunAction->fHit_z = fHit_z;
    fRunAction->fHit_E = fHit_E;

    fRunAction->fVDEnergy = fVDEnergy;

    fRunAction->GetTree()->Fill();

    // --- Clear vectors for next event
    fHit_ix.clear();
    fHit_iy.clear();
    fHit_iz.clear();
    fHit_x.clear();
    fHit_y.clear();
    fHit_z.clear();
    fHit_E.clear();
    fVDEnergy = 0.;
}
