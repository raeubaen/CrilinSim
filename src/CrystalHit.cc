#include "CrystalHit.hh"
#include "G4VVisManager.hh"

CrystalHit::CrystalHit() : energyDep(0.), fID(-1), fTotalEnergy(0.) {}

CrystalHit::~CrystalHit() {}

void CrystalHit::AddEnergy(G4double energy) {
    energyDep += energy;
}

G4double CrystalHit::GetTotalEnergy() const {
    return fTotalEnergy;
}

G4double CrystalHit::GetEnergyDep() const {
    return energyDep;
}

G4int CrystalHit::GetID() const {
    return fID;
}

void CrystalHit::SetID(G4int id) {
    fID = id; // Implementazione corretta
}

void CrystalHit::Draw() {
    // Implementazione del disegno
}

void CrystalHit::Print() const {
    G4cout << "Energy Deposited: " << energyDep / CLHEP::MeV << " MeV" << G4endl;
}
