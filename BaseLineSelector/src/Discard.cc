#include "CPVAnalysis/BaseLineSelector/interface/Selection.h"
#include "ManagerUtils/SysUtils/interface/PathUtils/CommonPath.hpp"

using namespace std;

extern void
Discard_FullCut( TTree* tree )
{
    tree->SetBranchStatus( "*",                                     0 );
   
    tree->SetBranchStatus( "PUWeight",                              1 );   
    tree->SetBranchStatus( "PUWeight_up",                           1 );       
    tree->SetBranchStatus( "PUWeight_dn",                           1 );   

    tree->SetBranchStatus( "GenInfo.LHESystematicWeights",          1 );
    tree->SetBranchStatus( "GenInfo.LHEOriginalWeight",             1 );
    tree->SetBranchStatus( "GenInfo.Weight",                        1 );

    tree->SetBranchStatus( "LepInfo.Pt",                            1 );
    tree->SetBranchStatus( "LepInfo.Eta",                           1 );
    tree->SetBranchStatus( "LepInfo.Phi",                           1 );
    tree->SetBranchStatus( "LepInfo.GenPt",                         1 );
    tree->SetBranchStatus( "LepInfo.GenEta",                        1 );
    tree->SetBranchStatus( "LepInfo.GenPhi",                        1 );
    tree->SetBranchStatus( "LepInfo.Px",                            1 );
    tree->SetBranchStatus( "LepInfo.Py",                            1 );
    tree->SetBranchStatus( "LepInfo.Pz",                            1 );
    tree->SetBranchStatus( "LepInfo.Energy",                        1 );
    tree->SetBranchStatus( "LepInfo.Charge",                        1 );
    
    tree->SetBranchStatus( "JetInfo.Pt",                            1 );
    tree->SetBranchStatus( "JetInfo.Eta",                           1 );
    tree->SetBranchStatus( "JetInfo.Phi",                           1 );
    tree->SetBranchStatus( "JetInfo.GenPt",                         1 );
    tree->SetBranchStatus( "JetInfo.GenEta",                        1 );
    tree->SetBranchStatus( "JetInfo.GenPhi",                        1 );
    tree->SetBranchStatus( "JetInfo.Px",                            1 );
    tree->SetBranchStatus( "JetInfo.Py",                            1 );
    tree->SetBranchStatus( "JetInfo.Pz",                            1 );
    tree->SetBranchStatus( "JetInfo.Energy",                        1 );
    tree->SetBranchStatus( "JetInfo.pfDeepCSVJetTags_probb",        1 );
    tree->SetBranchStatus( "JetInfo.pfDeepCSVJetTags_probbb",       1 );
}


extern void
Discard( TChain* ch )
{
    ch->SetBranchStatus( "EvtInfo.EvtNo",                                      0 );
    ch->SetBranchStatus( "EvtInfo.BxNo",                                       0 );
    ch->SetBranchStatus( "EvtInfo.Orbit",                                      0 );
    ch->SetBranchStatus( "EvtInfo.McFlag",                                     0 );
    ch->SetBranchStatus( "EvtInfo.EventTime",                                  0 );
    ch->SetBranchStatus( "EvtInfo.Flag_METFilter",                             0 );
    ch->SetBranchStatus( "EvtInfo.RhoNoFastJet",                               0 );
    ch->SetBranchStatus( "EvtInfo.BeamSpotX",                                  0 );
    ch->SetBranchStatus( "EvtInfo.BeamSpotY",                                  0 );
    ch->SetBranchStatus( "EvtInfo.BeamSpotZ",                                  0 );
    ch->SetBranchStatus( "EvtInfo.BSsigmaZ",                                   0 );
    ch->SetBranchStatus( "EvtInfo.PFMETPhi",                                   0 );
    ch->SetBranchStatus( "EvtInfo.PFMETType1CorrPtShiftJetEnUp",               0 );
    ch->SetBranchStatus( "EvtInfo.PFMETType1CorrPtShiftJetEnDown",             0 );
    ch->SetBranchStatus( "EvtInfo.PFMETType1CorrPtShiftJetResUp",              0 );
    ch->SetBranchStatus( "EvtInfo.PFMETType1CorrPtShiftJetResDown",            0 );
    ch->SetBranchStatus( "EvtInfo.PFMETType1CorrPtShiftUnclusteredEnUp",       0 );
    ch->SetBranchStatus( "EvtInfo.PFMETType1CorrPtShiftUnclusteredEnDown",     0 );
    ch->SetBranchStatus( "EvtInfo.PFMETType1CorrPhiShiftJetEnUp",              0 );
    ch->SetBranchStatus( "EvtInfo.PFMETType1CorrPhiShiftJetEnDown",            0 );
    ch->SetBranchStatus( "EvtInfo.PFMETType1CorrPhiShiftJetResUp",             0 );
    ch->SetBranchStatus( "EvtInfo.PFMETType1CorrPhiShiftJetResDown",           0 );
    ch->SetBranchStatus( "EvtInfo.PFMETType1CorrPhiShiftUnclusteredEnUp",      0 );
    ch->SetBranchStatus( "EvtInfo.PFMETType1CorrPhiShiftUnclusteredEnDown",    0 );
    ch->SetBranchStatus( "EvtInfo.PFRawMET",                                   0 );
    ch->SetBranchStatus( "EvtInfo.PFRawMETPhi",                                0 );
    ch->SetBranchStatus( "EvtInfo.PFSumEt",                                    0 );
    ch->SetBranchStatus( "EvtInfo.PFMETSig",                                   0 );
    ch->SetBranchStatus( "EvtInfo.PFMETlongitudinal",                          0 );
    ch->SetBranchStatus( "EvtInfo.PFMETRealSig",                               0 );
    ch->SetBranchStatus( "EvtInfo.PFGenMET",                                   0 );
    ch->SetBranchStatus( "EvtInfo.PFGenMETPhi",                                0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMET",                                   0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETPhi",                                0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETType1CorrPtShiftJetEnUp",            0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETType1CorrPtShiftJetEnDown",          0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETType1CorrPtShiftJetResUp",           0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETType1CorrPtShiftJetResDown",         0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETType1CorrPtShiftUnclusteredEnUp",    0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETType1CorrPtShiftUnclusteredEnDown",  0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETType1CorrPhiShiftJetEnUp",           0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETType1CorrPhiShiftJetEnDown",         0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETType1CorrPhiShiftJetResUp",          0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETType1CorrPhiShiftJetResDown",        0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETType1CorrPhiShiftUnclusteredEnUp",   0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETType1CorrPhiShiftUnclusteredEnDown", 0 );
    ch->SetBranchStatus( "EvtInfo.PuppiRawMET",                                0 );
    ch->SetBranchStatus( "EvtInfo.PuppiRawMETPhi",                             0 );
    ch->SetBranchStatus( "EvtInfo.PuppiSumEt",                                 0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETSig",                                0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETlongitudinal",                       0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETRealSig",                            0 );
    ch->SetBranchStatus( "EvtInfo.PuppiGenMET",                                0 );
    ch->SetBranchStatus( "EvtInfo.PuppiGenMETPhi",                             0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETx",                                  0 );
    ch->SetBranchStatus( "EvtInfo.PuppiMETy",                                  0 );

    ch->SetBranchStatus( "GenInfo.PhotonFlag",                                 0 );
    ch->SetBranchStatus( "GenInfo.Mo1Status",                                  0 );
    ch->SetBranchStatus( "GenInfo.Mo2Status",                                  0 );
    ch->SetBranchStatus( "GenInfo.GrandMo1PdgID",                              0 );
    ch->SetBranchStatus( "GenInfo.GrandMo2PdgID",                              0 );
    ch->SetBranchStatus( "GenInfo.GrandMo1Status",                             0 );
    ch->SetBranchStatus( "GenInfo.GrandMo2Status",                             0 );
    ch->SetBranchStatus( "GenInfo.McIsTZTZ",                                   0 );
    ch->SetBranchStatus( "GenInfo.McIsTHTH",                                   0 );
    ch->SetBranchStatus( "GenInfo.McIsTZTH",                                   0 );
    ch->SetBranchStatus( "GenInfo.McIsTZBW",                                   0 );
    ch->SetBranchStatus( "GenInfo.McIsTHBW",                                   0 );
    ch->SetBranchStatus( "GenInfo.McIsBWBW",                                   0 );
    ch->SetBranchStatus( "GenInfo.ptHat",                                      0 );

    ch->SetBranchStatus( "JetInfo.Index",                                      0 );
    ch->SetBranchStatus( "JetInfo.NTracks",                                    0 );
    ch->SetBranchStatus( "JetInfo.JetCharge",                                  0 );
    ch->SetBranchStatus( "JetInfo.Pt_MuonCleaned",                             0 );
    ch->SetBranchStatus( "JetInfo.Eta_MuonCleaned",                            0 );
    ch->SetBranchStatus( "JetInfo.Phi_MuonCleaned",                            0 );
    ch->SetBranchStatus( "JetInfo.Energy_MuonCleaned",                         0 );
    ch->SetBranchStatus( "JetInfo.QGTagsLikelihood",                           0 );
    ch->SetBranchStatus( "JetInfo.PUJetIDfullDiscriminant",                    0 );
    ch->SetBranchStatus( "JetInfo.PUJetIDcutbased",                            0 );
    ch->SetBranchStatus( "JetInfo.PtCorrL2",                                   0 );
    ch->SetBranchStatus( "JetInfo.PtCorrL3",                                   0 );
    ch->SetBranchStatus( "JetInfo.summaryDeepCSVJetTags_BvsAll",               0 );
    ch->SetBranchStatus( "JetInfo.summaryDeepCSVJetTags_CvsB",                 0 );
    ch->SetBranchStatus( "JetInfo.summaryDeepCSVJetTags_CvsL",                 0 );
    ch->SetBranchStatus( "JetInfo.GenPdgID",                                   0 );
    ch->SetBranchStatus( "JetInfo.GenHadronFlavor",                            0 );
    ch->SetBranchStatus( "JetInfo.GenMCTag",                                   0 );
    ch->SetBranchStatus( "JetInfo.NSubjets",                                   0 );
    ch->SetBranchStatus( "JetInfo.SubjetsIdxStart",                            0 );
    ch->SetBranchStatus( "JetInfo.NjettinessAK8tau1",                          0 );
    ch->SetBranchStatus( "JetInfo.NjettinessAK8tau2",                          0 );
    ch->SetBranchStatus( "JetInfo.NjettinessAK8tau3",                          0 );
    ch->SetBranchStatus( "JetInfo.PuppiSoftDrop_ECFb1N2",                      0 );
    ch->SetBranchStatus( "JetInfo.PuppiSoftDrop_ECFb1N3",                      0 );
    ch->SetBranchStatus( "JetInfo.ak8PFJetsCHSSoftDropMass",                   0 );
    ch->SetBranchStatus( "JetInfo.ak8PFJetsCHSPrunedMass",                     0 );
    ch->SetBranchStatus( "JetInfo.ak8PFJetsCHSTrimmedMass",                    0 );
    ch->SetBranchStatus( "JetInfo.ak8PFJetsCHSFilteredMass",                   0 );
    ch->SetBranchStatus( "JetInfo.topJetMass",                                 0 );
    ch->SetBranchStatus( "JetInfo.ca8TopMass",                                 0 );
    ch->SetBranchStatus( "JetInfo.ca8MinMass",                                 0 );
    ch->SetBranchStatus( "JetInfo.SubjetMass",                                 0 );
    ch->SetBranchStatus( "JetInfo.SubjetPt",                                   0 );
    ch->SetBranchStatus( "JetInfo.SubjetEt",                                   0 );
    ch->SetBranchStatus( "JetInfo.SubjetEta",                                  0 );
    ch->SetBranchStatus( "JetInfo.SubjetPhi",                                  0 );
    ch->SetBranchStatus( "JetInfo.SubjetArea",                                 0 );
    ch->SetBranchStatus( "JetInfo.SubjetPtUncorr",                             0 );
    ch->SetBranchStatus( "JetInfo.SubjetCombinedSVBJetTags",                   0 );
    ch->SetBranchStatus( "JetInfo.SubjetDeepCSVJetTags_probb",                 0 );
    ch->SetBranchStatus( "JetInfo.SubjetDeepCSVJetTags_probbb",                0 );
    ch->SetBranchStatus( "JetInfo.SubjetDeepCSVJetTags_probc",                 0 );
    ch->SetBranchStatus( "JetInfo.SubjetDeepCSVJetTags_probudsg",              0 );
    ch->SetBranchStatus( "JetInfo.SubjetsummaryDeepCSVJetTags_BvsAll",         0 );
    ch->SetBranchStatus( "JetInfo.SubjetsummaryDeepCSVJetTags_CvsB",           0 );
    ch->SetBranchStatus( "JetInfo.SubjetsummaryDeepCSVJetTags_CvsL",           0 );
    ch->SetBranchStatus( "JetInfo.PuppiSoftDrop_SubjetECFb1N2",                0 );
    ch->SetBranchStatus( "JetInfo.PuppiSoftDrop_SubjetECFb1N3",                0 );
    ch->SetBranchStatus( "JetInfo.SubjetGenPdgId",                             0 );
    ch->SetBranchStatus( "JetInfo.SubjetGenFlavour",                           0 );
    ch->SetBranchStatus( "JetInfo.SubjetHadronFlavour",                        0 );
    ch->SetBranchStatus( "JetInfo.TempPt1",                                    0 );
    ch->SetBranchStatus( "JetInfo.TempEta1",                                   0 );
    ch->SetBranchStatus( "JetInfo.TempPhi1",                                   0 );
    ch->SetBranchStatus( "JetInfo.TempEnergy1",                                0 );
    ch->SetBranchStatus( "JetInfo.TempPt2",                                    0 );
    ch->SetBranchStatus( "JetInfo.TempEta2",                                   0 );
    ch->SetBranchStatus( "JetInfo.TempPhi2",                                   0 );
    ch->SetBranchStatus( "JetInfo.TempEnergy2",                                0 );
    ch->SetBranchStatus( "JetInfo.TempFloat1",                                 0 );
    ch->SetBranchStatus( "JetInfo.TempFloat2",                                 0 );
    ch->SetBranchStatus( "JetInfo.TempFloat3",                                 0 );
    ch->SetBranchStatus( "JetInfo.TempFloat4",                                 0 );

    ch->SetBranchStatus( "LepInfo.Index",                                      0 );
    ch->SetBranchStatus( "LepInfo.EcalPFIsoRhoCorr2016",                       0 );
    ch->SetBranchStatus( "LepInfo.HcalPFIsoRhoCorr2016",                       0 );
    ch->SetBranchStatus( "LepInfo.Ip3dPV",                                     0 );
    ch->SetBranchStatus( "LepInfo.Ip3dPVErr",                                  0 );
    ch->SetBranchStatus( "LepInfo.Ip3dPVSignificance",                         0 );
    ch->SetBranchStatus( "LepInfo.MiniIso",                                    0 );
    ch->SetBranchStatus( "LepInfo.MuonCutBasedMuIdLOOSE",                      0 );
    ch->SetBranchStatus( "LepInfo.MuonCutBasedMuIdMEDIUM",                     0 );
    ch->SetBranchStatus( "LepInfo.MuonCutBasedMuIdTIGHT",                      0 );
    ch->SetBranchStatus( "LepInfo.MuonCutBasedIdGlobalHighPt",                 0 );
    ch->SetBranchStatus( "LepInfo.MuonCutBasedIdTrkHighPt",                    0 );
    ch->SetBranchStatus( "LepInfo.isGoodMuonTMOneStationTight",                0 );
    ch->SetBranchStatus( "LepInfo.isGoodMuonGlobalMuonPromptTight",            0 );
    ch->SetBranchStatus( "LepInfo.MuInnerPt",                                  0 );
    ch->SetBranchStatus( "LepInfo.MuInnerEta",                                 0 );
    ch->SetBranchStatus( "LepInfo.MuInnerPhi",                                 0 );
    ch->SetBranchStatus( "LepInfo.MuInnerPtError",                             0 );
    ch->SetBranchStatus( "LepInfo.innerTracknormalizedChi2",                   0 );
    ch->SetBranchStatus( "LepInfo.MuInnerTrackVHitFrac",                       0 );
    ch->SetBranchStatus( "LepInfo.MuInnerTrackHighPurity",                     0 );
    ch->SetBranchStatus( "LepInfo.MuInnerTrackNHits",                          0 );
    ch->SetBranchStatus( "LepInfo.MuDThits",                                   0 );
    ch->SetBranchStatus( "LepInfo.MuCSChits",                                  0 );
    ch->SetBranchStatus( "LepInfo.MuRPChits",                                  0 );
    ch->SetBranchStatus( "LepInfo.MuSegCompatibility",                         0 );
    ch->SetBranchStatus( "LepInfo.MuSTATKMatchingChi2",                        0 );
    ch->SetBranchStatus( "LepInfo.MuInnerTrackKinkVal",                        0 );
    ch->SetBranchStatus( "LepInfo.MuStationMask",                              0 );
    ch->SetBranchStatus( "LepInfo.MuBestTrackType",                            0 );
    ch->SetBranchStatus( "LepInfo.MuBestTrackPt",                              0 );
    ch->SetBranchStatus( "LepInfo.MuBestTrackEta",                             0 );
    ch->SetBranchStatus( "LepInfo.MuBestTrackPhi",                             0 );
    ch->SetBranchStatus( "LepInfo.MuBestTrackPtError",                         0 );
    ch->SetBranchStatus( "LepInfo.MuBestTrackDz",                              0 );
    ch->SetBranchStatus( "LepInfo.MuBestTrackDxy",                             0 );
    ch->SetBranchStatus( "LepInfo.MuBestTrackNMuonhits",                       0 );
    ch->SetBranchStatus( "LepInfo.MuTunePBestTrackType",                       0 );
    ch->SetBranchStatus( "LepInfo.MuTunePBestTrackPt",                         0 );
    ch->SetBranchStatus( "LepInfo.MuTunePBestTrackEta",                        0 );
    ch->SetBranchStatus( "LepInfo.MuTunePBestTrackPhi",                        0 );
    ch->SetBranchStatus( "LepInfo.MuTunePBestTrackPtError",                    0 );
    ch->SetBranchStatus( "LepInfo.MuTunePBestTrackDz",                         0 );
    ch->SetBranchStatus( "LepInfo.MuTunePBestTrackDxy",                        0 );
    ch->SetBranchStatus( "LepInfo.MuTunePBestTrackNMuonhits",                  0 );
    ch->SetBranchStatus( "LepInfo.MuontimenDof",                               0 );
    ch->SetBranchStatus( "LepInfo.MuontimeAtIpInOut",                          0 );
    ch->SetBranchStatus( "LepInfo.MuontimeAtIpOutIn",                          0 );
    ch->SetBranchStatus( "LepInfo.Muondirection",                              0 );
    ch->SetBranchStatus( "LepInfo.isPFElectron",                               0 );
    ch->SetBranchStatus( "LepInfo.EtaSC",                                      0 );
    ch->SetBranchStatus( "LepInfo.PhiSC",                                      0 );
    ch->SetBranchStatus( "LepInfo.rawEnergySC",                                0 );
    ch->SetBranchStatus( "LepInfo.R9",                                         0 );
    ch->SetBranchStatus( "LepInfo.E1x5",                                       0 );
    ch->SetBranchStatus( "LepInfo.E2x5Max",                                    0 );
    ch->SetBranchStatus( "LepInfo.E5x5",                                       0 );
    ch->SetBranchStatus( "LepInfo.SigmaIetaIphi",                              0 );
    ch->SetBranchStatus( "LepInfo.SigmaIphiIphi",                              0 );
    ch->SetBranchStatus( "LepInfo.EtaWidth",                                   0 );
    ch->SetBranchStatus( "LepInfo.PhiWidth",                                   0 );
    ch->SetBranchStatus( "LepInfo.HcalDepth1Iso",                              0 );
    ch->SetBranchStatus( "LepInfo.HcalDepth2Iso",                              0 );
    ch->SetBranchStatus( "LepInfo.HEEPTrackIso",                               0 );
    ch->SetBranchStatus( "LepInfo.EgammaMVANonTrig",                           0 );
    ch->SetBranchStatus( "LepInfo.EgammaMVATrig",                              0 );
    ch->SetBranchStatus( "LepInfo.isEcalDriven",                               0 );
    ch->SetBranchStatus( "LepInfo.isTrackerDriven",                            0 );
    ch->SetBranchStatus( "LepInfo.isPFTau",                                    0 );
    ch->SetBranchStatus( "LepInfo.GenMCTag",                                   0 );
    ch->SetBranchStatus( "LepInfo.TempPt1",                                    0 );
    ch->SetBranchStatus( "LepInfo.TempEta1",                                   0 );
    ch->SetBranchStatus( "LepInfo.TempPhi1",                                   0 );
    ch->SetBranchStatus( "LepInfo.TempEnergy1",                                0 );
    ch->SetBranchStatus( "LepInfo.TempPt2",                                    0 );
    ch->SetBranchStatus( "LepInfo.TempEta2",                                   0 );
    ch->SetBranchStatus( "LepInfo.TempPhi2",                                   0 );
    ch->SetBranchStatus( "LepInfo.TempEnergy2",                                0 );
    ch->SetBranchStatus( "LepInfo.TempFloat1",                                 0 );
    ch->SetBranchStatus( "LepInfo.TempFloat2",                                 0 );
    ch->SetBranchStatus( "LepInfo.TempFloat3",                                 0 );
    ch->SetBranchStatus( "LepInfo.TempFloat4",                                 0 );

    ch->SetBranchStatus( "Photon*",                                            0 );
}
