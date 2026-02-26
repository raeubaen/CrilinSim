#ifndef RunAction_h
#define RunAction_h

#include "G4UserRunAction.hh"
#include "G4AnalysisManager.hh" // Include il file di intestazione completo

class RunAction : public G4UserRunAction {
public:
    RunAction();
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

private:
    G4AnalysisManager* analysisManager; // Analysis manager for ROOT
};

#endif
