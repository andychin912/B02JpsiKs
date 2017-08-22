#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TCanvas.h"
#include "TMath.h"
#include "BParticle.h"
#include "BEvent.h"
#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooAddPdf.h"
#include "RooDataSet.h"
#include "RooChebychev.h"
#include "RooPlot.h"

using namespace RooFit;

void example_04()
{
    TFile *fin = new TFile( "jpsi.root" );
    
    TTree *T;
    fin->GetObject("T",T);
    
    BEvent* Event = new BEvent();
    T->SetBranchAddress("BEvent",&Event);
    
    TNtuple* n_jpsi = new TNtuple ( "n_jpsi", "Invariant Mass of #mu^{+}#mu^{-}", "mass");
    
    int NEvents = (int)T->GetEntries();
    for (int idx=0;idx<NEvents;idx++) {
        
        if ((idx % 1000) == 0) printf ( "At Event: %d/%d.\n",idx,NEvents);
        T->GetEntry(idx);
        
        int NParticles = Event->NParticles();
        TClonesArray& Plist = *(Event->GetParticleList());
        
        for (int i=0;i<NParticles;i++) {
            for (int j=0;j<NParticles;j++) {
                
                BParticle* P1 = (BParticle*)Plist[i];
                BParticle* P2 = (BParticle*)Plist[j];
                
                if (P1->pid()!=MUON  || P2->pid()!=MUON ) continue;
                if (P1->charge()!=+1 || P2->charge()!=-1) continue;
                
                float Px = P1->px() + P2->px();
                float Py = P1->py() + P2->py();
                float Pz = P1->pz() + P2->pz();
                float E  = P1->e()  + P2->e();
                float P  = sqrt(Px*Px + Py*Py + Pz*Pz);
                float Mass = sqrt(E*E - P*P);
                
                if (Mass>2.7 && Mass<3.4) n_jpsi->Fill(Mass);
            }
        }
    }
    
    RooRealVar mass("mass","mass",2.7,3.4);
    
    RooRealVar nbkg("nbkg","nbkg", 6000., 0.,9000.);
    RooRealVar area("area","area", 1000., 0.,4000.);
    RooRealVar slope("slope","slope",-0.5, -10.,  10.);
    RooRealVar mean("mean","mean",  3.09, 3., 3.2 );
    RooRealVar width("width","width", 0.01, 0.0001, 0.05);
    
    RooGaussian gaussian("gaussian","gaussian",mass,mean,width);
    RooChebychev linear("linear","linear",mass,RooArgList(slope));
    
    RooAddPdf model("model","model",RooArgList(gaussian,linear),RooArgList(area,nbkg));
    
    RooDataSet *data = new RooDataSet("data","data",n_jpsi,RooArgSet(mass));
    
    model.fitTo(*data,Extended(kTRUE),Minos(kTRUE));
    
    TCanvas* c1 = new TCanvas("c1","c1",600,400);
    
    RooPlot* frame = mass.frame();
    data->plotOn(frame,Binning(140));
    model.plotOn(frame,LineWidth(3));
    frame->Draw();
}

