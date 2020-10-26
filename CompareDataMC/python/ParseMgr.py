import sys
import os
import argparse
from ManagerUtils.Common.progressbar import *

class Parsemgr:
    def __init__( self, package="CPVAnalysis", subpackage="CompareDataMC" ):
        self.base =  "{}/{}".format( os.environ['CMSSW_BASE'], "src" )
        self.package = package
        self.subpackage = subpackage
        self.parser = argparse.ArgumentParser("Plotmgr")
        self.parser.add_argument('-l', '--lepton',type=str,required=True)
        self.parser.add_argument('-y', '--year',type=str,required=True)
        self.parser.add_argument('-s', '--sample',type=str)
        self.inputoptionlst  = []
        self.outputoptionlst = []
    # /*******************************************************************************
    # *   Initialize parsing
    # *******************************************************************************/
    def AddInput( self, abbr, full ):
        self.parser.add_argument( "-" + abbr, "--" + full, type=str )     
        return self

    def AddFlag( self, abbr, full ):
        self.parser.add_argument( "-" + abbr, "--" + full, action='store_true' )
        return self

    def AddInputName( self, *args ):
        for arg in args:
            self.inputoptionlst.append( arg )

    def RemoveInputName( self, *args ):
        for arg in args:
            self.inputoptionlst.remove( arg )

    def AddOutputName( self, *args ):
        for arg in args:
            self.outputoptionlst.append( arg )

    def RemoveOutputName( self, *args ):
        for arg in args:
            self.outputoptionlst.remove( arg )
    
    def Parsing( self, input="Hist" ):
        try:
            self.opt = self.parser.parse_args(sys.argv[1:])
        except:
            print "Error processing arguments!"
            self.parser.print_help()
            raise
    
    def GetOption( self, option, add=False ):
        try:
            arg = getattr( self.opt, option )
        except AttributeError:
            print "Error processing arguments!"
            raise
        if arg and add:
            if type( arg ) == bool:
                return "_" + option
            else:
                return "_" + option + "_" + arg
        elif arg and not add:
            return arg 
        else:
            return ""

    def PbarStart( self,nevt ):
        widgets = [ Percentage(), ' ', Bar(), ' ', Timer(), ' ', AdaptiveETA(), ' ' ]
        self.pbar = ProgressBar(widgets=widgets, maxval=nevt).start()
        return self.pbar

    # /*******************************************************************************
    # *   Get file name
    # *******************************************************************************/
    def SubpackagePath( self ):
        return "{}/{}/{}".format( self.base, self.package, self.subpackage )

    def GetInputName( self, sample, prefix="Hist", type="root", dir="results" ):
        lepton = "{L}" if self.LeptonType() == "co" else self.LeptonType()
        year   = "{Y}" if self.Year() == "RunII" else self.Year()
        self.inputname  = "{}_{}_{}_{}".format( prefix, year, lepton, sample )
        for option in self.inputoptionlst :
            arg = getattr( self.opt, option )
            if arg:
                if isinstance( arg, bool ):
                    self.inputname  += "_" + option 
                else:
                    self.inputname  += "_" + option + "_" + arg 
        
        file = "{}/{}/{}.{}".format( self.SubpackagePath(), dir, self.inputname, type)
        return file

    def GetOutputName( self, output, prefix="Stack", type="pdf", dir="results"):
        self.outputname = "{}_{}_{}_{}".format( prefix, self.opt.year, self.opt.lepton, output ) 
        for option in self.outputoptionlst :
            arg = getattr( self.opt, option )
            if arg:
                if isinstance( arg, bool ):
                    self.outputname += "_" + option
                else:
                    self.outputname += "_" + option + "_" + arg
        
        return "{}/{}/{}.{}".format( self.SubpackagePath(), dir, self.outputname, type )

    # /*******************************************************************************
    # *   Default settings for lumi and entry
    # /*******************************************************************************
    def Year( self ):
        return self.opt.year 

    def LeptonType( self ):
        return self.opt.lepton

    def SampleInfo( self ):
        return self.opt.sample

    def Entry( self ):
        if self.opt.lepton == "el":
            lep = "e"
        elif self.opt.lepton == "mu":
            lep = "#mu"
        else:
            lep = "lep"

        try :
            bjet = "2"
            if self.opt.region:
                bjet = "0"
        except:
            pass
        return "1 {}, #geq 4 jets ( {} b jets )".format( lep, bjet )

    def Lumi( self ):
        if self.opt.year == "16":
            return 35900
        elif self.opt.year == "17":
            return 41540
        elif self.opt.year == "18":
            return 59690
        else:
            return 35900 + 41540 + 59690

    
