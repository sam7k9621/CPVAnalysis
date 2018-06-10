#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/PlotUtils/interface/Common.hpp"
#include "THStack.h"
using namespace std;

extern double
Obs3( TVector3 isoLep, TVector3 hardJet, TVector3 b, float charge )
{
    return double(charge) * ( b.Dot( isoLep.Cross( hardJet ) ) );
}

extern double
Obs4( TVector3 isoLep, TVector3 hardJet, TVector3 b, TVector3 bbar, float charge )
{
    TVector3 O4_1v = b - bbar;
    TVector3 O4_2v = isoLep.Cross( hardJet );

    return double(charge) * ( O4_1v.Dot( O4_2v ) );
}
