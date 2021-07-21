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
        return TLorentzVector( cls.px[ idx ], cls.py[ idx ], cls.pz[ idx ], cls.energy[ idx ] )

def TLVtoTV3( tlv ):
    vect = tlv.Vect()
    return OBS.TVector3( vect.X(), vect.Y(), vect.Z() ) 

def GetIdx( idlst ):
    for idx, id in enumerate( list( Particle.pdgid ) ):  
        # To avoid qqbar production
        if idx < 2:
            continue 
        if id in idlst:
            return idx

    raise InvalidIndex()

def main(args):
    parser = argparse.ArgumentParser()
    parser.add_argument('-d','--dir',type=str)
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
    
    # pos  = "/wk_cms2/sam7k9621/CEDM_sample/{}/*.root".format( opt.dir )
    pos  = "/wk_cms2/cychuang/MG5/mg_root/10_7/{}/ntuple_*.root".format( opt.dir )
    # pos  = "/wk_cms2/sam7k9621/CEDM_ntuple/CEDM_dtg_{}/ntuple_*.root".format( d )
    ch   = TChain( "MGtree" )
    # ch   = TChain( "MC" )
    ch.Add( pos )
    print ">> Processing " + pos 
    # lepton = 11 if opt.lepton == "el" else 13
    for idx, entry in enumerate( ch ):
        Particle.pdgid  = getattr( entry, "GenEvent.PdgID" )
        Particle.px     = getattr( entry, "GenEvent.Px" )
        Particle.py     = getattr( entry, "GenEvent.Py" )
        Particle.pz     = getattr( entry, "GenEvent.Pz" )
        Particle.energy = getattr( entry, "GenEvent.E" )
        try:
            b    = Particle.GetTLV( GetIdx( [ 5 ] ) )
            bbar = Particle.GetTLV( GetIdx( [ -5 ] ) )
            lep  = Particle.GetTLV( GetIdx( [ 11, -11, 13, -13 ] ) )
            # lep  = Particle.GetTLV( GetIdx( [ lepton, -lepton ] ) )
            neu  = Particle.GetTLV( GetIdx( [ 14, -14, 12, -12 ] ) )
            jet1 = Particle.GetTLV( GetIdx( [ 1, -1, 3, -3 ] ) )
            jet2 = Particle.GetTLV( GetIdx( [ 2, -2, 4, -4 ] ) )
            
            # charge  = -1. if Particle.pdgid[GetIdx( [ lepton, -lepton ] )] > 0 else 1. 
            charge  = -1. if Particle.pdgid[GetIdx( [ 11, -11, 13, -13 ] )] > 0 else 1. 
            hardjet = jet1 if abs( jet1.Pt() ) > abs( jet2.Pt() ) else jet2
    
        except InvalidIndex :
            continue
    
        if not abs( lep. Pt() ) > 20 or \
           not abs( jet1.Pt() ) > 20 or \
           not abs( jet2.Pt() ) > 20 or \
           not abs( neu. Pt() ) > 30 or \
           not abs( b.   Pt() ) > 25 or \
           not abs( bbar.Pt() ) > 25 :
                continue
       
        if not abs( lep. Eta() ) < 2.5 or \
           not abs( jet1.Eta() ) < 2.5 or \
           not abs( jet2.Eta() ) < 2.5 or \
           not abs( b.   Eta() ) < 2.5 or \
           not abs( bbar.Eta() ) < 2.5 :
                continue
        
        comb =  list(itertools.combinations([ b, bbar, lep, neu, jet1, jet2 ], 2))
        if any( l1.DeltaR( l2 ) <= 0.4 for l1, l2 in comb ):
            continue
    
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
    
    myfile = TFile( '{}_{}.root'.format( opt.lepton, opt.dir ), 'RECREATE' )
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
