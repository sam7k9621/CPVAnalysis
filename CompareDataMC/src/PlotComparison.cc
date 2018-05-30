#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern Histor&
PlotMgr( const string& subdir, const string& json )
{
    static Histor mgr( subdir, json );
    return mgr;
}

extern void
MakePlotCompare()
{
    MergeMC();
    string lepton = PlotMgr().GetOption<string>("lepton");
    string entry  = PlotMgr().GetSingleData<string>( lepton + "entry");

    vector<string> histlst = { "lep_tmass", "had_tmass", "nVtx", "chi2" };
    //vector<string> histlst = { 
        //"LJetPt1",
        //"LJetPt2",
        //"LJetPt3",
        //"LJetPt4",
        //"LLepPt1",
        //"LLepPt2",
        //"LLepPt3",
        //"LLepPt4",
    //};

    for( const auto& title : histlst ){
        cout << "Finishing extracting and Plotting " + title << endl;
        PlotCompare( ExtractMC( title ), ExtractData( title ), title, entry );
    }

    GetYield( ExtractMC( "lep_tmass" ), ExtractData( "lep_tmass" ) );

    vector<string> mclst = { "Obs3", "Obs4" };

    for( const auto& title : mclst ){
        PlotMC( ExtractMC( title ), title, entry );
    }

    PlotPDF(
        ExtractMC( "chi2_Correct" ),
        ExtractMC( "chi2_Misid" ),
        ExtractMC( "chi2_Mistag" ),
        ExtractMC( "chi2_Nomatched" )
        );

    Plot2D( ExtractMC2D( "chi2_tmass" ) );

    CleanMC();
}

extern void
MergeMC()
{
    string cmd                 = "hadd -f ";
    vector<string> MCsamplelst = PlotMgr().GetListData<string>( "MClst" );

    for( const auto& s : MCsamplelst ){
        string output = PlotMgr().DatasDir() / s + "_temp.root ";

        string input = ( PlotMgr().GetResultsName( "", "Hist" ) + ".root" );
        input.insert( input.find( "Hist" ) + 8, s + "*_" ) ;
        system( ( cmd + output + input ).c_str() );
    }
}

extern void
CleanMC()
{
    string cmd  = "rm ";
    string file = PlotMgr().DatasDir() / "*temp.root ";
    system( ( cmd + file ).c_str() );
}

extern vector<TH1D*>
ExtractMC( const string& title )
{
     //Extracting MC hist
    vector<string> MCsamplelst = PlotMgr().GetListData<string>( "MClst" );
    vector<TH1D*> mclst;

    for( const auto s : MCsamplelst ){
        string file = PlotMgr().DatasDir() / s + "_temp.root";

        TFile* f = TFile::Open( file.c_str() );
        TH1D* h  = (TH1D*)( f->Get( title.c_str() )->Clone() );
        h->SetDirectory( 0 );

        mclst.push_back( h );
        f->Close();
    }

    return mclst;
}

extern vector<TH2D*>
ExtractMC2D( const string& title )
{
     //Extracting MC hist
    vector<string> MCsamplelst = PlotMgr().GetListData<string>( "MClst" );
    vector<TH2D*> mclst;

    for( const auto s : MCsamplelst ){
        string file = PlotMgr().DatasDir() / s + "_temp.root";

        TFile* f = TFile::Open( file.c_str() );
        TH2D* h  = (TH2D*)( f->Get( title.c_str() )->Clone() );
        h->SetDirectory( 0 );

        mclst.push_back( h );
        f->Close();
    }

    return mclst;
}

extern TH1D*
ExtractData( const string& title )
{
     //Extracting Data hist
    string file = PlotMgr().GetResultsName( "", "Hist" ) + ".root";
    file.insert( file.find( "Hist" ) + 8, "Data_" );

    TFile* f = TFile::Open( file.c_str() );
    TH1D* h  = (TH1D*)( f->Get( title.c_str() )->Clone() );
    h->SetDirectory( 0 );
    f->Close();
    return h;
}

extern double
GetErrSum( TH1D* hist )
{
    double binerror = 0;

    for( int i = 0; i < hist->GetSize(); ++i ){
        binerror += ( hist->GetBinError( i ) * hist->GetBinError( i ) );
    }

    return sqrt( binerror );
}
