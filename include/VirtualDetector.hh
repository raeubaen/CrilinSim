#ifndef VIRTUALDETECTOR_H
#define VIRTUALDETECTOR_H

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"

class VirtualDetector : public G4VSensitiveDetector {
public:
    VirtualDetector(const G4String& name);
    virtual ~VirtualDetector() {}
    virtual void Initialize(G4HCofThisEvent* hce) override;
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory*);

    G4double GetTotalEnergy() const { return totalE; }

private:
    G4double totalE;
};

#endif
