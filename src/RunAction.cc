#include "RunAction.hh"
#include "G4Run.hh"
#include "G4AnalysisManager.hh" // Include il file di intestazione completo

RunAction::RunAction()
: G4UserRunAction(), 
  analysisManager(G4AnalysisManager::Instance()) // Inizializza l'analisi manager
{
}

RunAction::~RunAction()
{
    // Non è necessario eliminare l'istanza di G4AnalysisManager, gestisce la vita dell'oggetto automaticamente
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    // Setup analysis manager for ROOT
    analysisManager->OpenFile("../data/output.root");
    analysisManager->CreateNtuple("Hits", "Hits");
    analysisManager->CreateNtupleIColumn("EventID");
    analysisManager->CreateNtupleIColumn("CrystalID");
    analysisManager->CreateNtupleDColumn("EnergyDeposited");
    analysisManager->FinishNtuple();
}

void RunAction::EndOfRunAction(const G4Run*)
{
    // Close the ROOT file
    analysisManager->CloseFile();
}
