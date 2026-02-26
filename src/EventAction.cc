#include "EventAction.hh"
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh" // Include per la gestione della raccolta di hit
#include "CrystalHit.hh"
#include "G4AnalysisManager.hh" // Include per l'analisi dei dati
#include <map> // Include per la mappa

EventAction::EventAction()
: G4UserEventAction(), 
  analysisManager(G4AnalysisManager::Instance()), eventID(-1) // Inizializza l'analisi manager e l'ID dell'evento
{
    fCrystalHCID = -1; // Assicurati di inizializzare fCrystalHCID
}

EventAction::~EventAction()
{
    // Non è necessario eliminare l'istanza di G4AnalysisManager, gestisce la vita dell'oggetto automaticamente
}

void EventAction::BeginOfEventAction(const G4Event* event)
{

    // Memorizza l'ID dell'evento
    eventID = event->GetEventID();

    // Inizializza l'ID della raccolta se non è stato fatto
    if (fCrystalHCID == -1) {
        auto sdManager = G4SDManager::GetSDMpointer();
        fCrystalHCID = sdManager->GetCollectionID("CrystalHitsCollection");
    }
}

void EventAction::EndOfEventAction(const G4Event* event)
{
    // Retrieve the hits collection
    auto hitsCollection = static_cast<G4THitsCollection<CrystalHit>*>(event->GetHCofThisEvent()->GetHC(fCrystalHCID));

    if (hitsCollection) {
        // Mappa per accumulare l'energia totale per ogni cristallo
        std::map<G4int, G4double> energyMap;

        // Accumula l'energia depositata per ciascun cristallo
        for (G4int i = 0; i < hitsCollection->GetSize(); ++i) {
            CrystalHit* hit = (*hitsCollection)[i];
            G4int crystalID = hit->GetID();    // Get crystal ID
            G4double edep = hit->GetEnergyDep();    // Get deposited energy

            // Somma l'energia per ciascun ID di cristallo
            energyMap[crystalID] += edep;
        }

        // Stampa dell'energia totale per ciascun cristallo
        for (const auto& entry : energyMap) {
            G4int crystalID = entry.first;
            G4double totalEnergy = entry.second / CLHEP::MeV; // Convert energy to MeV

            //G4cout << "Event " << eventID << ", Crystal ID: " << crystalID << ", Total Energy Deposited: " << totalEnergy << " MeV" << G4endl;

            // Write to the ROOT file
            analysisManager->FillNtupleIColumn(0, eventID);
            analysisManager->FillNtupleIColumn(1, crystalID);
            analysisManager->FillNtupleDColumn(2, totalEnergy); // Convert energy to MeV
            analysisManager->AddNtupleRow();
        }
        G4cout << "Event " << eventID << G4endl;
    }
}

