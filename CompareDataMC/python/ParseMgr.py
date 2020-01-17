import sys
import os
import argparse

class Parsemgr:
    def __init__( self ):
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
            return "_" + option + "_" + arg
        elif arg and not add:
            return arg 
        else:
            return ""

    # /*******************************************************************************
    # *   Get file name
    # *******************************************************************************/
    def GetInputName( self, sample, prefix="Hist", type="root", dir="results" ):
        
        self.inputname  = "{}_{}_{}_{}".format( prefix, self.opt.year, self.opt.lepton, sample )
        for option in self.inputoptionlst :
            arg = getattr( self.opt, option )
            if arg:
                if isinstance( arg, bool ):
                    self.inputname  += "_" + option 
                else:
                    self.inputname  += "_" + option + "_" + arg 
        
        file = "{}/{}.{}".format(dir, self.inputname, type)
        
        if self.opt.lepton == "co":
            haddfile = file.replace( self.opt.lepton, "*" )
            os.system( "hadd -f {} {}".format( file, haddfile ) )
        
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
        
        return "{}/{}.{}".format( dir, self.outputname, type )

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
        lep  = "e" if self.opt.lepton == "el" else "#mu"
        try :
            bjet = "2"
            if self.opt.region == "WJets":
                bjet = "0"

            if self.opt.region == "QCD":
                bjet = "< 2"
                if getattr( self.opt, "0bjet" ):
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
            return 0

    
