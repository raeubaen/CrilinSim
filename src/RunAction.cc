#include "RunAction.hh"
#include "G4Run.hh"

RunAction::RunAction()
: G4UserRunAction(),
  fOutFile(nullptr),
  fTree(nullptr),
  fEventID(0),
  fVDEnergy(0.0)
{}

RunAction::~RunAction()
{
        fOutFile->Close();
        delete fOutFile;
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    fOutFile = new TFile("../data/output.root","RECREATE");
    fTree = new TTree("events","events per event");

    fTree->Branch("EventID", &fEventID);
    fTree->Branch("VD_energy", &fVDEnergy);

    fTree->Branch("Hit_ix", &fHit_ix);
    fTree->Branch("Hit_iy", &fHit_iy);
    fTree->Branch("Hit_iz", &fHit_iz);

    fTree->Branch("Hit_x", &fHit_x);
    fTree->Branch("Hit_y", &fHit_y);
    fTree->Branch("Hit_z", &fHit_z);

    fTree->Branch("Hit_E", &fHit_E);
}

void RunAction::EndOfRunAction(const G4Run*)
{
    if(fOutFile) {
        fTree->Write();
        fOutFile->Close();
    }
}
