#ifndef CRYSTAL_HIT_HH
#define CRYSTAL_HIT_HH

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

class CrystalHit : public G4VHit {
public:
    CrystalHit();
     virtual ~CrystalHit();

    void AddEnergy(G4double energy);
    G4double GetEnergyDep() const;
    G4double GetTotalEnergy() const;
    G4int GetID() const;
    void SetID(G4int id); // Dichiarazione del setter
    void Draw();
    void Print() const;

private:
    G4double energyDep;
  //    G4int id; BOH!
    G4int fID; // ID del cristallo
    G4double fTotalEnergy;   // Energia totale depositata
};

#endif
