#include "CPVAnalysis/SelectionInfo/interface/SelectionMgr.h"

using namespace std;
using namespace sel;


/*******************************************************************************

 Jet selection

*******************************************************************************/

bool SelectionMgr::passJetPt(double pt ){
     return (jet.Pt[idx] > 30);
}

bool SelectionMgr::passJetEta(double eta){
    return ( fabs(jet.Eta[idx]) < 2.4 );
}

bool SelectionMgr::JetNConstituents(){
    return ( jet.NConstituents[idx] > 1 );
}

bool SelectionMgr::JetNEF(){
    return ( jet.NEF[idx] < 0.99 );
}

bool SelectionMgr::JetNHF(){
    return ( jet.NHF[idx] < 0.99 );
}

bool SelectionMgr::JetCHF(){
    return ( jet.CHF[idx] > 0 );
}

bool SelectionMgr::JetNCH(){
    return ( jet.NCH[idx] > 0 );
}

bool SelectionMgr::JetCEF(){
    return ( jet.CEF[idx] < 0.99 );
}

//b-tagging 
bool SelectionMgr::JetCSVM(){
    return ( jet.pfCombinedInclusiveSecondaryVertexV2BJetTags[idx] > 0.679 );
}

