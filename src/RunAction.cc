#include "RunAction.hh"
#include "G4Run.hh"

RunAction::RunAction(char *fFileName)
: G4UserRunAction(),
  fOutFile(nullptr),
  fTree(nullptr),
  fEventID(0),
  fVDEnergy(0.0),
  fFileName(fFileName)
{}

RunAction::~RunAction()
{
	if(fOutFile) {
        if(fOutFile->IsOpen()) fOutFile->Close();
        delete fOutFile;
	}
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    fOutFile = new TFile(fFileName, "RECREATE");
    fTree = new TTree("events","events per event");

    fTree->Branch("EventID", &fEventID);
    fTree->Branch("PrimaryEnergy", &fPrimaryEnergy);
    fTree->Branch("VD_energy", &fVDEnergy);

    fTree->Branch("EnergyTotal", &fETotal);
    fTree->Branch("NCherenkovTotal", &fNCherenkovTotal);
    fTree->Branch("ScintillationEnergyTotal", &fEScintillationTotal);

    fTree->Branch("Hit_ix", &fHit_ix);
    fTree->Branch("Hit_iy", &fHit_iy);
    fTree->Branch("Hit_iz", &fHit_iz);

    fTree->Branch("Vertex_x", &fVertexX);
    fTree->Branch("Vertex_y", &fVertexY);
    fTree->Branch("Vertex_z", &fVertexZ);


    fTree->Branch("Hit_x", &fHit_x);
    fTree->Branch("Hit_y", &fHit_y);
    fTree->Branch("Hit_z", &fHit_z);

    fTree->Branch("Hit_E", &fHit_E);
    fTree->Branch("Hit_NCherenkov", &fHit_NCherenkov);
    fTree->Branch("Hit_EScintillation", &fHit_EScintillation);
}

void RunAction::EndOfRunAction(const G4Run*)
{
    if(fOutFile) {
        fTree->Write();
        fOutFile->Close();
    }
}
