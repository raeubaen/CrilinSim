// CrystalSD.hh

#ifndef CrystalSD_h
#define CrystalSD_h

#include "G4VSensitiveDetector.hh"
#include "G4THitsCollection.hh"
#include "G4VHit.hh"
#include "globals.hh"

class CrystalHit;  // Forward declaration

class CrystalSD : public G4VSensitiveDetector {
public:
    CrystalSD(const G4String& name);
    virtual ~CrystalSD();

    virtual void Initialize(G4HCofThisEvent* hce);
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* hist);
    virtual void EndOfEvent(G4HCofThisEvent* hce);

private:
    G4THitsCollection<CrystalHit>* fHitsCollection;
};

#endif
