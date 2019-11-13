#!/bin/env python
import sys
import itertools
import argparse
import CPVAnalysis.CompareDataMC.PlotMgr as pltmgr
import CPVAnalysis.CompareDataMC.pluginObservable as OBS
from CPVAnalysis.CompareDataMC.Datacard import CEDM_dict
from ROOT import TChain, TLorentzVector, TVector3, TGraphErrors, TF1, kGreen
from progressbar import *
from array import array 
from math import sqrt

class InvalidIndex(Exception): pass
class Particle:
    @classmethod
    def GetTLV( cls, idx ):
        return TLorentzVector( cls.px[ idx ], cls.py[ idx ], cls.pz[ idx ], cls.energy[ idx ] )

def Error( p, f ):
    err = sqrt( p + f )
    return err

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

    raise InvalidIndex( "Following idlst not found {}".format( ','.join( str(x) for x in idlst) ) )

def MakePlot( x, y, ex, ey, func, obs ):
    gr = TGraphErrors( len(x), x, y, ex, ey )
    c = pltmgr.NewCanvas( obs ) 
    f = TF1( "f", func, 0, 5 )
    
    gr.Draw( "AEP" )
    f .Draw( "same" )

    f.SetLineColor( kGreen - 7 )
    f.SetLineStyle( 7 )
    gr.SetMarkerStyle( 20 )
    gr.SetTitle( obs )
    gr.GetXaxis().SetTitle( "d_{tG}" )
    gr.GetYaxis().SetTitle( "A_{CP}" )

    pltmgr.SetSinglePad( c )
    pltmgr.SetAxis( gr )
    c.SaveAs( "results/CEDM_Simulation_{}.pdf".format(obs) )

def main(args):
    parser = argparse.ArgumentParser()
    parser.add_argument('-d','--dirlst',type=str, nargs='+', default=["0_0"])
    try:
        opt = parser.parse_args(args[1:])
    except:
        print "Error processing arguments!"
        parser.print_help()
        raise

    x = array( 'f' )
    y3, ex3, ey3    = array( 'f' ), array( 'f' ),array( 'f' )
    y6, ex6, ey6    = array( 'f' ), array( 'f' ),array( 'f' )
    y12, ex12, ey12 = array( 'f' ), array( 'f' ),array( 'f' )
    y13, ex13, ey13 = array( 'f' ), array( 'f' ),array( 'f' )
    for i, d in enumerate( opt.dirlst ):
        pos  = "/eos/cms/store/user/pusheng/public/CEDM_ntuple/CEDM_dtg_{}/ntuple_*.root".format( d )
        ch   = TChain( "MC" )
        ch.Add( pos )
        print ">> Processing " + pos 

        x.append( i / 2.0 )
        NO3p, NO3n   = 0., 0.
        NO6p, NO6n   = 0., 0.
        NO12p, NO12n = 0., 0.
        NO13p, NO13n = 0., 0.
        total = 1000000

        widgets = [ Percentage(), ' ', Bar(), ' ', Timer(), ' | ', AdaptiveETA() ]
        pbar    = ProgressBar( widgets=widgets, maxval=10*total ).start()
        for idx, entry in enumerate( ch ):
            Particle.pdgid  = getattr( entry, "pdgid" )
            Particle.px     = getattr( entry, "px" )
            Particle.py     = getattr( entry, "py" )
            Particle.pz     = getattr( entry, "pz" )
            Particle.energy = getattr( entry, "energy" )
           
            try:
                b    = Particle.GetTLV( GetIdx( [ 5 ] ) )
                bbar = Particle.GetTLV( GetIdx( [ -5 ] ) )
                lep  = Particle.GetTLV( GetIdx( [ 13, -13 ] ) )
                neu  = Particle.GetTLV( GetIdx( [ 14, -14, 12, -12 ] ) )
                jet1 = Particle.GetTLV( GetIdx( [ 1, -1, 3, -3 ] ) )
                jet2 = Particle.GetTLV( GetIdx( [ 2, -2, 4, -4 ] ) )
                
                charge  = -1. if Particle.pdgid[ GetIdx( [ 13, -13 ] ) ] > 0 else 1. 
                hardjet = jet1 if abs( jet1.Pt() ) > abs( jet2.Pt() ) else jet2

            except Exception as II:
                print "In event {}, {}".format( idx, II )

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
            o13 = OBS.Obs13( vec_lep, vec_hardjet, vec_b, vec_bbar, charge )

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

            if o3 > 0.:
                NO3p += 1
            else:
                NO3n += 1
            if o6 > 0.:
                NO6p += 1 
            else: 
                NO6n += 1

            if o12 > 0.:
                NO12p += 1
            else:
                NO12n += 1

            if o13 > 0.:
                NO13p += 1
            else:
                NO13n += 1

            pbar.update( 10*(NO3p + NO3n) )
            if NO3p + NO3n == total:
                break
       
        xsec = CEDM_dict[ d ]
        y3 .append( ( xsec / 19.86648916 ) / ( NO3p  + NO3n )  * ( NO3p  - NO3n ) )
        y6 .append( ( xsec / 19.86648916 ) / ( NO6p  + NO6n )  * ( NO6p  - NO6n ) )
        y12.append( ( xsec / 19.86648916 ) / ( NO12p + NO12n ) * ( NO12p - NO12n ) )
        y13.append( ( xsec / 19.86648916 ) / ( NO13p + NO13n ) * ( NO13p - NO13n ) )

        ey3 .append( ( xsec / 19.86648916 ) / ( NO3p  + NO3n )  * Error( NO3p , NO3n ) )
        ey6 .append( ( xsec / 19.86648916 ) / ( NO6p  + NO6n )  * Error( NO6p , NO6n ) )
        ey12.append( ( xsec / 19.86648916 ) / ( NO12p + NO12n ) * Error( NO12p, NO12n ) )
        ey13.append( ( xsec / 19.86648916 ) / ( NO13p + NO13n ) * Error( NO13p, NO13n ) )
        
        ex3 .append( 0 )
        ex6 .append( 0 )
        ex12.append( 0 )
        ex13.append( 0 )

    MakePlot( x, y3, ex3, ey3, "-0.0148*x", "Obs_{3}" )
    MakePlot( x, y6, ex6, ey6, "0.0095*x", "Obs_{6}" )
    MakePlot( x, y12, ex12, ey12, "0.0018*x", "Obs_{12}" )
    MakePlot( x, y13, ex13, ey13, "0.0*x" ,"Obs_{13}" )

if __name__ == '__main__':
    main(sys.argv)
