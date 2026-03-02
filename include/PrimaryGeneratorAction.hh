#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"

class G4ParticleGun;
class PrimaryGeneratorMessenger;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event*);

    // Spot size setters (optional, controlled via messenger)
    void SetSigmaX(G4double val) { fSigmaX = val; }
    void SetSigmaY(G4double val) { fSigmaY = val; }

private:
    G4ParticleGun* fParticleGun;
    PrimaryGeneratorMessenger* fMessenger;

    // Gaussian transverse spot
    G4double fSigmaX;
    G4double fSigmaY;
};

#endif
