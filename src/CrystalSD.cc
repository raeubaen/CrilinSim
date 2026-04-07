// CrystalSD.cc
#include "CrystalSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "CrystalHit.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VTouchable.hh"
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include <vector>

CrystalSD::CrystalSD(const G4String& name, G4int nx, G4int ny, G4int nz)
    : G4VSensitiveDetector(name),
      fHitsCollection(nullptr),
      fNcryX(nx),
      fNcryY(ny),
      fNlayer(nz)
{
    collectionName.push_back("CrystalHitsCollection");

}

CrystalSD::~CrystalSD()
{
    fHitsCollection = nullptr;  // SDManager owns the hits
}

void CrystalSD::Initialize(G4HCofThisEvent* hce)
{
    // Create hits collection
    fHitsCollection = new CrystalHitsCollection(SensitiveDetectorName, collectionName[0]);

    // Register the hits collection with the SD manager
    G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    hce->AddHitsCollection(hcID, fHitsCollection);

    // Clear map
    fCrystalHitMap.clear();

    // Pre-create hits for all crystals in this SD
    for (G4int iz = 0; iz < fNlayer; ++iz) {
        for (G4int iy = 0; iy < fNcryY; ++iy) {
            for (G4int ix = 0; ix < fNcryX; ++ix) {
                G4int id = ix + iy * fNcryX + iz * fNcryX * fNcryY;
                CrystalHit* hit = new CrystalHit();
                hit->SetIndices(ix, iy, iz);

                hit->SetPos(fGeometry->GetCrystalCenter(ix, iy, iz));

                fHitsCollection->insert(hit);
                fCrystalHitMap.push_back(hit);
            }
        }
    }
}

G4bool CrystalSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    G4double edep = step->GetTotalEnergyDeposit();
    if (edep == 0.) return false;

    const G4VTouchable* touchable = step->GetPreStepPoint()->GetTouchable();
    G4int crystalID = touchable->GetReplicaNumber(0);
    //std::cout << "crystalID beccato: " << crystalID << std::endl;
    if (crystalID < 0 || crystalID >= static_cast<G4int>(fCrystalHitMap.size())) return false;

    //Compute energy released via Cherenkov light

    G4double edepCh = 0;
    G4double edepSc = 0;

    G4double length = step->GetStepLength(); // dx (mm)
    G4double p = step->GetTrack()->GetMomentum().mag();
    G4double E = step->GetTrack()->GetTotalEnergy();
    G4double beta = 0; 
    if (p>0 && E>0) beta = p / E; //beta
    G4double n = 1.82; //avarage n for PbF2 in the photon energy range

    if (length>0 && beta>1./n) {

    	G4double charge = step->GetTrack()->GetParticleDefinition()->GetPDGCharge(); //Charge

    	G4double beta = p / E; //beta

    	//Typical energy range for the PDE peak of a SiPM
    	G4double Emin = 2.0e-6; //MeV, corresponds to 620 nm
    	G4double Emax = 3.1e-6; //MeV, corresponds to 400 nm 

    	edepCh = length * 0.232 * charge * charge * (1. - 1./(beta*beta*n*n)) * 0.5 * (Emax*Emax-Emin*Emin); //energy released as Cherenkov light in MeV

    	//Compute scintillation energy, assuming Birks law parameters for PbWO4. Ioniziation is similar for PbF2 and PbWO4

    	G4double S = 200; //photons/MeV
    	G4double kB = 0.005; //mm/MeV

    	G4double lightyield = S*edep/(1+kB*edep/length); //photons per MeV
    	edepSc = 2.95e-6 * lightyield; //assuming PbWO4 scintillation peak at 420 nm, 2.95 eV per photon  

    	}

    // Add energy
    fCrystalHitMap[crystalID]->AddEnergy(edep);
    fCrystalHitMap[crystalID]->AddCherenkovEnergy(edepCh);
    fCrystalHitMap[crystalID]->AddScintillationEnergy(edepSc);

    return true;
}

