#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern double
Obs2( TVector3 isoLep, TVector3 hardJet, TVector3 b, TVector3 bbar )
{
    TVector3 v1 =  b + bbar;
    TVector3 v2 = isoLep.Cross( hardJet );
    
    double o2 = v1.Dot( v2 );
    return o2;
}

extern double
Obs3( TVector3 isoLep, TVector3 hardJet, TVector3 b, TVector3 bbar, float charge )
{
    double o3 =  double(charge) * ( b  ).Dot( isoLep.Cross( hardJet ) );
    return o3;
}

extern double
Obs4( TVector3 isoLep, TVector3 hardJet, TVector3 b, TVector3 bbar, float charge )
{
    TVector3 O4_1v = b - bbar;
    TVector3 O4_2v = isoLep.Cross( hardJet );

    double o4 = double(charge) * ( O4_1v.Dot( O4_2v ) );
    return o4;
}

extern double 
Obs7( TVector3 b, TVector3 bbar )
{
    TVector3 az;
    az.SetXYZ(0, 0, 1);

    double v1 = az.Dot( b - bbar );
    double v2 = az.Dot( b.Cross( bbar ));
    
    double O7 = v1 * v2;
    return O7;
}
