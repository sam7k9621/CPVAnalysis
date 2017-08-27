#include "CPVAnalysis/SelectionInfo/interface/SelectionMgr.h"

#include "TLorentzVector.h"
#include <TMath.h>
#include <iostream>

using namespace std;
using namespace sel;
using namespace mgr;
/*******************************************************************************
*  Class initialization
*******************************************************************************/

SelectionMgr::SelectionMgr( const string& subdir ):
    Pathmgr( "CPVAnalysis", subdir ),
    Readmgr( SettingsDir() / "Selection.json" ),
    Parsermgr() {
    _rvalue = 0;
}

SelectionMgr::~SelectionMgr() {
}

void SelectionMgr::SetIndex( const int& i ) {
    _idx = i;
}

void SelectionMgr::SetRoot( TChain* ch ) {
    vtx.Register( ch );
    lep.Register( ch );
    evt.Register( ch );
    jet.Register( ch );
    gen.Register( ch );
}


/*******************************************************************************
*   Common calculation
*******************************************************************************/
bool SelectionMgr::passHLT( vector<int> hlt ) {
    for( auto h : hlt ) {
        if( int( evt.TrgBook[h] ) == 1 ) {
            return true;
        }
    }

    return false;
}


double SelectionMgr::RelIsoR04() {
    double rel = ( lep.ChargedHadronIsoR04[_idx] + max( float ( 0. ), lep.NeutralHadronIsoR04[_idx] + lep.PhotonIsoR04[_idx] - float ( 0.5 ) * lep.sumPUPtR04[_idx] ) ) / lep.Pt[_idx];
    return rel;
}

int SelectionMgr::vsize() {
    return vtx.Size;
}

int SelectionMgr::lsize() {
    return lep.Size;
}

int SelectionMgr::jsize() {
    return jet.Size;
}

int SelectionMgr::gsize() {
    return gen.Size;
}

int SelectionMgr::lep_type() {
    return lep.LeptonType[_idx];
}

int SelectionMgr::runNO() {
    return evt.RunNo;
}

int SelectionMgr::lumiNO() {
    return evt.LumiNo;
}

TLorentzVector SelectionMgr::getLorentzVector( const string& type, const int& idx ) {
    TLorentzVector tl;

    if( type == "lep" ) {
        tl.SetPxPyPzE(
            lep.Px[idx],
            lep.Py[idx],
            lep.Pz[idx],
            lep.Energy[idx]
        );
    }

    else if( type == "jet" ) {
        tl.SetPxPyPzE(
            jet.Px[idx],
            jet.Py[idx],
            jet.Pz[idx],
            jet.Energy[idx]
        );
    }

    return tl;
}

TLorentzVector SelectionMgr::getLorentzLep( const int& idx ) {
    return getLorentzVector( "lep", idx );
}

vector<TLorentzVector> SelectionMgr::getLorentzJet( const vector<int>& idx ) {
    vector<TLorentzVector> tl;

    for( const auto& i : idx ) {
        tl.push_back( getLorentzVector( "jet", i ) );
    }

    return tl;
}

bool SelectionMgr::isIsoLepton( const int& lepidx, const int& jetidx ) {
    double deta = ( double ) ( lep.Eta[lepidx] - jet.Eta[jetidx] );
    double dphi = Phi_mpi_pi( ( double ) ( lep.Phi[lepidx] - jet.Phi[jetidx] ) );
    double deltaR = TMath::Sqrt( deta * deta + dphi * dphi );
    return deltaR >= 0.4;
}

TLorentzVector SelectionMgr::getMET( const TLorentzVector lep ) {
    double px = lep.Px() + evt.PFMETx;
    double py = lep.Py() + evt.PFMETy;
    double e  = lep.E() + evt.PFMET;
    double pz2 = e * e - ( 82.9 * 82.9 ) - px * px - py * py;
    double pz;

    if( pz2 < 0 ) {
        pz = 1000000;
    }

    else {
        pz  = -1 * ( sqrt( pz2 ) - lep.Pz() );
    }

    TLorentzVector tl;
    tl.SetPxPyPzE(
        evt.PFMETx,
        evt.PFMETy,
        pz,
        evt.PFMET
    );
    return tl;
}

double SelectionMgr::Phi_mpi_pi( double x ) {
    Double_t const  kPI        = TMath::Pi();
    Double_t const  kTWOPI     = 2.*kPI;

    while ( x >=  kPI ) {
        x -= kTWOPI;
    }

    while ( x <  -kPI ) {
        x += kTWOPI;
    }

    return x;
}

double SelectionMgr::getLepPt( const int& idx ) {
    return lep.Pt[idx];
}

double SelectionMgr::getJetPt( const int& idx ) {
    return jet.Pt[idx];
}

/*******************************************************************************
*   MC Truth
*******************************************************************************/
int SelectionMgr::matchBhandle( const int& idx, const int& charge ) {
    // flag info : interface/SelectionInfo.h

    // fake b
    if ( !( fabs( gen.PdgID[idx] ) == 5 ) ) {
        return 1 << 1 ;
    }

    for( const auto& i : bhandle ) {
        if(
            getDirectMother( i ) == getDirectMother( idx )
        ) {
            // correct
            if( gen.PdgID[idx] * charge < 0 ) {
                return 1 << 0;
            }

            // charge swapped
            else {
                return 1 << 3;
            }
        }
    }

    // mistag
    return 1 << 2;
}

int SelectionMgr::getMuonCharge( const int& idx ) {
    return lep.Charge[idx];
}

int SelectionMgr::bbarDeltaR( const int& bidx, const int& charge ) {
    int    idx    = -1;
    double dR     = 999;

    for( int i = 0; i < gsize(); i++ ) {
        double deta   = jet.GenEta[bidx] - gen.Eta[i];
        double dphi   = Phi_mpi_pi( jet.GenPhi[bidx] - gen.Phi[i] );
        double deltaR = TMath::Sqrt( deta * deta + dphi * dphi );

        //Choosing the smallest deltaR #original 0.4
        if( deltaR < 0.4 && deltaR < dR ) {
            dR  = deltaR;
            idx = i;
        }

        /* if( deltaR < 0.1){*/
        //idx = i;
        //return matchBhandle(idx,charge);
        /*}*/
    }

    if( idx < 0 ) {
        return 1 << 5;
    }

    return matchBhandle( idx, charge );
}

void SelectionMgr::RvalueUP( const double& up ) {
    _rvalue += up;
}

void SelectionMgr::getRvalue() {
    cout << "Rvalue " << _rvalue << endl;
}

int SelectionMgr::matchGenlevel( const float& eta, const float& phi ) {
    double deta   = 0;
    double dphi   = 0;
    double deltaR = 0;

    for( int i = 0; i < gsize(); i++ ) {
        deta   = eta - gen.Eta[i];
        dphi   = Phi_mpi_pi( ( double )( phi - gen.Phi[i] ) );
        deltaR = TMath::Sqrt( deta * deta + dphi * dphi );

        //prevent from shower particle
        if( deltaR < _rvalue ) {
            return i;
        }
    }

    return -999;
}

void SelectionMgr::showGenInfo( const int& i ) {
    cout << "pt  " << gen.Pt [i] << endl;
    cout << "eta " << gen.Eta[i] << endl;
    cout << "phi " << gen.Phi[i] << endl;
}

void SelectionMgr::showJetInfo( const int& i ) {
    cout << "pt  " << jet.Pt [i] << endl;
    cout << "eta " << jet.Eta[i] << endl;
    cout << "phi " << jet.Phi[i] << endl;
}

int SelectionMgr::getGenParticle() {
    int pdgid   = lep.GenPdgID[_idx];
    float eta   = lep.GenEta  [_idx];
    float phi   = lep.GenPhi  [_idx];

    if( pdgid == 0 ) {
        return -999;
    }

    return matchGenlevel( eta, phi );
}


int SelectionMgr::getGenParton() {
    float eta   = jet.Eta[_idx];
    float phi   = jet.Phi[_idx];
    return matchGenlevel( eta, phi );
}

int SelectionMgr::getDirectDa1( int idx ) {
    if( gen.Da1[idx] < 0 ) {
        return -999;
    }

    while( gen.Da1PdgID[idx] == gen.PdgID[idx] ) {
        idx = gen.Da1[idx];
    }

    return gen.Da1[idx];
}

int SelectionMgr::getDirectDa2( int idx ) {
    if( gen.Da2[idx] < 0 ) {
        return -999;
    }

    while( gen.Da2PdgID[idx] == gen.PdgID[idx] ) {
        idx = gen.Da2[idx];
    }

    return gen.Da2[idx];
}

int SelectionMgr::getDirectMother( int idx ) {
    if( gen.Mo1[idx] < 0 ) {
        return -999;
    }

    while( gen.Mo1PdgID[idx] == gen.PdgID[idx] ) {
        idx = gen.Mo1[idx];
    }

    return gen.Mo1[idx];
}

int SelectionMgr::getDirectMotherPdgID( const int& idx ) {
    return gen.PdgID[ getDirectMother( idx ) ];
}

bool SelectionMgr::isXGenParticle( const int& idx, const int& pdgid ) {
    return ( fabs( gen.PdgID[idx] ) == pdgid );
}

int SelectionMgr::getPdgID( const int& idx ) {
    return gen.PdgID[idx];
}


bool SelectionMgr::isCommonMo( const int& idx1, const int& idx2, const int& pdgid ) {
    if ( getDirectMother( idx1 ) != getDirectMother( idx2 ) ) {
        return false;
    }

    if ( fabs( getDirectMotherPdgID( idx1 ) ) != pdgid ) {
        return false;
    }

    return true;
}

void SelectionMgr::cleanHandle() {
    bhandle.clear();
}

bool SelectionMgr::checkPartonTopo() {
    for( int i = 0; i < gsize(); i++ ) {
        if( fabs( gen.PdgID[i] ) == 6 ) {
            if( fabs( gen.Da1PdgID[i] ) == 5 ) {
                bhandle.push_back( gen.Da1[i] );
                continue;
            }

            if( fabs( gen.Da2PdgID[i] ) == 5 ) {
                bhandle.push_back( gen.Da2[i] );
            }
        }
    }

    return bhandle.size() == 2;
}















