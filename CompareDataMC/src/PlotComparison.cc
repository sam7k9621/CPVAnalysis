#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern Selector&
PlotMgr( const string& subdir, const string& json )
{
    static Selector mgr(subdir, json);
    return mgr;
}

extern void 
MakePlotCompare()
{
    MergeMC();

    vector<string> histlst = { "tmass", "nVtx", "chi2" };
    for(const auto& title : histlst){
        cout<<"Finishing extracting and Plotting " + title<<endl;
        PlotCompare( ExtractMC(title), ExtractData(title), title );
    }
    CleanMC();
}

extern void
MergeMC()
{
    string cmd = "hadd ";
    vector<string> MCsamplelst = PlotMgr().GetListData<string>( "MClst" );
   

    for(const auto& s : MCsamplelst){

        string output =  PlotMgr().DatasDir() / s + "_temp.root ";
        
        string input  =  PlotMgr().GetResultsName("", "FullCut") + "_" + s + "*.root";
        system( (cmd + output + input).c_str() );
    }
}

extern void
CleanMC()
{
    string cmd  = "rm ";
    string file = PlotMgr().DatasDir() /  "*temp.root ";
    system( (cmd + file).c_str() );
}

extern vector<TH1D*>
ExtractMC(const string& title)
{
    //Extracting MC hist
    vector<string> MCsamplelst = PlotMgr().GetListData<string>( "MClst" );
    vector<TH1D*>  mclst;
    
    for(const auto s : MCsamplelst){
        
        string file = PlotMgr().DatasDir() / s + "_temp.root";
        
        TFile* f = TFile::Open( file.c_str() );
        TH1D*  h = (TH1D*)( f->Get( title.c_str() )->Clone() );
        h->SetDirectory(0); 
        
        mclst.push_back(h); 
        f->Close();
    }
    return mclst;
}

extern TH1D*
ExtractData(const string& title)
{
    //Extracting Data hist
    string file = PlotMgr().GetResultsName("", "FullCut") + "_" + "Data" ".root";
    
    TFile* f = TFile::Open( file.c_str() );
    TH1D*  h = (TH1D*)( f->Get( title.c_str() )->Clone() );
    h->SetDirectory(0); 
    f->Close();
    return h; 
}

extern double
GetErrSum(TH1D* hist)
{
    double binerror = 0;
    for( int i = 0; i < hist->GetSize(); ++i ){
        binerror += ( hist->GetBinError( i ) * hist->GetBinError( i ) );
    }

    return sqrt(binerror);
}

