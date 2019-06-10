#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern double
Obs13( TVector3 isoLep, TVector3 hardJet, TVector3 b, TVector3 bbar, float charge )
{
    TVector3 v1 = b + bbar;
    TVector3 v2 = isoLep.Cross( hardJet );

    double o13 = double(charge) * v1.Dot( v2 );
    return o13;
}

extern double
Obs3( TVector3 isoLep, TVector3 hardJet, TVector3 b, TVector3 bbar, float charge )
{
    double o3 = double(charge) * ( b ).Dot( isoLep.Cross( hardJet ) );
    return o3;
}

extern double
Obs6( TVector3 isoLep, TVector3 hardJet, TVector3 b, TVector3 bbar, float charge )
{
    TVector3 O6_1v = b - bbar;
    TVector3 O6_2v = isoLep.Cross( hardJet );

    double o6 = double(charge) * ( O6_1v.Dot( O6_2v ) );
    return o6;
}

extern double
Obs12( TVector3 b, TVector3 bbar )
{
    TVector3 az;
    az.SetXYZ( 0, 0, 1 );

    double v1 = az.Dot( b - bbar );
    double v2 = az.Dot( b.Cross( bbar ) );

    double O12 = v1 * v2;
    return O12;
}
