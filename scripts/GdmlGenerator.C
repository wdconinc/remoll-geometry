#include <iostream>
#include <cmath>
#include <TTree.h>
#include <TFile.h>
#include <TChain.h>
#include <TCanvas.h>
#include <THStack.h>
#include <GdmlGenerator.h>
#define PI 4*atan(1.0)
/*------------Run it in batchmode in root by typing root -q -b GdmlGenerator.C------------*/



int GdmlGenerator()
{
int i,j,num=0; //looping variable

char preamble[500]="<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n\n<!DOCTYPE gdml [ \n\t<!ENTITY definitions SYSTEM \"definitionsDet.xml\"> \n\t<!ENTITY optical_definitions SYSTEM \"definitions_global.xml\"> \n\t<!ENTITY materials SYSTEM \"materials.xml\"> \n\t<!ENTITY solids SYSTEM \"solidsDet.xml\">\n]> \n\n<gdml xmlns:gdml=\"http://cern.ch/2001/Schemas/GDML\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"schema/gdml.xsd\"> \n\n&definitions;\n&materials; \n&solids;";

char geometryFolder[30]="geometry_new";



const int numRing = 7;  // Number of detector rings
int numDetPerRing[numRing] = {28, 28, 28, 28, 84, 28, 84};     // Number of detectors per ring
int physDetIndex[numRing]={500,600,700,800,900,1000,1100}  ;   // starting index for detectors in detector region as auxilliary information.           
float detThickness[numRing] = {15,15,15,15,15,15,15};            // Detector thickness along z-direction 
float detHeight[numRing] = {50, 100,60,40,120,100,120};          //  Detector height along radial direction
float radRing[numRing] =  {656,731, 811, 860, 940, 1050, 940};  // Radii of detector rings
float detWidthOff[numRing] = { 0, 0, 0, 0, 0, 0, 0};
float zRing[numRing]= {0, 400, 800, 1200, 1400, 1600, 1800};   // Z-Position of rings in the local coordinate system of detector module 
float detWidth[numRing];                // detector widths    
DetRing detRing[numRing];

float lightGuideAngle[numRing] ={45, 45, 45, 45, 45, 45, 45};  // angle between lightguides and quartz detector ring.
float lightGuideLength[numRing] = {48.5,38.5,32.5,28.5,16.5,6.5,16.5}; // length of lightguides
char logicDet[30]="logicDet"; 		// name of logical volume holding sensitive detectors.
char logicDetMat[30]= "Vacuum"; 	// Material of the logical volume holding the sensitive detectors.
char physDet[30]= "physDet"; 		// prefix for name of physical volumes for sensitive detectors. 
char physLight[30] = "physLight";
char physDetMat[30]= "Quartz"; 		// Material of the physical volumes of the sensitive detectors.
char physLightMat[30] = "Vacuum";
char physDetType[30]="planeDet"; 	// type of sensitive detector passed as auxilliary information.

 










/*------------------------------ Definitions ------------------------------------------------*/
ofstream definitions;  
definitions.open("definitionsDet.xml");

definitions << "<define>" << endl;

num=numRing;
FormSingMatrix(num, lightGuideAngle, definitions, physLight,  1);          
FormSingMatrix(num, lightGuideLength, definitions, physLight, 0);


// Position, rotation and dimension information for detector rings inside detector logic volume.
for(j=0;j<numRing;j++){
detRing[j].SetParameters(numDetPerRing[j], radRing[j], zRing[j], detHeight[j], detWidthOff[j], detThickness[j]);
FormPosMatrix(*(numDetPerRing+j), (detRing+j)->pos, definitions, physDet,j,0);
FormPosMatrix(*(numDetPerRing+j), (detRing+j)->dim, definitions, physDet,j,1);
FormPosMatrix(*(numDetPerRing+j), (detRing+j)->rot, definitions, physDet,j,2);
}



definitions<< "</define>" << endl;
/*--------------------------------------------------------------------------------------------*/


/*------------------------------- Solids ----------------------------------------------------*/
ofstream solids;  
solids.open("solidsDet.xml");

solids << "<solids>" << endl;

solids << "\t<box lunit=\"mm\" name=\"logicDetSol\" x=\"4000\" y=\"4000\" z=\"4000\"/>"<< endl;



for(j=1;j<=numRing;j++){
for(i=1; i<=numDetPerRing[j-1]; i++){
solids << Form("\t<box name=\"%sSol_%i_%i\" lunit=\"mm\" x=\"%sDim_%i[%i,1]\" y=\"%sDim_%i[%i,2]\" z=\"%sDim_%i[%i,3]\"/> ", physDet, j, i, physDet, j, i, physDet, j, i, physDet, j, i) << std::endl;
}}


solids << "</solids>" << endl;

/*---------------------------------------------------------------------------------------------*/


/*-----------------------Detector Daughter Structure-------------------------------------------*/
ofstream detectorDaughterStructure;    
detectorDaughterStructure.open("detectorDaughter.gdml");
detectorDaughterStructure<< preamble <<endl;
detectorDaughterStructure<< "\n<structure>\n" << endl;


for(j=1;j<=numRing;j++){
for(i=1; i<=numDetPerRing[j-1]; i++){
detectorDaughterStructure << Form("<volume name=\"%sVol_%i_%i\"> \n\t <materialref ref=\"%s\"/> \n\t<solidref ref=\"%sSol_%i_%i\"/> \n\t<auxiliary auxtype=\"Color\" auxvalue=\"green\"/> \n\t<auxiliary auxtype=\"SensDet\" auxvalue=\"%s\"/> \n\t<auxiliary auxtype=\"DetNo\" auxvalue=\"%i\"/>  \n</volume>", physDet, j, i, physDetMat, physDet, j, i, physDetType, (physDetIndex[j-1]+i)) << std::endl;

/*detectorDaughterStructure << Form("<volume name=\"%sVol_%i_%i\"> \n\t <materialref ref=\"%s\"/> \n\t<solidref ref=\"%sSol_%i_%i\"/> \n\t<auxiliary auxtype=\"Color\" auxvalue=\"green\"/> \n\t<auxiliary auxtype=\"SensDet\" auxvalue=\"%s\"/> \n\t<auxiliary auxtype=\"DetNo\" auxvalue=\"%i\"/>  \n</volume>", physLight, j, i, physDetMat, physDet, j, i, physDetType, (physDetIndex[j-1]+i)) << std::endl;*/
}}

detectorDaughterStructure << Form("\n<volume name=\"%sVol\"> \n\t<materialref ref=\"%s\"/>\n\t<solidref ref=\"%sSol\"/>\n", logicDet, logicDetMat, logicDet) << std::endl;

for(j=1;j<=numRing;j++){
for(i=1; i<=numDetPerRing[j-1]; i++){
detectorDaughterStructure << Form("\t<physvol name=\"%s_%i_%i\">\n\t\t<volumeref ref=\"%sVol_%i_%i\"/>\n\t\t<position name=\"%sPos_%i_%i\" unit=\"mm\" x=\"%sPos_%i[%i,1]*cos(%sPos_%i[%i,2]*deg)\" y=\"%sPos_%i[%i,1]*sin(%sPos_%i[%i,2]*deg)\" z=\"%sPos_%i[%i,3]\"/>\n\t\t<rotation name=\"%sRot_%i_%i\" unit=\"deg\" x=\"%sRot_%i[%i,1]\" y=\"%sRot_%i[%i,2]\" z=\"%sRot_%i[%i,3]\"/> \n\t</physvol>", physDet, j, i, physDet, j, i, physDet, j, i, physDet, j, i, physDet, j, i, physDet, j, i, physDet, j, i, physDet, j, i, physDet, j, i, physDet, j, i, physDet, j, i, physDet, j, i) << std::endl;
}}
detectorDaughterStructure << "</volume>\n" << std::endl;
detectorDaughterStructure << "</structure>\n" << std::endl;
detectorDaughterStructure << Form("<setup name=\"%s\" version=\"1.0\">\n\t<world ref=\"%sVol\"/>\n</setup>", logicDet, logicDet) <<endl;
detectorDaughterStructure<< "\n</gdml>" << std::endl;
detectorDaughterStructure.close();
/*-------------------------------------------------------------------------------------------------*/


return 0;
}


void FormSingMatrix(int num, float pos[], std::ofstream& file, char name[], int k) // Writes a vector defining lengths and tilt angles of light guides for individual rings
{
char type[3][30]={"Length","Angle"};
file<< Form("<matrix name=\"%s%s\" coldim=\"1\" values=\"", name, type[k]);

for(int i=0; i<num-1; i++)
{
file << Form("%3.6f\n\t\t\t\t\t      ", pos[i]);
}  
file << Form("%3.6f\"/>", pos[(num-1)])<<endl;

}


void FormPosMatrix(int num, float pos[], std::ofstream& file, char name[], int j, int k) // Writes a position matrix in the definition gdml file
{
char type[3][30]={"Pos", "Dim", "Rot"};
file<< Form("<matrix name=\"%s%s_%i\" coldim=\"3\" values=\"", name, type[k], j+1);

for(int i=0; i<num-1; i++)
{
file << Form("%3.6f %3.6f %3.6f \n\t\t\t\t\t      ", pos[3*i], pos[3*i+1], pos[3*i+2]);
}  
file << Form("%3.6f %3.6f %3.6f\"/>", pos[3*(num-1)], pos[3*(num-1)+1], pos[3*(num-1)+2])<<endl;

}



DetRing::DetRing(void){
std::cout << "Detector Ring is being created." << std::endl;
}



void DetRing::SetParameters(int a, float b, float c, float d, float e, float f){
i=0;
numDet = a;
pos = new float[3*numDet];
dim =new float[3*numDet];
rot =new float[3*numDet];
radRing = b;
zRing = c;
detHeight = d;
detWidth = PI*((radRing+detHeight/2)*(radRing+detHeight/2)-(radRing-detHeight/2)*(radRing-detHeight/2))/(numDet*detHeight);
detWidth+= e*detWidth;
detThickness = f;
dTheta = 360.0/numDet;
for(i=0;i<numDet;i++)
{ pos[3*i]= radRing;
  pos[3*i+1]= i*dTheta;
  pos[3*i+2]= (i & 1) ? (zRing-detThickness):zRing;
  dim[3*i] = detHeight;
  dim[3*i+1] = detWidth;
  dim[3*i+2] = detThickness;
  rot[3*i] =   0          ;
  rot[3*i+1] =  0         ;
  rot[3*i+2] =  -i*dTheta ;
}
std::cout<< "Setting parameters for Detector Ring." << std::endl;
}

DetRing::~DetRing(void){
std::cout << "Object is being destroyed" << std::endl;
}
