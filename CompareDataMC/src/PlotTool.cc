#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "TFile.h"
using namespace std;

extern vector<TH1D*>
ExtractMC( const string& title )
{
     //Extracting MC hist
    string region = PlotMgr().GetOption<string>("mc");
    vector<string> MCsamplelst = PlotMgr().GetListData<string>( "MClst" );
    vector<TH1D*> mclst;

    for( const auto s : MCsamplelst ){
        string file = GetFilename() + "_" + region + ".root";
        file.insert( file.find( "HistMerge" ) + 12, "_" + s );

        cout<<"[ "<<file<<" ]"<<endl;
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
    string region = PlotMgr().GetOption<string>("mc");
    vector<string> MCsamplelst = PlotMgr().GetListData<string>( "MClst" );
    vector<TH2D*> mclst;

    for( const auto s : MCsamplelst ){
        string file = GetFilename() + "_" + region + ".root";
        file.insert( file.find( "HistMerge" ) + 12, "_" + s );

        TFile* f = TFile::Open( file.c_str() );
        TH2D* h  = (TH2D*)( f->Get( title.c_str() )->Clone() );
        h->SetDirectory( 0 );

        mclst.push_back( h );
        f->Close();
    }

    return mclst;
}

extern TH1D*
ExtractData( const string& title, string region )
{
     //Extracting Data hist
    if( region == "" ){
        region = PlotMgr().GetOption<string>("data");
    }
    string file = GetFilename() + "_" + region + ".root";
    file.insert( file.find( "HistMerge" ) + 12, "_Data" );
    
    cout<<"[ "<<file<<" ]"<<endl;
    TFile* f = TFile::Open( file.c_str() );
    TH1D* h  = (TH1D*)( f->Get( title.c_str() )->Clone() );
    h->SetDirectory( 0 );
    f->Close();
    return h;
}

extern string
GetFilename()
{
    string filename = PlotMgr().GetResultsName( "", "HistMerge" );
    auto pos = filename.find( "_region" );
    if( pos != std::string::npos ){
        filename.erase( filename.begin() + pos, filename.end() );
    }

    return filename;
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
extern void
SetYTitle( TH1* plot )
{
    const double ymax = plot->GetMaximum();
    string ytitle = plot->GetYaxis()->GetTitle();
    
    int order = 0;
    double loop = ymax;
    while( (loop / 1000) > 1 ){
        order++;
        loop /= 1000;
    }
    
    if(order){
        ytitle += " x 10^{" + to_string(order*3) + "}";
    }
    plot->GetYaxis()->SetTitle( ytitle.c_str() );
}

extern string
GetName( TH1D* h )
{
    string name  = h->GetName();
    size_t found = name.find( "_" );
    if( found != std::string::npos ){
        return name.substr( 0, found );
    }
    else{
        return name;
    }
}

extern TH1D*
SumHist( vector<TH1D*> histlst )
{
    TH1D* hist = (TH1D*)histlst[ 0 ]->Clone();

    for( int i = 1; i < (int)histlst.size(); i++ ){
        hist->Add( histlst[ i ] );
    }

    return hist;
}

extern TH2D*
SumHist2D( vector<TH2D*> histlst )
{
    TH2D* hist = (TH2D*)histlst[ 0 ]->Clone();

    for( int i = 1; i < (int)histlst.size(); i++ ){
        hist->Add( histlst[ i ] );
    }

    return hist;
}
