#include "CPVAnalysis/SampleMgr/interface/BaseLineMgr.h"
#include <climits>
#include <iostream>
#include <map>
#include <string>
using namespace std;

void
BaseLineMgr::RegisterWeight( TChain* ch )
{
    ch->SetBranchAddress( "PUWeight",    &_puweight );
    ch->SetBranchAddress( "PUWeight_up", &_puweight_up );
    ch->SetBranchAddress( "PUWeight_dn", &_puweight_dn );
}

double
BaseLineMgr::GetSFTH2( TH2D* hist, double eta, double pt, const int& stat )
{
    // Pt-Eta plot
    if( hist->GetXaxis()->GetXmax() > 3 ){
        // absEta Plot
        if( hist->GetYaxis()->GetXmin() >= 0 ){
            eta = fabs( eta );
        }
        eta = min( eta, hist->GetYaxis()->GetXmax() - 0.01 );
        eta = max( eta, hist->GetYaxis()->GetXmin() + 0.01 );
        pt  = min( pt, hist->GetXaxis()->GetXmax() - 0.01 );

        return hist->GetBinContent( hist->FindFixBin( pt, eta ) ) + stat * hist->GetBinError( hist->FindFixBin( pt, eta ) );
    }
    // Eta-Pt plot
    else{
        // absEta Plot
        if( hist->GetXaxis()->GetXmin() >= 0 ){
            eta = fabs( eta );
        }
        eta = min( eta, hist->GetXaxis()->GetXmax() - 0.01 );
        eta = max( eta, hist->GetXaxis()->GetXmin() + 0.01 );
        pt  = min( pt, hist->GetYaxis()->GetXmax() - 0.01 );

        return hist->GetBinContent( hist->FindFixBin( eta, pt ) ) + stat * hist->GetBinError( hist->FindFixBin( eta, pt ) );
    }
}

void
BaseLineMgr::InitBtagWeight( const string& tagger, const string& filename )
{
    _calib = new BTagCalibration( tagger, filename );

    for( int i = BTagEntry::OP_LOOSE; i != BTagEntry::OP_RESHAPING; ++i ){
        _reader_map[ BTagEntry::OperatingPoint( i ) ] = BTagCalibrationReader(
            BTagEntry::OperatingPoint( i ),// operating point
            "central",// central sys type
            { "up", "down" }
            );// other sys types
        _reader_map[ BTagEntry::OperatingPoint( i ) ].load(
            *_calib,
            BTagEntry::FLAV_B,
            "comb"
            );
        _reader_map[ BTagEntry::OperatingPoint( i ) ].load(
            *_calib,
            BTagEntry::FLAV_C,
            "comb"
            );
        _reader_map[ BTagEntry::OperatingPoint( i ) ].load(
            *_calib,
            BTagEntry::FLAV_UDSG,
            "incl"
            );
    }
}

double
BaseLineMgr::BtagScaleFactor( const int& idx, const BTagEntry::OperatingPoint& op, const BTagEntry::JetFlavor& jf, const std::string& unc )
{
    SetIndex( idx );
    return _reader_map.at( op ).eval_auto_bounds(
        unc,
        jf,
        JetAbsEta(),
        JetPt()
        );
}

double
BaseLineMgr::TopPtWeight()
{
    bool   tweight = false;
    bool   aweight = false;
    double topweight     = 1.;
    double antitopweight = 1.;

    for( int i = 0; i < 20; i++ ){ // should be in first 20 entries
        if( tweight && aweight ){
            break;
        }

        double pt = _gen.Pt[i] > 500 ? 500 : _gen.Pt[i];

        if( _gen.PdgID[ i ] == 6 && _gen.nMo[ i ] >= 2 ){
            tweight = true;
            topweight = exp( 0.0615 - 0.0005 * pt );
        }
        else if( _gen.PdgID[ i ] == -6 && _gen.nMo[ i ] >= 2 ){
            aweight = false;
            antitopweight = exp( 0.0615 - 0.0005 * pt );
        }
    }

    return sqrt( topweight * antitopweight );
}

float
BaseLineMgr::PDFWeight( const int& idx )
{
    // pdf100 10-109 as116 110 as120 111
    return _gen.LHESystematicWeights[ idx ] / _gen.LHEOriginalWeight;
}

float
BaseLineMgr::muFmuRWeight( const int& idx )
{
    // anti-correlated variation are dropped
    // drop nominal, 1006 and 1008
    return _gen.LHESystematicWeights[ idx ] / _gen.LHEOriginalWeight;
}
