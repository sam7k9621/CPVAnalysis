#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "THStack.h"
#include "TFile.h"
using namespace std;

extern TH2*
GetSFHist(const string& tag)
{
    string filename  = mgr::GetSingle<string>( "file"  , CompMgr().GetSubTree( tag ) );
    string title     = mgr::GetSingle<string>( "title" , CompMgr().GetSubTree( tag ) );
    
    TFile* f = TFile::Open( filename.c_str() );
    TH2* h  = (TH2*)( f->Get( title.c_str() )->Clone() );
    h->SetDirectory(0);
    f->Close();
    
    return h;
}

extern double
GetSF(TH2* h, const int& idx)
{
    return CompMgr().GetSFTH2(h, idx);
}

extern void 
WeightMC(const string& sample)
{
    double lumi    = CompMgr().GetSingleData<double>( "lumi" );
    double xs      = mgr::GetSingle<double>( "cross_section", CompMgr().GetSubTree( sample ) );
    double gen_num = mgr::GetSingle<double>( "gen_num", CompMgr().GetSubTree( sample ) );

    CompMgr().Scale( (lumi*xs) / gen_num );
}
