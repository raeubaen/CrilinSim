// CrystalSD.hh

#ifndef CrystalSD_h
#define CrystalSD_h
#include "G4VSensitiveDetector.hh"
#include "G4THitsCollection.hh"
#include "G4VHit.hh"
#include "globals.hh"
#include "CrystalHit.hh"

class CrystalHit;  // Forward declaration

class CrystalSD : public G4VSensitiveDetector {
public:
    CrystalSD(const G4String& name, G4int nx, G4int ny, G4int nz);
    virtual ~CrystalSD();
    virtual void Initialize(G4HCofThisEvent* hce) override;
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
    G4int GetNcryX() { return fNcryX; };
    G4int GetNcryY() { return fNcryY; };
    G4int GetNlayer() { return fNlayer; };
    G4ThreeVector GetCrystalCenter(G4int ix, G4int iy, G4int iz) const;

private:
    G4int fNcryX;
    G4int fNcryY;
    G4int fNlayer;
    CrystalHitsCollection* fHitsCollection;
    std::vector<CrystalHit*> fCrystalHitMap;
    G4double fCrystalPitchX, fCrystalPitchY, fCrystalPitchZ;
    G4ThreeVector fOrigin;

};

#endif
