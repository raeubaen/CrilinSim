#ifndef PrimaryGeneratorMessenger_h
#define PrimaryGeneratorMessenger_h

#include "G4UImessenger.hh"

class PrimaryGeneratorAction;
class G4UIcmdWithADouble;

class PrimaryGeneratorMessenger : public G4UImessenger
{
public:
    PrimaryGeneratorMessenger(PrimaryGeneratorAction* action);
    ~PrimaryGeneratorMessenger();

    virtual void SetNewValue(G4UIcommand* command, G4String newValue);

private:
    PrimaryGeneratorAction* fAction;

    G4UIcmdWithADouble* fSigmaXCmd;
    G4UIcmdWithADouble* fSigmaYCmd;
};

#endif
