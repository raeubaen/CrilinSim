#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4EventManager.hh"
#include "G4VTrajectory.hh"
#include "G4SystemOfUnits.hh"

SteppingAction::SteppingAction()
: G4UserSteppingAction()
{}

SteppingAction::~SteppingAction()
{}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    // Get the track
    const G4Track* track = step->GetTrack();

    // Optionally, handle tracks or steps here
    // For instance, print track information for debugging:
    if (track->GetTrackID() == 1) { // Example condition
      /*  G4cout << "Track ID: " << track->GetTrackID() << ", Position: " 
               << track->GetPosition() << ", Momentum: " 
               << track->GetMomentum() << G4endl;*/
    }
}
