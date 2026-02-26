#include "CrystalHitsCollection.hh"

// Costruttore
CrystalHitsCollection::CrystalHitsCollection(const G4String& name, const G4String& hcName)
    : G4THitsCollection<CrystalHit>(name, hcName) {}

// Distruttore
CrystalHitsCollection::~CrystalHitsCollection() {}

// Implementazione di eventuali metodi specifici
