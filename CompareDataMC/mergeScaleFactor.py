#!/bin/env python
#*************************************************************************
#
#  Filename    : mergeScaleFactor.py
#  Description : Merging Scale factor from different objects and
#  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
#
#*************************************************************************
import argparse
import os
import ROOT

mergelist = {
    'muid': [
        {
            'filename': '/src/CPVAnalysis/CompareDataMC/data/IDSF_BtoF.root',
            'fileobj': 'MC_NUM_TightID_DEN_genTracks_PAR_pt_eta/abseta_pt_ratio',
            'lumi': 20.1
        },
        {
            'filename': '/src/CPVAnalysis/CompareDataMC/data/IDSF_GH.root',
            'fileobj': 'MC_NUM_TightID_DEN_genTracks_PAR_pt_eta/abseta_pt_ratio',
            'lumi': 16.3
        }
    ],
    'muiso': [
        {
            'filename': '/src/CPVAnalysis/CompareDataMC/data/ISOSF_BtoF.root',
            'fileobj': 'TightISO_TightID_pt_eta/abseta_pt_ratio',
            'lumi': 20.1
        },
        {
            'filename': '/src/CPVAnalysis/CompareDataMC/data/ISOSF_GH.root',
            'fileobj': 'TightISO_TightID_pt_eta/abseta_pt_ratio',
            'lumi': 16.3
        }
    ],
    'mutrig16':[
        {
            'filename': '/src/CPVAnalysis/CompareDataMC/data/EfficienciesAndSF_RunBtoF.root',
            'fileobj': 'IsoMu24_OR_IsoTkMu24_PtEtaBins/abseta_pt_ratio',
            'lumi': 35.9
        }
    ],
    'mutrig17':[
        {
            'filename': '/src/CPVAnalysis/CompareDataMC/data/EfficienciesAndSF_RunBtoF_Nov17Nov2017.root',
            'fileobj': 'IsoMu27_PtEtaBins/abseta_pt_ratio',
            'lumi': 41.5
        }
    ],
    'mutrig18':[
        {
            'filename': '/src/CPVAnalysis/CompareDataMC/data/EfficienciesAndSF_2018Data_BeforeMuonHLTUpdate.root',
            'fileobj': 'IsoMu24_PtEtaBins/abseta_pt_ratio',
            'lumi': 8.95
        },
        {
            'filename': '/src/CPVAnalysis/CompareDataMC/data/EfficienciesAndSF_2018Data_AfterMuonHLTUpdate.root',
            'fileobj': 'IsoMu24_PtEtaBins/abseta_pt_ratio',
            'lumi': 50.79
        }
    ]
}

# Stop directory association with file call
# https://root.cern.ch/doc/master/classTH1.html#a490b9d2cf9460f8428f104d7e0f82614
ROOT.TH1.AddDirectory(False);

def GetHist(arg):
  tfile = ROOT.TFile.Open( os.environ["CMSSW_BASE"] + arg['filename'])
  obj   = tfile.Get(arg['fileobj']).Clone()
  return obj

def GetTotalLumi(arg):
    ans = 0
    for obj in mergelist[arg]:
        ans = ans + obj['lumi']
    return ans

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument('-m','--merge',type=str,help='type of object to merge')
  parser.add_argument('-o','--output',type=str,default='test.root',help='output root file name')
  parser.add_argument('-n','--name',type=str,help='store object name')

  arg = parser.parse_args()

  if not arg.merge :
      print "Merging object not specified!"
      print "Available:"
      for key in mergelist:
          print key
      return
  if arg.merge not in mergelist:
      print "Merge setting not Available!"
      print "Available"
      for key in mergelist:
          print key
      return

  if not arg.name:
      arg.name = arg.merge

  totallumi = GetTotalLumi(arg.merge)

  mergehist = GetHist( mergelist[arg.merge][0] )
  mergehist.Reset()

  for obj in mergelist[arg.merge]:
      hist = ROOT.TH2F( GetHist( obj ) )
      hist.Scale( obj['lumi'] / totallumi )
      mergehist.Add( hist )

  tfile = ROOT.TFile.Open(arg.output,'UPDATE')
  mergehist.Write(arg.name)
  tfile.Close()


if __name__ == '__main__':
  main()
