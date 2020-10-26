#include "CPVAnalysis/CompareDataMC/interface/CompareDataMC.h"
#include "ManagerUtils/Common/interface/ProgressBar.hpp"
#include <boost/python.hpp>
using namespace boost::python;
using namespace mgr;
BOOST_PYTHON_MODULE( pluginObservable )
{
    class_<TVector3>( "TVector3", init<double, double, double>() )
    // .def( self + self )
    // .def( float() * self )
    ;
    def( "ProgressBar", ProgressBar );
    def( "Obs3",  Obs3 );
    def( "Obs6",  Obs6 );
    def( "Obs12", Obs12 );
    def( "Obs14", Obs14 );
}
