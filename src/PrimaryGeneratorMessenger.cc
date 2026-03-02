#include "PrimaryGeneratorMessenger.hh"
#include "PrimaryGeneratorAction.hh"

#include "G4UIcmdWithADouble.hh"
#include "G4UIdirectory.hh"

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* action)
: fAction(action)
{
    G4UIdirectory* dir = new G4UIdirectory("/beam/");
    dir->SetGuidance("Beam control commands");

    fSigmaXCmd = new G4UIcmdWithADouble("/beam/sigmaX", this);
    fSigmaXCmd->SetGuidance("Set Gaussian sigma in X [length]");
    fSigmaXCmd->SetParameterName("SigmaX", true);
    fSigmaXCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fSigmaYCmd = new G4UIcmdWithADouble("/beam/sigmaY", this);
    fSigmaYCmd->SetGuidance("Set Gaussian sigma in Y [length]");
    fSigmaYCmd->SetParameterName("SigmaY", true);
    fSigmaYCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
    delete fSigmaXCmd;
    delete fSigmaYCmd;
}

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (command == fSigmaXCmd)
        fAction->SetSigmaX(fSigmaXCmd->GetNewDoubleValue(newValue));
    else if (command == fSigmaYCmd)
        fAction->SetSigmaY(fSigmaYCmd->GetNewDoubleValue(newValue));
}
