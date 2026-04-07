#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"
#include "RunAction.hh"
#include <vector>
#include "CrystalSD.hh"

class CrystalHit;

class EventAction : public G4UserEventAction {
public:
    EventAction(RunAction* runAction);
    virtual ~EventAction() {}

    virtual void BeginOfEventAction(const G4Event* event) override;
    virtual void EndOfEventAction(const G4Event* event) override;

private:
    RunAction* fRunAction;  // pointer to RunAction
    CrystalSD* fCrystalSD;   // pointer to the SD
    int fCrystalHCID;
    G4double fPrimaryEnergy;
    G4int fNcryX;
    G4int fNcryY;
    G4int fNlayer;
    std::vector<int> fHit_ix, fHit_iy, fHit_iz;
    std::vector<double> fHit_x, fHit_y, fHit_z, fHit_E, fHit_ECherenkov, fHit_EScintillation;
    double fVDEnergy;
    double fETotal;
    double fECherenkovTotal;
    double fEScintillationTotal;
};

#endif
