#!/bin/env python
import sys
import itertools
import argparse
import CPVAnalysis.CompareDataMC.pluginObservable as OBS
from ROOT import TChain, TLorentzVector, TH1D, TFile
from math import sqrt

class InvalidIndex(Exception): pass
class Particle:
    @classmethod
    def GetTLV( cls, idx ):
        t = TLorentzVector()
        t.SetPtEtaPhiM( cls.pt[idx], cls.eta[idx], cls.phi[idx], cls.mass[idx] )
        return t

def TLVtoTV3( tlv ):
    vect = tlv.Vect()
    return OBS.TVector3( vect.X(), vect.Y(), vect.Z() ) 

def GetParton( size, id, moid ):
    for s in range(size):
        if int(idlst[s]) in id and ( int(m1lst[s]) in moid  or int(m2lst[s]) in moid ):
            return s
    return -1


def main(args):
    parser = argparse.ArgumentParser()
    parser.add_argument('-g','--dtg',type=str)
    parser.add_argument('-l','--lepton',type=str)
    try:
        opt = parser.parse_args(args[1:])
    except:
        print "Error processing arguments!"
        parser.print_help()
        raise
    obs3  = TH1D( 'obs3',  '', 50, -1, 1 )
    obs6  = TH1D( 'obs6',  '', 50, -1, 1 )
    obs12 = TH1D( 'obs12', '', 50, -1, 1 )
    obs14 = TH1D( 'obs14', '', 50, -1, 1 )
    
    val_obs3  = TH1D( 'val_obs3',  '', 2, -1, 1 )
    val_obs6  = TH1D( 'val_obs6',  '', 2, -1, 1 )
    val_obs12 = TH1D( 'val_obs12', '', 2, -1, 1 )
    val_obs14 = TH1D( 'val_obs14', '', 2, -1, 1 )
   
    # pos  = "/wk_cms2/youying/public/bprimekit_2017_ReMiniAODv2/TT_CEDM_dtG5-MadGraph5-pythia8/bpk_ntuple_[0-9].root"
    pos  = "/wk_cms2/sam7k9621/CMSSW_10_2_17/src/CPVAnalysis/BaseLineSelector/results/FullCut_17_*_TT_CEDM_dtG{}_[0-9].root".format( opt.dtg )
    # ch   = TChain( "bprimeKit/root" )
    ch   = TChain( "root" )
    ch.Add( pos )
    print ">> Processing " + pos 
    
    global idlst, m1lst, m2lst
    
    for idx, entry in enumerate( ch ):
        if getattr( entry, "chi2mass") > 20:
            continue
        if getattr( entry, "lep_tmass" ) > 150:
            continue
        
        Particle.pt   = getattr( entry, "GenInfo.Pt" )
        Particle.eta  = getattr( entry, "GenInfo.Eta" )
        Particle.phi  = getattr( entry, "GenInfo.Phi" )
        Particle.mass = getattr( entry, "GenInfo.Mass" )

        size  = getattr( entry, "GenInfo.Size" )
        idlst = getattr( entry, "GenInfo.PdgID" )
        m1lst = getattr( entry, "GenInfo.Mo1PdgID" )
        m2lst = getattr( entry, "GenInfo.Mo2PdgID" )

        b    = GetParton( size, [5], [6] ) 
        bbar = GetParton( size, [-5], [-6] )
        jet1 = GetParton( size, [1, -1, 3, -3], [24, -24] )
        jet2 = GetParton( size, [2, -2, 4, -4], [24, -24] )
        lep  = GetParton( size, [11,-11,13,-13], [24, -24] )
       
        if -1 in [b, bbar, jet1, jet2, lep]:
            continue

        charge  = -1. if idlst[lep] > 0 else +1.
        b    = Particle.GetTLV( b )
        bbar = Particle.GetTLV( bbar )
        lep  = Particle.GetTLV( lep )
        jet1 = Particle.GetTLV( jet1 )
        jet2 = Particle.GetTLV( jet2 )
        hardjet = jet1 if abs( jet1.Pt() ) > abs( jet2.Pt() ) else jet2

        vec_b       = TLVtoTV3( b       ) 
        vec_bbar    = TLVtoTV3( bbar    ) 
        vec_lep     = TLVtoTV3( lep     ) 
        vec_hardjet = TLVtoTV3( hardjet ) 
        
        o6  = OBS.Obs6 ( vec_lep, vec_hardjet, vec_b, vec_bbar, charge )
        o12 = OBS.Obs12( vec_b, vec_bbar )
        o14 = OBS.Obs14( vec_lep, vec_hardjet, vec_b, vec_bbar )
        
        # In bbar CM frame
        bbCM = -( b + bbar ).BoostVector()
        b.Boost( bbCM )
        bbar.Boost( bbCM )
        lep.Boost( bbCM )
        hardjet.Boost( bbCM )
    
        vec_b       = TLVtoTV3( b       ) 
        vec_bbar    = TLVtoTV3( bbar    ) 
        vec_lep     = TLVtoTV3( lep     ) 
        vec_hardjet = TLVtoTV3( hardjet ) 
    
        o3 = OBS.Obs3( vec_lep, vec_hardjet, vec_b, vec_bbar, charge )
    
        obs3.Fill ( o3 / (172.5**3) )
        obs6.Fill ( o6 / (172.5**3) )
        obs12.Fill( o12 / (172.5**3) )
        obs14.Fill( o14 / (172.5**3) )
        
        val_obs3.Fill ( 0.5 if o3  > 0 else -0.5 )
        val_obs6.Fill ( 0.5 if o6  > 0 else -0.5 )
        val_obs12.Fill( 0.5 if o12 > 0 else -0.5 )
        val_obs14.Fill( 0.5 if o14 > 0 else -0.5 )
    
    myfile = TFile( "{}_CEDM_dtg{}_genlevel.root".format( opt.lepton, opt.dtg ), 'RECREATE' )
    obs3.Write()
    obs6.Write()
    obs12.Write()
    obs14.Write()
    val_obs3.Write()
    val_obs6.Write()
    val_obs12.Write()
    val_obs14.Write()
    myfile.Close()

if __name__ == '__main__':
    main(sys.argv)
