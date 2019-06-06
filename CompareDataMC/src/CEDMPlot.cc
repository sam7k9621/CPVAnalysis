#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
#include "TGraphErrors.h"
using namespace std;

extern int 
GetIndex( const int& size, const int* pdgid, const vector<int>& id )
{
    for( int i=2; i<size; i++ ){
        for( const auto _i : id ){
            if( pdgid[i] == _i )
                return i;
        }
    }

    return -1;
}

extern TLorentzVector
GetLorentzVector( const Particle& p, const int& idx)
{
    if( idx == -1 ){
        cout<<"warning"<<endl;
    }
    return TLorentzVector( p.px[idx], p.py[idx], p.pz[idx], p.energy[idx] );
}

extern bool
PtSelect( const TLorentzVector& tl, const double& pt )
{
    return fabs( tl.Pt() ) > pt;
}

extern bool
EtaSelect( const TLorentzVector& tl, const double& eta )
{
    return fabs( tl.Eta() ) < eta;
}

extern double
DeltaR( const TLorentzVector& tl1, const TLorentzVector& tl2 )
{
    return tl1.DeltaR( tl2 );
}

extern double
ErrorEstimate( const double& pass, const double& fail )
{
    double error = TMath::Sqrt( pass + fail );
    return error;
}

extern void
CEDMPlot()
{
    Double_t x[100];
    Double_t y_3[100],  y_6[100],  y_12[100],  y_13[100];
    Double_t ey_3[100], ey_6[100], ey_12[100], ey_13[100];

    vector<string> dirlst  = CompMgr().GetOption< vector<string> >("input");
    for( int idx=0; idx<(int)dirlst.size(); idx++ ){
    
        vector<string> filelst = CompMgr().GetSubListData<string>( dirlst[idx], "path" );
        TChain* ch = new TChain( "MC" );

        for( const auto& f : filelst ){
            ch->Add( f.c_str() );
        }

        Int_t psize;
        Int_t pdgid[ 256 ];
        Int_t mother[ 256 ];
        Float_t px[ 256 ];
        Float_t py[ 256 ];
        Float_t pz[ 256 ];
        Float_t energy[ 256 ];
        ch->SetBranchAddress( "psize", &psize );
        ch->SetBranchAddress( "pdgid", pdgid );
        ch->SetBranchAddress( "mother", mother );
        ch->SetBranchAddress( "px", px );
        ch->SetBranchAddress( "py", py );
        ch->SetBranchAddress( "pz", pz );
        ch->SetBranchAddress( "energy", energy );

        double NO3p = 0.;
        double NO6p = 0.;
        double NO12p = 0.;
        double NO13p = 0.;

        double NO3n = 0.;
        double NO6n = 0.;
        double NO12n = 0.;
        double NO13n = 0.;

        Particle part = { px, py, pz, energy};
        // Looping events
        int events   = CompMgr().CheckOption( "test" ) ? 1000 : ch->GetEntries();
        for( int i = 0; i< events; i++ ){
            ch->GetEntry(i);
            CompMgr().process( events, i );

            int bidx    = GetIndex( psize, pdgid, { 5 } );
            int bbaridx = GetIndex( psize, pdgid, { -5 } );
            int lepidx  = GetIndex( psize, pdgid, { 13, -13 } );
            int neuidx  = GetIndex( psize, pdgid, { 14, -14, 12, -12 } );
            int jet1idx = GetIndex( psize, pdgid, { 1, -1, 3, -3 } );
            int jet2idx = GetIndex( psize, pdgid, { 2, -2, 4, -4 } );
            float charge = pdgid[lepidx] > 0 ? -1. : 1;
           
            //only take muon
            if( lepidx == -1 ){
                continue;
            }

            TLorentzVector b        = GetLorentzVector( part, bidx );
            TLorentzVector bbar     = GetLorentzVector( part, bbaridx );   
            TLorentzVector lepton   = GetLorentzVector( part, lepidx );
            TLorentzVector neutrino = GetLorentzVector( part, neuidx );   
            TLorentzVector jet1     = GetLorentzVector( part, jet1idx );  
            TLorentzVector jet2     = GetLorentzVector( part, jet2idx );  
            TLorentzVector hardjet  = jet1.Pt() > jet2.Pt() ? jet1 : jet2 ;
       
            if( 
                !PtSelect( lepton, 20 ) ||
                !PtSelect( hardjet, 20 ) || 
                !PtSelect( neutrino, 30 ) ||
                !PtSelect( b, 25 ) || 
                !PtSelect( bbar, 25 )  
            ){
                continue;   
            }
        
            if(
                !EtaSelect( b, 2.5 ) ||
                !EtaSelect( bbar, 2.5 ) ||
                !EtaSelect( lepton, 2.5 ) ||
                !EtaSelect( jet1, 2.5 ) ||
                !EtaSelect( jet2, 2.5 ) 
            ){
                continue;
            }
           
            bool passDeltaR = true;
            vector<TLorentzVector> infolst = { lepton, b, bbar, jet1, jet2 };
            for( int m=0; m<(int)infolst.size(); m++ ){
                for( int n=(m+1); n<(int)infolst.size(); n++ ){
                    if( DeltaR( infolst[m], infolst[n] ) <= 0.4 ){
                        passDeltaR = false;
                    }
                }
            }

            if( !passDeltaR ){
                continue;
            }

            // In Lab frame
            double o13 = Obs13( lepton.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect(), charge );
            double o6  = Obs6 ( lepton.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect(), charge );
            double o12 = Obs12( b.Vect(), bbar.Vect() );

            // In bbar CM frame
            TVector3 bbCM = -( b + bbar ).BoostVector();
            b.Boost( bbCM );
            bbar.Boost( bbCM );
            lepton.Boost( bbCM );
            hardjet.Boost( bbCM );
        
            double o3 = Obs3( lepton.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect(), charge );
           
            o3 > 0 ?  NO3p++  : NO3n++ ;
            o6 > 0 ?  NO6p++  : NO6n++ ;
            o12 > 0 ? NO12p++ : NO12n++ ;
            o13 > 0 ? NO13p++ : NO13n++ ;

            if( ( NO3p + NO3n ) == 1000000 )
                break;
        }

        double x_dtg = CompMgr().GetSubSingleData<double>( dirlst[idx], "xsection" );
       
        x[idx]    = idx;
        
        y_3[idx]  = (x_dtg / 19.86648916) / (NO3p  + NO3n)  * (NO3p - NO3n); 
        y_6[idx]  = (x_dtg / 19.86648916) / (NO6p  + NO6n)  * (NO6p - NO6n); 
        y_12[idx] = (x_dtg / 19.86648916) / (NO12p + NO12n) * (NO12p - NO12n); 
        y_13[idx] = (x_dtg / 19.86648916) / (NO13p + NO13n) * (NO13p - NO13n); 
        
        ey_3[idx]  = (x_dtg / 19.86648916) / (NO3p  + NO3n)  * ErrorEstimate( NO3p, NO3n );
        ey_6[idx]  = (x_dtg / 19.86648916) / (NO6p  + NO6n)  * ErrorEstimate( NO6p, NO6n );
        ey_12[idx] = (x_dtg / 19.86648916) / (NO12p + NO12n) * ErrorEstimate( NO12p, NO12n );
        ey_13[idx] = (x_dtg / 19.86648916) / (NO13p + NO13n) * ErrorEstimate( NO13p, NO13n );

        delete ch;
   }

    for( int i = 0; i<(int)dirlst.size(); i++ ){
        x[i] /= 2.0;
    }

    TGraphErrors* gr_3  = new TGraphErrors( dirlst.size(), x, y_3,  0, ey_3 );
    TGraphErrors* gr_6  = new TGraphErrors( dirlst.size(), x, y_6,  0, ey_6 );
    TGraphErrors* gr_12 = new TGraphErrors( dirlst.size(), x, y_12, 0, ey_12 );
    TGraphErrors* gr_13 = new TGraphErrors( dirlst.size(), x, y_13, 0, ey_13 );
    
    TCanvas* c = mgr::NewCanvas();
    TF1* f13 = new TF1("f13","0.0*x", 0, 5);
    gr_13->Draw("AEP");
    f13->Draw("same");

    f13->SetLineColor( kGreen - 7 );
    f13->SetLineStyle( 7 );
    gr_13->SetMarkerStyle( 20 );
    gr_13->SetTitle("Obs_{13}");
    gr_13->GetXaxis()->SetTitle("d_{tG}");
    gr_13->GetYaxis()->SetTitle("A_{CP}");
    
    mgr::SetSinglePad( c );
    mgr::SetAxis( gr_13 );
    TGaxis::SetMaxDigits( 5 );
    mgr::SaveToPDF( c, CompMgr().GetResultsName( "pdf", "CEDM_Obs13" ) );
    
    TF1* f3 = new TF1("f3","-0.0148*x", 0, 5);
    gr_3->Draw("AEP");
    f3->Draw("same");

    f3->SetLineColor( kGreen - 7 );
    f3->SetLineStyle( 7 );
    gr_3->SetMarkerStyle( 20 );
    gr_3->SetTitle("Obs_{3}");
    gr_3->GetXaxis()->SetTitle("d_{tG}");
    gr_3->GetYaxis()->SetTitle("A_{CP}");
    
    mgr::SetSinglePad( c );
    mgr::SetAxis( gr_3 );
    TGaxis::SetMaxDigits( 5 );
    mgr::SaveToPDF( c, CompMgr().GetResultsName( "pdf", "CEDM_Obs3" ) );

    TF1* f6 = new TF1("f6", "0.0095*x", 0, 5);
    gr_6->Draw("AEP");
    f6->Draw("same");

    f6->SetLineColor( kGreen - 7 );
    f6->SetLineStyle( 7 );
    gr_6->SetMarkerStyle( 20 );
    gr_6->SetTitle("Obs_{6}");
    gr_6->GetXaxis()->SetTitle("d_{tG}");
    gr_6->GetYaxis()->SetTitle("A_{CP}");
    
    mgr::SetSinglePad( c );
    mgr::SetAxis( gr_6 );
    TGaxis::SetMaxDigits( 5 );

    mgr::SaveToPDF( c, CompMgr().GetResultsName( "pdf", "CEDM_Obs6" ) );

    TF1* f12 = new TF1("f12","0.0018*x", 0, 5);
    gr_12->Draw("AEP");
    f12->Draw("same");

    f12->SetLineColor( kGreen - 7 );
    f12->SetLineStyle( 7 );
    gr_12->SetMarkerStyle( 20 );
    gr_12->SetTitle("Obs_{12}");
    gr_12->GetXaxis()->SetTitle("d_{tG}");
    gr_12->GetYaxis()->SetTitle("A_{CP}");
    
    mgr::SetSinglePad( c );
    mgr::SetAxis( gr_12 );
    TGaxis::SetMaxDigits( 5 );
    mgr::SaveToPDF( c, CompMgr().GetResultsName( "pdf", "CEDM_Obs12" ) );
    
    delete gr_3;
    delete gr_6;
    delete gr_12;
    delete gr_13;
    delete c;

}
