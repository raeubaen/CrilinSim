#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "TFile.h"
#include "TTree.h"
#include <vector>

class RunAction : public G4UserRunAction {
public:
    RunAction();
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*) override;
    virtual void EndOfRunAction(const G4Run*) override;
    TTree* GetTree() {return fTree;}
    // Tree & file are public so EventAction can fill them
    // Branch data
    int fEventID;
    double fPrimaryEnergy;
    double fVDEnergy;
    double fETotal;
    double fNCherenkovTotal;
    double fEScintillationTotal;

    std::vector<int> fHit_ix;
    std::vector<int> fHit_iy;
    std::vector<int> fHit_iz;

    std::vector<double> fHit_x;
    std::vector<double> fHit_y;
    std::vector<double> fHit_z;

    std::vector<double> fHit_E;
    std::vector<double> fHit_NCherenkov;
    std::vector<double> fHit_EScintillation;
private:
    TTree *fTree;
    TFile *fOutFile;
};
#endif
