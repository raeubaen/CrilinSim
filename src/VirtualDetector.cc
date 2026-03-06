#include "VirtualDetector.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"

VirtualDetector::VirtualDetector(const G4String& name)
: G4VSensitiveDetector(name), totalE(0.)
{}


void VirtualDetector::Initialize(G4HCofThisEvent* /*HCE*/)
{
    // Reset energy at the beginning of each event
    totalE = 0.0;
}

G4bool VirtualDetector::ProcessHits(G4Step* step, G4TouchableHistory*) {
    G4Track* track = step->GetTrack();
    G4double edep = 0.;

    // Kinetic energy for baryons, total energy for others
    if(track->GetDefinition()->GetBaryonNumber() > 0 && track->GetDefinition()->GetPDGEncoding() > 0) {
        edep = track->GetKineticEnergy();
    } else {
        edep = track->GetTotalEnergy();
    }

    totalE += edep;

    // Stop the particle
    track->SetTrackStatus(fStopAndKill);

    return true;
}
