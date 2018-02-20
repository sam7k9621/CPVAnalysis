#include "CPVAnalysis/BaseLineSelector/interface/BaseLineMgr.h"
#include <climits>
#include <iostream>
#include <string>
using namespace std;

/*******************************************************************************
*   Class initialization
*******************************************************************************/
BaseLineMgr::BaseLineMgr( const string& sample, TChain* ch ) :
    HistMgr( sample ),
    Hist2DMgr( sample ),
    _sample( new mgr::SampleMgr( ch ) ),
    _ch(ch)
{
}

BaseLineMgr::~BaseLineMgr()
{
    delete _ch;
    delete _calib;
}

/*******************************************************************************
*   Common
*******************************************************************************/
TLorentzVector
BaseLineMgr::GetLepP4(const int& idx)
{
    return _sample->GetLorentzVector( "lep", idx);
}

TLorentzVector
BaseLineMgr::GetJetP4(const int& idx)
{
    return _sample->GetLorentzVector( "jet", idx);
}
    
tuple<double, double, int>
BaseLineMgr::GetChi2Info( const vector<int>& jetidx, const vector<int>& bjetidx )
{
    vector<TLorentzVector> jethandle;

    for( const auto& j : jetidx ){
        jethandle.push_back( GetJetP4(j) );
    }

    vector<TLorentzVector> bjethandle;

    for( const auto& b : bjetidx ){
        bjethandle.push_back( GetJetP4(b) );
    }

    // Mass constrain method - find hadronic b
    double chi2mass = INT_MAX;
    double seltmass = 0;
    int had_b       = -1;

    for( unsigned int i = 0; i < jethandle.size(); i++ ){
        for( unsigned int j = ( i + 1 ); j < jethandle.size(); j++ ){
            for( unsigned int k = 0; k < bjethandle.size(); k++ ){
                double t_mass = ( jethandle[ i ] + jethandle[ j ] + bjethandle[ k ] ).M();
                double w_mass = ( jethandle[ i ] + jethandle[ j ] ).M();
                double chi_t  = ( t_mass - 172.5 ) / 16.3;
                double chi_w  = ( w_mass - 82.9 ) / 9.5;

                if( ( chi_t * chi_t + chi_w * chi_w ) < chi2mass ){
                    chi2mass = ( chi_t * chi_t + chi_w * chi_w );
                    seltmass = t_mass;
                    had_b    = k;
                }
            }
        }
    }

    return make_tuple( chi2mass, seltmass, had_b );
}

double
BaseLineMgr::GetLeptonicM(const int& lidx, const int& bidx){
    TLorentzVector lep  = GetLepP4(lidx);
    TLorentzVector bjet = GetJetP4(bidx);

    return (lep + bjet ).M();
}

float 
BaseLineMgr::GetIsoLepCharge(const int& idx)
{
    return _sample->GetLepCharge(idx);
}

BaseLineMgr::MatchType
BaseLineMgr::bbSeparation( const int& had_b, const int& lep_b, const int& lepidx )
{
    //hadronic b charge equals to muon, and vice versa
    float charge = _sample->GetLepCharge( lepidx );
    float had_id = _sample->GetPartonID( had_b );
    float lep_id = _sample->GetPartonID( lep_b );

    if( had_id == 0 || lep_id == 0)
        return Nomatched;

    if( fabs(had_id) != 5 || fabs(lep_id) != 5  )
        return Mistag;

    if( (had_id * charge) > 0 || (lep_id * charge) < 0)
        return Misid;
    else
        return Correct;
}


/*******************************************************************************
*   Vertex & HLT selection
*******************************************************************************/
bool
BaseLineMgr::passHLT( const vector<int>& hlt )
{
    return _sample->passHLT( hlt );
}

bool
BaseLineMgr::passVertex()
{
    for( int i = 0; i < _sample->Vsize(); i++ ){
        _sample->SetIndex( i );

        if( isGoodPVtx() ){
            return true;
        }
    }

    return false;
}

bool
BaseLineMgr::isGoodPVtx()
{
    return !( _sample->IsFake() ) &&
           _sample->passNdof() &&
           _sample->passAbsZ() &&
           _sample->passRho()
    ;
}

/*******************************************************************************
*   Jet selection
*******************************************************************************/
void 
BaseLineMgr::jetSmeared()
{
    int js = _sample->Jsize();
    
    for(int i=0; i<js; i++){
        _sample->SetIndex(i);
        
        double scale = _sample->MakeSmeared();
        TLorentzVector jetp4 = _sample->JetP4(); 
        jetp4 *= scale;
        _sample->Jet().Pt[i]  = jetp4.Pt();
        _sample->Jet().Eta[i] = jetp4.Eta();
    }
}
    
bool
BaseLineMgr::passJet()
{
    return
        // Kinematic cut
        _sample->passJetPt( 30 ) &&
        _sample->passJetEta( 2.4 ) &&
        // Loose ID
        _sample->JetNConstituents() &&
        _sample->JetNEF() &&
        _sample->JetNHF() &&
        _sample->JetCHF() &&
        _sample->JetNCH() &&
        _sample->JetCEF()
    ;
}

bool
BaseLineMgr::passBJet()
{
    return _sample->JetCSVM( 0.8484 )
    ;
}

bool
BaseLineMgr::passFullJet( vector<int>& jetidx, vector<int>& bjetidx, int& muidx )
{
    for( int j = 0; j < _sample->Jsize(); j++ ){
        _sample->SetIndex( j );

        // Cleaning against leptons (isolated lepton)
        if( !_sample->isIsoLepton( muidx, j ) ){
            continue;
        }

        int mask = 0x01;

        if( passJet() ){
            mask <<= 1;
        }

        if( passBJet() ){
            mask <<= 2;
        }

        if( mask & 0x02 ){
            jetidx.push_back( j );
        }
        else if( mask & 0x08 ){
            bjetidx.push_back( j );
        }
    }

    return jetidx.size() >= 2 && bjetidx.size() == 2;
}

/*******************************************************************************
*   Muon selection
*******************************************************************************/
bool
BaseLineMgr::passMuLoose()
{
    return
        // Kinematic cut
        _sample->passMuPt( 15 ) &&
        _sample->passMuEta( 2.4 ) &&
        // Isolation cut
        _sample->passMuRelIsoR04( 0.25 ) &&
        // Loose ID
        ( _sample->isGlobalMuon() || _sample->isTrackerMuon() )
    ;
}

bool
BaseLineMgr::passMuTight()
{
    return
        // Kinematic cut
        _sample->passMuPt( 30 ) &&
        _sample->passMuEta( 2.1 ) &&
        // Isolation cut
        _sample->passMuRelIsoR04( 0.15 ) &&
        // Tight ID
        _sample->MuInnerTrackDxy_PV() &&
        _sample->MuInnerTrackDz() &&
        _sample->MuNMuonhits() &&
        _sample->MuNMatchedStations() &&
        _sample->MuGlobalNormalizedChi2() &&
        _sample->MuNTrackLayersWMeasurement() &&
        _sample->MuNPixelLayers() &&
        _sample->isGlobalMuon()
    ;
}

bool
BaseLineMgr::passFullMuon( vector<int>& muidx )
{
    for( int i = 0; i < _sample->Lsize(); i++ ){
        _sample->SetIndex( i );

        if( _sample->Lep_Type() == 13 ){
            if( passMuTight() ){
                muidx.push_back( i );
                continue;
            }

            if( passMuLoose() ){
                return false;
            }
        }
        else if( _sample->Lep_Type() == 11 ){
            if( passElLoose() ){
                return false;
            }
        }
    }

    return muidx.size() > 0;
}

/*******************************************************************************
*   Electron selection
*******************************************************************************/
bool
BaseLineMgr::passElLoose()
{
    return
        // Kinematic
        _sample->passElPt( 15 ) &&
        _sample->passElEta( 2.4 ) &&
        // Loose ID
        _sample->passElIDLoose()
    ;
}

/*******************************************************************************
*   Pre-selection
*******************************************************************************/
bool
BaseLineMgr::preJet()
{
    return _sample->Jsize() >= 4;
}

bool
BaseLineMgr::preMuon()
{
    for( int i = 0; i < _sample->Lsize(); i++ ){
        _sample->SetIndex( i );

        if( passMuTight() ){
            return true;
        }
    }

    return false;
}

/*******************************************************************************
*   Event looping
*******************************************************************************/
void 
BaseLineMgr::GetEntry(const int& i)
{
    _ch->GetEntry(i);
}

TTree* 
BaseLineMgr::CloneTree()
{
    _ch->GetEntry(0);
    return (TTree*)_ch->GetTree()->CloneTree( 0 ); 
}
