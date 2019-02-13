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
    Double_t y_2[100],  y_3[100],  y_4[100],  y_7[100];
    Double_t ey_2[100], ey_3[100], ey_4[100], ey_7[100];

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

        double NO2p = 0.;
        double NO3p = 0.;
        double NO4p = 0.;
        double NO7p = 0.;

        double NO2n = 0.;
        double NO3n = 0.;
        double NO4n = 0.;
        double NO7n = 0.;

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
            double o2 = Obs2( lepton.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect() );
            double o4 = Obs4( lepton.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect(), charge );
            double o7 = Obs7( b.Vect(), bbar.Vect() );

            // In bbar CM frame
            TVector3 bbCM = -( b + bbar ).BoostVector();
            b.Boost( bbCM );
            bbar.Boost( bbCM );
            lepton.Boost( bbCM );
            hardjet.Boost( bbCM );
        
            double o3 = Obs3( lepton.Vect(), hardjet.Vect(), b.Vect(), bbar.Vect(), charge );
           
            o2 > 0 ? NO2p++ : NO2n++ ;
            o3 > 0 ? NO3p++ : NO3n++ ;
            o4 > 0 ? NO4p++ : NO4n++ ;
            o7 > 0 ? NO7p++ : NO7n++ ;

            if( ( NO3p + NO3n ) == 1000000 )
                break;
        }

        double x_dtg = CompMgr().GetSubSingleData<double>( dirlst[idx], "xsection" );
       
        x[idx]    = idx;
        
        y_2[idx]  = (x_dtg / 19.86648916) / (NO2p + NO2n) * (NO2p - NO2n); 
        y_3[idx]  = (x_dtg / 19.86648916) / (NO3p + NO3n) * (NO3p - NO3n); 
        y_4[idx]  = (x_dtg / 19.86648916) / (NO4p + NO4n) * (NO4p - NO4n); 
        y_7[idx]  = (x_dtg / 19.86648916) / (NO7p + NO7n) * (NO7p - NO7n); 
        
        ey_2[idx] = (x_dtg / 19.86648916) / (NO2p + NO2n) * ErrorEstimate( NO2p, NO2n );
        ey_3[idx] = (x_dtg / 19.86648916) / (NO3p + NO3n) * ErrorEstimate( NO3p, NO3n );
        ey_4[idx] = (x_dtg / 19.86648916) / (NO4p + NO4n) * ErrorEstimate( NO4p, NO4n );
        ey_7[idx] = (x_dtg / 19.86648916) / (NO7p + NO7n) * ErrorEstimate( NO7p, NO7n );

        delete ch;
   }

    for( int i = 0; i<(int)dirlst.size(); i++ ){
        x[i] /= 2.0;
    }

    TGraphErrors* gr_2 = new TGraphErrors( dirlst.size(), x, y_2, 0, ey_2 );
    TGraphErrors* gr_3 = new TGraphErrors( dirlst.size(), x, y_3, 0, ey_3 );
    TGraphErrors* gr_4 = new TGraphErrors( dirlst.size(), x, y_4, 0, ey_4 );
    TGraphErrors* gr_7 = new TGraphErrors( dirlst.size(), x, y_7, 0, ey_7 );
    
    TCanvas* c = mgr::NewCanvas();
    TF1* f2 = new TF1("f2","0.0*x", 0, 5);
    gr_2->Draw("AEP");
    f2->Draw("same");

    f2->SetLineColor( kGreen - 7 );
    f2->SetLineStyle( 7 );
    gr_2->SetMarkerStyle( 20 );
    gr_2->SetTitle("Obs_{13}");
    gr_2->GetXaxis()->SetTitle("d_{tG}");
    gr_2->GetYaxis()->SetTitle("A_{CP}");
    
    mgr::SetSinglePad( c );
    mgr::SetAxis( gr_2 );
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

    TF1* f4 = new TF1("f4", "0.0095*x", 0, 5);
    gr_4->Draw("AEP");
    f4->Draw("same");

    f4->SetLineColor( kGreen - 7 );
    f4->SetLineStyle( 7 );
    gr_4->SetMarkerStyle( 20 );
    gr_4->SetTitle("Obs_{6}");
    gr_4->GetXaxis()->SetTitle("d_{tG}");
    gr_4->GetYaxis()->SetTitle("A_{CP}");
    
    mgr::SetSinglePad( c );
    mgr::SetAxis( gr_4 );
    TGaxis::SetMaxDigits( 5 );

    mgr::SaveToPDF( c, CompMgr().GetResultsName( "pdf", "CEDM_Obs6" ) );

    TF1* f7 = new TF1("f7","0.0018*x", 0, 5);
    gr_7->Draw("AEP");
    f7->Draw("same");

    f7->SetLineColor( kGreen - 7 );
    f7->SetLineStyle( 7 );
    gr_7->SetMarkerStyle( 20 );
    gr_7->SetTitle("Obs_{12}");
    gr_7->GetXaxis()->SetTitle("d_{tG}");
    gr_7->GetYaxis()->SetTitle("A_{CP}");
    
    mgr::SetSinglePad( c );
    mgr::SetAxis( gr_7 );
    TGaxis::SetMaxDigits( 5 );
    mgr::SaveToPDF( c, CompMgr().GetResultsName( "pdf", "CEDM_Obs12" ) );
    
    delete gr_2;
    delete gr_3;
    delete gr_4;
    delete gr_7;
    delete c;

}
