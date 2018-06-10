#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
using namespace std;

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
