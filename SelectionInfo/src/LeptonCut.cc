#include "Selection/SelectionInfo/interface/SelectionMgr.h"
#include <iostream>

using namespace std;
using namespace sel;

/*******************************************************************************

 Muon selection

 **** reference muon id selection

 https://twiki.cern.ch/twiki/bin/view/CMS/SWGuideMuonIdRun2#Tight_Muon

 **** reference muon type in line 211

 http://cmsdoxygen.web.cern.ch/cmsdoxygen/CMSSW_8_0_11/doc/html/de/d37/DataFormats_2MuonReco_2interface_2Muon_8h_source.html

*******************************************************************************/
bool SelectionMgr::passMuPt(double pt){
    return (lep.Pt[idx] > pt);
}

bool SelectionMgr::passMuEta(double eta){
    return (fabs(lep.Eta[idx]) < eta);
}

bool SelectionMgr::passMuRelIsoR04(double rel){
    return (RelIsoR04() < rel);
}

bool SelectionMgr::MuInnerTrackDxy_PV(){
    return (fabs(lep.MuInnerTrackDxy_PV[idx])<0.2);
}

bool SelectionMgr::MuInnerTrackDz(){
    return (fabs(lep.MuInnerTrackDz[idx])<0.5);
}

//number of muon hits
bool SelectionMgr::MuNMuonhits(){
    return (lep.MuNMuonhits[idx]>0);
}

//number of matched muon stations
bool  SelectionMgr::MuNMatchedStations(){
    return (lep.MuNMatchedStations[idx]>1);
}

//global normalized chi^2 / dof
bool SelectionMgr::MuGlobalNormalizedChi2(){
    return (lep.MuGlobalNormalizedChi2[idx]<10);
}

//number of track layers with measurement 
bool SelectionMgr::MuNTrackLayersWMeasurement(){
    return (lep.MuNTrackLayersWMeasurement[idx]>5);
}

bool SelectionMgr::MuNPixelLayers(){
    return (lep.MuNPixelLayers[idx]>0);
}

bool SelectionMgr::isGlobalMuon(){
    return !( ( (lep.MuType[idx]) & 0x02 )== 0 );
}

bool SelectionMgr::isTrackerMuon(){
    return !( ( (lep.MuType[idx]) & 0x04 )== 0 );
}
/*******************************************************************************
 Electron selection

 **** reference electron id selection
 
 https://twiki.cern.ch/twiki/bin/view/CMS/CutBasedElectronIdentificationRun2#Cut_Based_Electron_ID_for_Run_2 
 
 https://github.com/ntuhep/bprimeKit/blob/master/src/LeptonNtuplizer_Electron.cc#L101

*******************************************************************************/

bool SelectionMgr::passElPt(double pt){
    return (lep.Pt[idx] > pt);
}

bool SelectionMgr::passElEta(double eta){
    if( fabs(lep.Eta[idx])<1.566 && fabs(lep.Eta[idx])>1.4442 )
        return false;

    return (lep.Eta[idx] < eta);
}

bool SelectionMgr::passElLoose(){
    return lep.EgammaCutBasedEleIdLOOSE[idx];
}

bool SelectionMgr::passElTight(){
    return lep.EgammaCutBasedEleIdTIGHT[idx];
}


