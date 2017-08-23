#include "TFile.h"
#include "TTree.h"
#include "TNtuple.h"
#include "TCanvas.h"
#include "TMath.h"
#include "RooRealVar.h"
#include "RooGaussian.h"
#include "RooAddPdf.h"
#include "RooDataSet.h"
#include "RooChebychev.h"
#include "RooPlot.h"

using namespace RooFit;

void B02JpsiKsfit()
{
    TFile *fin = new TFile( "~/pyexercise/B02JpsiKs_output.root" );
    
    TTree *h1;
    fin->GetObject("h1",h1);
    //Open B02JpsiKs TTree
    float B0_mbc;
    float B0_deltae;
    
    h1->SetBranchStatus("*",0);
    h1->SetBranchStatus("B0_mbc",1);
    h1->SetBranchStatus("B0_deltae",1);
    h1->SetBranchAddress("B0_mbc",&B0_mbc);
    h1->SetBranchAddress("B0_deltae",&B0_deltae);
    //Set branch address
    
    TNtuple* n1 = new TNtuple ("n1","B0","mass:deltae");
    float var[2];
    for (int idx=0;idx<(int)h1->GetEntries();idx++){
        h1->GetEntry(idx);
        var[0]=B0_mbc;
        var[1]=B0_deltae;
        n1->Fill(var);
    }

    //Get entries from TTree
    /*BEvent* Event = new BEvent();
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
    }*/
    //old code for n_jpsi
    RooRealVar mass("mass","mass",5.2,5.29);
    
    RooRealVar nbkg("nbkg","nbkg", 1000.);
    RooRealVar area("area","area", 50000.);
    RooRealVar slope("slope","slope",0., -10.,  10.);
    RooRealVar mean("mean","mean",  5.28, 5.2, 5.29 );
    RooRealVar width("width","width", 0.02, 0.0001, 0.05);
    
    RooGaussian gaussian("gaussian","gaussian",mass,mean,width);
    RooChebychev linear("linear","linear",mass,RooArgList(slope));
    
    RooAddPdf model("model","model",RooArgList(gaussian,linear),RooArgList(area,nbkg));
    
    RooDataSet *data = new RooDataSet("data","data",n1,RooArgSet(mass));
    
    model.fitTo(*data,Extended(kTRUE),Minos(kTRUE));
    
    TCanvas* c1 = new TCanvas("c1","c1",600,400);
    
    RooPlot* frame = mass.frame();
    data->plotOn(frame,Binning(1000));
    model.plotOn(frame,LineWidth(3));
    frame->Draw();
}

