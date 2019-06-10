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

void
BaseLineMgr::InitJES()
{
    _jecUnc = new JetCorrectionUncertainty( "/wk_cms2/sam7k9621/CMSSW_8_0_19/src/CPVAnalysis/BaseLineSelector/data/Spring16_25nsV6_MC_Uncertainty_AK4PFchs.txt" );
}

double
BaseLineMgr::GetSFTH2Dn( TH2D* hist, const int& idx )
{
    SetIndex( idx );
    double eta = LepEta();
    double pt  = LepPt();

    // Re-evaluating to avoid overflow
    eta = min( eta, hist->GetXaxis()->GetXmax() - 0.01 );
    eta = max( eta, hist->GetXaxis()->GetXmin() + 0.01 );
    pt  = min( pt, hist->GetYaxis()->GetXmax() - 0.01 );

    return hist->GetBinContent( hist->FindFixBin( eta, pt ) ) - hist->GetBinError( hist->FindFixBin( eta, pt ) );
}

double
BaseLineMgr::GetSFTH2Up( TH2D* hist, const int& idx )
{
    SetIndex( idx );
    double eta = LepEta();
    double pt  = LepPt();

    // Re-evaluating to avoid overflow
    eta = min( eta, hist->GetXaxis()->GetXmax() - 0.01 );
    eta = max( eta, hist->GetXaxis()->GetXmin() + 0.01 );
    pt  = min( pt, hist->GetYaxis()->GetXmax() - 0.01 );

    return hist->GetBinContent( hist->FindFixBin( eta, pt ) ) + hist->GetBinError( hist->FindFixBin( eta, pt ) );
}

double
BaseLineMgr::GetSFTH2( TH2D* hist, const int& idx )
{
    SetIndex( idx );
    double eta = LepEta();
    double pt  = LepPt();

    // Re-evaluating to avoid overflow
    eta = min( eta, hist->GetXaxis()->GetXmax() - 0.01 );
    eta = max( eta, hist->GetXaxis()->GetXmin() + 0.01 );
    pt  = min( pt, hist->GetYaxis()->GetXmax() - 0.01 );

    return hist->GetBinContent( hist->FindFixBin( eta, pt ) );
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
    }
}

double
BaseLineMgr::BtagScaleFactor( BTagEntry::OperatingPoint op, const int& idx )
{
    SetIndex( idx );
    return _reader_map.at( op ).eval_auto_bounds(
        "central",
        BTagEntry::FLAV_B,
        JetEta(),
        JetPt()
        );
}

double
BaseLineMgr::BtagScaleFactorUp( BTagEntry::OperatingPoint op, const int& idx )
{
    SetIndex( idx );
    return _reader_map.at( op ).eval_auto_bounds(
        "up",
        BTagEntry::FLAV_B,
        JetEta(),
        JetPt()
        );
}

double
BaseLineMgr::BtagScaleFactorDn( BTagEntry::OperatingPoint op, const int& idx )
{
    SetIndex( idx );
    return _reader_map.at( op ).eval_auto_bounds(
        "down",
        BTagEntry::FLAV_B,
        JetEta(),
        JetPt()
        );
}

double
BaseLineMgr::TopPtWeight()
{
    double topweight     = 1.;
    double antitopweight = 1.;

    for( int i = 0; i < 20; i++ ){// should be in first 20 entries
        if( _gen.PdgID[ i ] == 6 && _gen.nMo[ i ] >= 2 && topweight == 1 ){
            topweight = exp( 0.0615 - 0.0005 * _gen.Pt[ i ] );
        }
        else if( _gen.PdgID[ i ] == -6 && _gen.nMo[ i ] >= 2 && topweight == 1 ){
            antitopweight = exp( 0.0615 - 0.0005 * _gen.Pt[ i ] );
        }

        if( topweight != 1 && antitopweight != 1 ){
            break;
        }
    }

    return sqrt( topweight * antitopweight );
}
