// DetectorConstruction.cc

#include "DetectorConstruction.hh"
#include "G4SDManager.hh"
#include "CrystalSD.hh" // Includi il nuovo header
#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "G4Tubs.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

// Constructor
DetectorConstruction::DetectorConstruction()
: fCrystalMaterial(nullptr), fMylar(nullptr), fAluminum(nullptr), fFR4(nullptr), fSiPM(nullptr), fLogicCrystal(nullptr) {}

// Destructor
DetectorConstruction::~DetectorConstruction() {}

// Define the materials
void DetectorConstruction::DefineMaterials() {
  
  // Define elements
  G4double a;  // Atomic mass
  G4double z;  // Atomic number
  G4double density;
  // Lead (Pb)
  G4Element* Pb = new G4Element("Lead", "Pb", z=82., a=207.2*g/mole);
  // Fluorine (F)
  G4Element* F = new G4Element("Fluorine", "F", z=9., a=19.00*g/mole);
  
  // Define Lead Fluoride (PbF2) material
  density = 7.83*g/cm3;  // Density of PbF2   // IVANO 
  //density = 8.44*g/cm3;  // Density of PbF2   DANIELE
  G4Material* PbF2 = new G4Material("LeadFluoride", density, 2);
  PbF2->AddElement(Pb, 1);
  PbF2->AddElement(F, 2);

  // Define other materials from NIST database
  G4NistManager* nist = G4NistManager::Instance();
  //    fCrystalMaterial = nist->FindOrBuildMaterial("G4_PbF2");
  fMylar = nist->FindOrBuildMaterial("G4_MYLAR");
  fAluminum = nist->FindOrBuildMaterial("G4_Al");

  // Define SiPMs
  fSiPM = nist->FindOrBuildMaterial("G4_Si");

  /////////////////////
  // Define Electronics
    G4Element* H = nist->FindOrBuildElement("H");
    G4Element* C = nist->FindOrBuildElement("C");
    G4Element* O = nist->FindOrBuildElement("O");
    G4Element* Si = nist->FindOrBuildElement("Si");
    G4Element* Cu = nist->FindOrBuildElement("Cu");

    // Definizione del materiale FR4 (fibra di vetro e resina epossidica)
    G4Material* EpoxyResin = new G4Material("EpoxyResin", 1.2*g/cm3, 2);
    EpoxyResin->AddElement(H, 2);
    EpoxyResin->AddElement(C, 2);

    G4Material* Fiberglass = new G4Material("Fiberglass", 2.6*g/cm3, 2);
    Fiberglass->AddElement(Si, 1);
    Fiberglass->AddElement(O, 2);

    // Definizione del rame (per le tracce di rame)
    G4Material* Copper = nist->FindOrBuildMaterial("G4_Cu");

    // Definizione del materiale PCB come composito
    G4Material* PCB = new G4Material("PCB", 1.85*g/cm3, 3);
    PCB->AddMaterial(EpoxyResin, 0.4); // 40% resina epossidica
    PCB->AddMaterial(Fiberglass, 0.5); // 50% fibra di vetro
    PCB->AddMaterial(Copper, 0.1);     // 10% rame (dipende dallo spessore delle tracce)

    // Definizione del materiale Kapton (poliimmide)
    fKapton = nist->FindOrBuildMaterial("G4_KAPTON");
    
    /////////////////////
    /////////////////////
    
    // Assign the defined materials to member variables
    fCrystalMaterial = PbF2;
    //fFR4 = nist->FindOrBuildMaterial("G4_POLYETHYLENE"); // Approximation for FR4 // IVANO (credo sbagliato)
    fFR4 = PCB;
}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    DefineMaterials();
    return ConstructCalorimeter();
}

G4VPhysicalVolume* DetectorConstruction::ConstructCalorimeter() {
    // World
    G4double worldSizeXYZ = 1.2 * m;
    G4Material* worldMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");

    G4Box* solidWorld = new G4Box("World", worldSizeXYZ, worldSizeXYZ, worldSizeXYZ);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, worldMaterial, "World");

    G4VPhysicalVolume* physWorld = new G4PVPlacement(nullptr, G4ThreeVector(), logicWorld, "World", nullptr, false, 0);

    // Crystals
    G4double crystalSizeXY = 1.3 * cm;
    G4double crystalLength = 4.0 * cm;
    G4double wrapThickness = 0.01 * cm;
    //G4double honeycombThickness = 0.015 * cm;
    G4double honeycombThickness = 0.010 * cm;

    int NcryX = 7;
    int NcryY = 7;
    int Nlayer = 5;

    G4Box* solidCrystal = new G4Box("Crystal", crystalSizeXY / 2, crystalSizeXY / 2, crystalLength / 2);
    fLogicCrystal = new G4LogicalVolume(solidCrystal, fCrystalMaterial, "Crystal");

    G4double wrapcrystalSizeXY = crystalSizeXY + 2 * wrapThickness; //non metto il wrapping davanti per ora
    
    // Wrapping (Mylar)
    G4Box* solidWrap = new G4Box("Wrap", wrapcrystalSizeXY / 2, wrapcrystalSizeXY / 2, crystalLength / 2);
    G4SubtractionSolid* solidCrystalWrap = new G4SubtractionSolid("CrystalWrap", solidWrap, solidCrystal, nullptr, G4ThreeVector());
    G4LogicalVolume* logicWrap = new G4LogicalVolume(solidCrystalWrap, fMylar, "Wrap");

    G4double honeywrapcrystalSizeXY =  wrapcrystalSizeXY + 2 * honeycombThickness;
    
    // Honeycomb structure (Aluminum)
    G4Box* solidHoneycomb = new G4Box("Honeycomb", honeywrapcrystalSizeXY / 2, honeywrapcrystalSizeXY / 2, crystalLength / 2);
    G4SubtractionSolid* solidCrystalHoneycomb = new G4SubtractionSolid("CrystalHoneycomb", solidHoneycomb, solidWrap, nullptr, G4ThreeVector());
    G4LogicalVolume* logicHoneycomb = new G4LogicalVolume(solidCrystalHoneycomb, fAluminum, "Honeycomb");
    
    // Electronics
    G4double electronicsThickness = 0.2 * cm; 
    G4Box* solidElectronics = new G4Box("Electronics", (NcryX * honeywrapcrystalSizeXY) / 2 + 1.0 * mm, (NcryY * honeywrapcrystalSizeXY) / 2 + 1.0 * mm, electronicsThickness / 2);
    G4LogicalVolume* logicElectronics = new G4LogicalVolume(solidElectronics, fFR4, "Electronics");

    // SiPMs 
    G4double siPMSizeXY = 0.3 * cm;
    G4double siPMThickness = 0.2 * cm;
    G4double siPMOffset = (crystalSizeXY - (3. * siPMSizeXY)) / 2.0;
    G4Box* solidSiPM = new G4Box("SiPM", siPMSizeXY / 2, siPMSizeXY / 2, siPMThickness / 2); //levato il diviso 2 in una direzione per considerare 2 SiPM
    G4LogicalVolume* logicSiPM = new G4LogicalVolume(solidSiPM, fSiPM, "SiPM");

    // Creazione della shell di alluminio
    G4double aluminumThickness = 0.2 * cm; // 2 mm per lato
    G4double aluminumTopScavo = 0.1 * cm; // Scavo di 1 mm
    G4double aluminumScavoWidth = 8.0 * cm; // 4 cm a sinistra e 4 cm a destra dal centro
    
    G4Box* solidOuterAluminum = new G4Box("OuterAluminum", 
					  (NcryX * honeywrapcrystalSizeXY + 2 * aluminumThickness) / 2,
					  (NcryY * honeywrapcrystalSizeXY + 2 * aluminumThickness) / 2,
					  crystalLength / 2);
    
    G4Box* solidInnerAluminum = new G4Box("InnerAluminum", 
					  (NcryX * honeywrapcrystalSizeXY ) / 2,
					  (NcryY * honeywrapcrystalSizeXY ) / 2,
					  crystalLength / 2);
    G4SubtractionSolid* solidAluminum1 = new G4SubtractionSolid("AluminumShell1", solidOuterAluminum, solidInnerAluminum);
    
    G4Box* solidScavoAluminum = new G4Box("ScavoAluminum", 
					  aluminumScavoWidth / 2, aluminumTopScavo / 2,
					  (crystalLength + 0.05 * cm) /2 ); // aggiunto uno scasso in z maggiore nella sottrazione rispetto ai cristalli per sicurezza
    G4ThreeVector scavoPos_alto(0, (NcryY * honeywrapcrystalSizeXY + 2 * aluminumThickness) / 2 - aluminumTopScavo / 2 + 0.01 * mm, 0); //aggiunti 20um per la parte grafica ^_^
    G4ThreeVector scavoPos_basso(0, -1*(NcryY * honeywrapcrystalSizeXY + 2 * aluminumThickness) / 2 + aluminumTopScavo / 2 - 0.01 * mm, 0); //sottratti 20um per la parte grafica ^_^
    G4SubtractionSolid* solidAluminum = new G4SubtractionSolid("AluminumShell", solidAluminum1, solidScavoAluminum, nullptr, scavoPos_alto);
    solidAluminum = new G4SubtractionSolid("AluminumShell", solidAluminum, solidScavoAluminum, nullptr, scavoPos_basso);
    
    G4LogicalVolume* logicAluminum = new G4LogicalVolume(solidAluminum, fAluminum, "AluminumShell");

    // Definizione della striscia di Kapton
    G4double kaptonWidth = 8.0 * cm; // 4 cm a sinistra + 4 cm a destra dal centro
    G4double kaptonThickness = 0.035 * cm; // 300 µm di spessore
    kaptonThickness *= Nlayer;
    G4double kaptonLength = (crystalLength + siPMThickness +  electronicsThickness); // Lungo tutta la lunghezza del calorimetro

    G4Box* solidKaptonStrip = new G4Box("KaptonStrip", kaptonWidth / 2, kaptonThickness / 2, kaptonLength / 2);
    G4LogicalVolume* logicKaptonStrip = new G4LogicalVolume(solidKaptonStrip, fKapton, "KaptonStrip");
    
    //////////////////////////////////////////////////////
    //////////////////////////////////////////////////////

    G4ThreeVector crystalPos(0, 0, crystalLength / 2);
    G4ThreeVector wrapPos = crystalPos;
    G4ThreeVector honeycombPos = wrapPos;
    G4ThreeVector electronicsPos(0, 0, crystalLength + siPMThickness + electronicsThickness / 2);
    G4ThreeVector aluminumPos(0, 0, crystalLength / 2);
    G4ThreeVector kaptonStripPos(0, (NcryY * honeywrapcrystalSizeXY + 2 * aluminumThickness) / 2 - aluminumTopScavo + 0.01 * mm + kaptonThickness / 2 , (crystalLength + siPMThickness) / 2  );


    // Placement
    for (int k = 0; k < Nlayer; ++k) {      
        for (int i = 0; i < NcryX; ++i) {
            for (int j = 0; j < NcryY; ++j) {

	      G4ThreeVector crystalShift(i * (honeywrapcrystalSizeXY), j * (honeywrapcrystalSizeXY), k * (crystalLength + electronicsThickness + siPMThickness));
	      G4ThreeVector sipmBasePos = crystalPos + crystalShift + G4ThreeVector(0, 0, crystalLength / 2 + siPMThickness / 2);

	      new G4PVPlacement(nullptr, crystalPos + crystalShift, fLogicCrystal, "Crystal", logicWorld, false, i + j * NcryY + k * NcryX * NcryY);
	      new G4PVPlacement(nullptr, wrapPos + crystalShift, logicWrap, "Wrap", logicWorld, false,  i + j * NcryY + k * NcryX * NcryY);
	      new G4PVPlacement(nullptr, honeycombPos + crystalShift, logicHoneycomb, "Honeycomb", logicWorld, false,  i + j * NcryY + k * NcryX * NcryY);

	      G4ThreeVector sipmShifts[4] = {
		G4ThreeVector(+siPMOffset, +siPMOffset, 0),
		G4ThreeVector(+siPMOffset, -siPMOffset, 0),
		G4ThreeVector(-siPMOffset, +siPMOffset, 0),
		G4ThreeVector(-siPMOffset, -siPMOffset, 0)
	      };
	      for (int s = 0; s < 4; ++s) {
                new G4PVPlacement(nullptr, sipmBasePos + sipmShifts[s], logicSiPM, "SiPM", logicWorld, false, 4 * (i + j * NcryY + k * NcryX * NcryY) + s);
	      }	      
            }
        }
        G4ThreeVector electronicShift(honeywrapcrystalSizeXY * (NcryX/2.-0.5), honeywrapcrystalSizeXY * (NcryY/2.-0.5), k * (crystalLength + electronicsThickness + siPMThickness)); //aggiunto da me
	new G4PVPlacement(nullptr, electronicsPos + electronicShift, logicElectronics, "Electronics", logicWorld, false, k);
        G4ThreeVector aluminumShift(honeywrapcrystalSizeXY * (NcryX/2.-0.5), honeywrapcrystalSizeXY * (NcryY/2.-0.5),  k * (crystalLength + electronicsThickness + siPMThickness)); //aggiunto da me
	new G4PVPlacement(nullptr, aluminumPos + aluminumShift, logicAluminum, "AluminumShell", logicWorld, false, k);
        G4ThreeVector kaptonStripShift(honeywrapcrystalSizeXY * (NcryX/2.-0.5), honeywrapcrystalSizeXY * (NcryY/2.-0.5),  k * (crystalLength + electronicsThickness + siPMThickness)); //aggiunto da me
	new G4PVPlacement(nullptr, kaptonStripPos+kaptonStripShift, logicKaptonStrip, "KaptonStrip", logicWorld, false, 0);

													
    }

    // Visualization attributes
    //   logicWorld->SetVisAttributes(G4VisAttributes::Invisible);

    G4VisAttributes* invisAttributes = new G4VisAttributes(G4Colour::White()); // Color is optional
    invisAttributes->SetVisibility(false); // Make it invisible
    logicWorld->SetVisAttributes(invisAttributes);

    
    G4VisAttributes* crystalVisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 1.0, 0.1)); // trasparente
    fLogicCrystal->SetVisAttributes(crystalVisAtt);

    G4VisAttributes* wrapVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0)); //  white (paint)
    logicWrap->SetVisAttributes(wrapVisAtt);

    G4VisAttributes* honeycombVisAtt = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5));
    logicHoneycomb->SetVisAttributes(honeycombVisAtt);

    G4VisAttributes* electronicsVisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
    logicElectronics->SetVisAttributes(electronicsVisAtt);

    G4VisAttributes* siPMVisAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
    logicSiPM->SetVisAttributes(siPMVisAtt);

    G4VisAttributes* aluminumVisAtt = new G4VisAttributes(G4Colour(0.7, 0.7, 0.7)); // Gray aluminum
    logicAluminum->SetVisAttributes(aluminumVisAtt);

    G4VisAttributes* kaptonStripVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 0.0)); // Giallo
    logicKaptonStrip->SetVisAttributes(kaptonStripVisAtt);

    return physWorld;
}

void DetectorConstruction::ConstructSDandField() {
    // Creazione del rivelatore sensibile
    CrystalSD* crystalSD = new CrystalSD("CrystalSD");

    // Registrazione presso il SDManager
    G4SDManager::GetSDMpointer()->AddNewDetector(crystalSD);

    // Associazione del rivelatore sensibile ai volumi logici dei cristalli
    if (fLogicCrystal) {
        fLogicCrystal->SetSensitiveDetector(crystalSD);
    }
}

