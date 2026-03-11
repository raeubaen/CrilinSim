#include "PrimaryGeneratorAction.hh"
#include "PrimaryGeneratorMessenger.hh"

#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(new G4ParticleGun(1)),
  fMessenger(nullptr),
  fSigmaX(2.65*mm),
  fSigmaY(2.32*mm)
{
    // Default particle: electron
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("e-");
    fParticleGun->SetParticleDefinition(particle);

    // Default direction along -z
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., -1.));
    fParticleGun->SetParticleEnergy(240*MeV);

    // Default position center (will be smeared in GeneratePrimaries)
    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., 30*cm));

    // Create messenger to control sigmaX/Y
    fMessenger = new PrimaryGeneratorMessenger(this);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
    delete fMessenger;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    // Central position from /gun/position
    //G4ThreeVector center = fParticleGun->GetParticlePosition();

    // Gaussian smear in x and y only
    //G4double x = G4RandGauss::shoot(center.x(), fSigmaX);
    //G4double y = G4RandGauss::shoot(center.y(), fSigmaY);
    //G4double z = center.z();

    //fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));

    // Energy and direction fully controlled by /gun/
    fParticleGun->GeneratePrimaryVertex(anEvent);
}
