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
PlotCompare()
{
    cout<<PlotMgr().SettingsDir()<<endl;
    MergeMC();
    //vector<TH1D*> mclst = ExtractMC();
    //TH1D*        data  = ExtractData();

    /*PlotMass(mclst, data);*/
}

extern void
MergeMC()
{
    string cmd = "hadd ";
    vector<string> MCsamplelst = PlotMgr().GetListData<string>( "MClst" );
    
    for(const auto& s : MCsamplelst){

        string output =  PlotMgr().DatasDir() / s + ".root";
        string input  =  PlotMgr().ResultsDir() / "Fullcut_" + CompMgr().GetOption<string>("lepton") + "_" + s + "*.root";
        //system( (cmd + output + input).c_str() );
        cout<<cmd + output + input<<endl;
    }
}

extern vector<TH1D*>
ExtractMC(const string& title)
{
    //Extracting MC hist
    vector<string> MCsamplelst = PlotMgr().GetListData<string>( "MClst" );
    vector<TH1D*>  mclst;
    
    for(const auto s : MCsamplelst){
        
        string file = PlotMgr().DatasDir() / s + ".root";
        
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

    string file = PlotMgr().ResultsDir() / "FullCut_" + CompMgr().GetOption<string>("lepton") + ".root";
    
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
