#ifndef CRYSTAL_HIT_HH
#define CRYSTAL_HIT_HH

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class CrystalHit : public G4VHit {
public:
    CrystalHit()
        : ix(-1), iy(-1), iz(-1), pos(G4ThreeVector()), edep(0.), ech(0.), esc(0.) {}
    virtual ~CrystalHit() {}

    // Setters
    void SetIndices(G4int i, G4int j, G4int k) { ix=i; iy=j; iz=k; }
    void SetPos(const G4ThreeVector& p) { pos = p; }
    void AddEnergy(G4double e) { edep += e; }
    void AddCherenkovEnergy(G4double e) { ech += e; }
    void AddScintillationEnergy(G4double e) { esc += e; }

    // Getters
    G4int GetIx() const { return ix; }
    G4int GetIy() const { return iy; }
    G4int GetIz() const { return iz; }
    G4ThreeVector GetPos() const { return pos; }
    G4double GetEnergyDep() const { return edep; }
    G4double GetCherenkovEnergyDep() const { return ech; }
    G4double GetScintillationEnergyDep() const { return esc; }

private:
    G4int ix, iy, iz;          // indices in the crystal grid
    G4ThreeVector pos;         // absolute position in world coordinates
    G4double edep;             // deposited energy
    G4double ech;             // deposited energy measured from Cherenkov light
    G4double esc;             // deposited energy measured from Scintillation light
};

typedef G4THitsCollection<CrystalHit> CrystalHitsCollection;

#endif
