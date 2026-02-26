#ifndef CRYSTALHITCOLLECTION_HH
#define CRYSTALHITCOLLECTION_HH

#include "G4THitsCollection.hh"
#include "CrystalHit.hh"

class CrystalHitsCollection : public G4THitsCollection<CrystalHit> {
public:
    CrystalHitsCollection(const G4String& name, const G4String& hcName);
    virtual ~CrystalHitsCollection();

    // Aggiungi metodi specifici se necessari

private:
    // Aggiungi variabili membri specifiche se necessari
};

#endif // CRYSTALHITCOLLECTION_HH
