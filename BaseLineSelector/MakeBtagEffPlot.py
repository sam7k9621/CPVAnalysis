#!/bin/env python
import sys
import argparse
import numpy
from array import array
from ROOT import TFile, TChain, TH2D, TEfficiency 
from progressbar import *

def PassJetLooseID( entry, idx ):
    return  (
            getattr( entry, "JetInfo.NHF" )[ idx ] < 0.9 and 
            getattr( entry, "JetInfo.NEF" )[ idx ] < 0.9 and 
            getattr( entry, "JetInfo.NConstituents" )[ idx ] > 1 and 
        
            abs( getattr( entry, "JetInfo.Eta" )[ idx ] ) <= 2.4 and 
            getattr( entry, "JetInfo.CHF" )[ idx ] > 0 and 
            getattr( entry, "JetInfo.NCH" )[ idx ] > 0 
            )

def PassJetKinematic( entry, idx ):
    return ( 
            getattr( entry, "JetInfo.Pt" )[ idx ] > 30 and
            abs( getattr( entry, "JetInfo.Eta" )[ idx ] ) < 2.4
            )

def main(args):
    parser = argparse.ArgumentParser("")
    parser.add_argument(
            '-s', '--sample',
            help='testing command',
            type=str,
            required=True
            )
    parser.add_argument(
            '-d', '--discriminator',
            help='testing command',
            type=float,
            required=True
            )
    parser.add_argument(
            '-t', '--test',
            help='testing command',
            action='store_true'
            )
    try:
        opt = parser.parse_args(args[1:])
    except:
        parser.print_help()
        raise

    # pos  = "/afs/cern.ch/work/p/pusheng/CMSSW_9_4_13/src/PreCut_{}_[0-9]+.root";
    pos  = "/eos/cms/store/user/youying/public/2016/PreCut_{}*.root";
    ch   = TChain( "root" )
    ch.Add( pos.format( opt.sample ) )
    print ">> Processing " + pos.format( opt.sample )

    newfile = TFile( "beffPlot_{}_{}.root".format( opt.sample, str(opt.discriminator).replace(".","pt") ), 'RECREATE' )
    
    xbins = array( 'd', [0., 20., 30., 50., 70., 100., 140., 200., 300., 600., 1000.] )
    ybins = array( 'd', [-2.4, -2.0, -1.6, -1.2, -0.8, -0.4, 0., 0.4, 0.8, 1.2, 1.6, 2.0, 2.4] )
    eff_b = TEfficiency( "eff_b", "eff_b", len(xbins) - 1, xbins, len(ybins) - 1, ybins )
    eff_c = TEfficiency( "eff_c", "eff_c", len(xbins) - 1, xbins, len(ybins) - 1, ybins )
    eff_l = TEfficiency( "eff_l", "eff_l", len(xbins) - 1, xbins, len(ybins) - 1, ybins )
   
    if opt.test:
        ch = ch.CloneTree(10000)

    entries = ch.GetEntries()
    widgets = [ Percentage(), ' ', Bar(), ' ', Timer(), ' | ', AdaptiveETA() ]
    pbar    = ProgressBar( widgets=widgets, maxval=10*entries ).start()
    for idx, entry in enumerate( ch ):
        pbar.update(10 * idx + 1)
        jsize  = getattr( entry, "JetInfo.Size" )
        flavor = getattr( entry, "JetInfo.GenFlavor" )
        bb_csv = getattr( entry, "JetInfo.pfDeepCSVJetTags_probb" )
        bbb_csv= getattr( entry, "JetInfo.pfDeepCSVJetTags_probbb" )
        pt     = getattr( entry, "JetInfo.Pt" )
        eta    = getattr( entry, "JetInfo.Eta" )

        for i in range( jsize ):
            if not PassJetLooseID( entry, i ) or not PassJetKinematic( entry, i ): 
                continue
            
            if abs( flavor[i] ) == 5:
                eff_b.Fill( bb_csv[i] + bbb_csv[i] >= opt.discriminator, pt[i], eta[i] )
            elif abs( flavor[i] ) == 4:
                eff_c.Fill( bb_csv[i] + bbb_csv[i] >= opt.discriminator, pt[i], eta[i] )
            else:
                eff_l.Fill( bb_csv[i] + bbb_csv[i] >= opt.discriminator, pt[i], eta[i] )

    plot_b = eff_b.CreateHistogram()
    plot_c = eff_c.CreateHistogram()
    plot_l = eff_l.CreateHistogram()
    plot_b.SetOption("colz")
    plot_b.SetName( "eff_b" )
    plot_c.SetOption("colz")
    plot_c.SetName( "eff_c" )
    plot_l.SetOption("colz")
    plot_l.SetName( "eff_l" )

    plot_b.Write()
    plot_c.Write()
    plot_l.Write()
    newfile.Close()
           
if __name__ == '__main__':
    main(sys.argv)
