
#include "CPVAnalysis/BaseLineSelector/interface/BaseLineMgr.h"
#include <climits>
#include <iostream>
#include <string>
#include <map>
using namespace std;

void
BaseLineMgr::RegisterWeight()
{
    _ch->SetBranchAddress( "PUWeight", &_puweight);
}

double
BaseLineMgr::GetSFTH2( TH2* hist, const int& idx )
{
    SetIndex(idx);
    double eta = LepAbsEta();
    double pt  = LepPt();
    
    // Re-evaluating to avoid overflow
    eta = min( eta, hist->GetXaxis()->GetXmax() - 0.01 );
    eta = max( eta, hist->GetXaxis()->GetXmin() + 0.01 );
    pt  = min( pt,  hist->GetYaxis()->GetXmax() - 0.01 );

    return hist->GetBinContent( hist->FindFixBin( eta, pt ) );
}

void
BaseLineMgr::InitBtagWeight( const string& tagger, const string& filename)
{
    _calib = new BTagCalibration(tagger, filename);

    for( int i = BTagEntry::OP_LOOSE; i != BTagEntry::OP_RESHAPING; ++i ){
        _reader_map[BTagEntry::OperatingPoint( i )] = BTagCalibrationReader(
                BTagEntry::OperatingPoint( i ),   // operating point
                "central",   // central sys type 
                {"up", "down"} 
                );   // other sys types
        _reader_map[BTagEntry::OperatingPoint( i )].load(
                *_calib,
                BTagEntry::FLAV_B,
                "comb"
          );
    }
}

double
BaseLineMgr::BtagScaleFactor( BTagEntry::OperatingPoint op, const int& idx )
{
    SetIndex(idx);
    return _reader_map.at( op ).eval_auto_bounds(
            "central",
            BTagEntry::FLAV_B,
            JetEta(),
            JetPt()
            );
}
