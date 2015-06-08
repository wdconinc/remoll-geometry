#include <iostream>
#include <cmath>
#include <TTree.h>
#include <TFile.h>
#include <TChain.h>
#include <TCanvas.h>
#include <THStack.h>


class DetRing {
		int i;
                float dTheta;
   	public: int numDet;
                float radRing, zRing, detHeight, detWidth, detThickness;
		float* pos;
                float* dim;
                float* rot;
                DetRing(void);
		void SetParameters(int, float, float, float, float, float); 
                ~DetRing(void);
};
