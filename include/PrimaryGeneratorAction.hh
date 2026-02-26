// PrimaryGeneratorAction.hh

#ifndef PRIMARY_GENERATOR_ACTION_HH
#define PRIMARY_GENERATOR_ACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event*);

    void SetBeamEnergy(G4double energy);
    void SetBeamSmearing(G4double energySmearing, G4double directionSmearing);

private:
    G4ParticleGun* fParticleGun;
    G4double fBeamEnergy;
    G4double fEnergySmearing;
    G4double fDirectionSmearing;
};

#endif
