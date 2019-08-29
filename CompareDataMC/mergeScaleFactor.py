#!/bin/env python
#*************************************************************************
#
#  Filename    : mergeScaleFactor.py
#  Description : Merging Scale factor from different objects and
#  Author      : Yi-Mu "Enoch" Chen [ ensc@hep1.phys.ntu.edu.tw ]
#
#*************************************************************************
import argparse

import ROOT

mergelist = {
    'muid': [
        {
            'filename': 'RunBCDEF_SF_ID.root',
            'fileobj': 'NUM_TightID_DEN_genTracks_eta_pt',
            'lumi': 19.7
        },
        {
            'filename': 'RunGH_SF_ID.root',
            'fileobj': 'NUM_TightID_DEN_genTracks_eta_pt',
            'lumi': 16.2
        }
    ],
    'muiso': [
        {
            'filename': 'RunBCDEF_SF_ISO.root',
            'fileobj': 'NUM_TightRelIso_DEN_TightIDandIPCut_eta_pt',
            'lumi': 19.7
        },
        {
            'filename': 'RunGH_SF_ISO.root',
            'fileobj': 'NUM_TightRelIso_DEN_TightIDandIPCut_eta_pt',
            'lumi': 16.2
        }
    ]
#     'mutrig':[
        # {
            # 'filename': 'CPVAnalysis/CompareDataMC/data/TrgSF_BtoF.root',
            # 'fileobj': 'IsoMu24_OR_IsoTkMu24_PtEtaBins/abseta_pt_ratio',
            # 'lumi': 20.1
        # },
        # {
            # 'filename': 'CPVAnalysis/CompareDataMC/data/TrgSF_GH.root',
            # 'fileobj': 'IsoMu24_OR_IsoTkMu24_PtEtaBins/abseta_pt_ratio',
            # 'lumi': 16.3
        # }
    # ]
}

# Stop directory association with file call
# https://root.cern.ch/doc/master/classTH1.html#a490b9d2cf9460f8428f104d7e0f82614
ROOT.TH1.AddDirectory(False);

def GetHist(arg):
  tfile = ROOT.TFile.Open( "data/" + arg['filename'] )
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
      hist = GetHist( obj )
      hist.Scale( obj['lumi'] / totallumi )
      mergehist.Add( hist )

  tfile = ROOT.TFile.Open( "data/" + arg.output + ".root",'UPDATE')
  mergehist.Write(arg.name)
  tfile.Close()


if __name__ == '__main__':
  main()
