#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include <boost/python.hpp>
using namespace boost::python;

BOOST_PYTHON_MODULE( pluginObservable )
{
    class_<TVector3>( "TVector3", init<double, double, double>() )
    // .def( self + self )
    // .def( float() * self )
    ;
    def( "Obs3",  Obs3 );
    def( "Obs6",  Obs6 );
    def( "Obs12", Obs12 );
    def( "Obs13", Obs13 );
}
