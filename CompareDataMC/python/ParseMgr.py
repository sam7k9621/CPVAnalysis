import sys
import argparse

class Parsemgr:
    def __init__( self ):
        self.parser = argparse.ArgumentParser("Plotmgr")
        self.parser.add_argument('-l', '--lepton',type=str,required=True)
        self.optionlst = []
    
    # /*******************************************************************************
    # *   Initialize parsing
    # *******************************************************************************/
    def AddInput( self, abbr, full ):
        self.parser.add_argument( "-" + abbr, "--" + full, type=str )     
        return self

    def AddFlag( self, abbr, full ):
        self.parser.add_argument( "-" + abbr, "--" + full, action='store_true' )
        return self

    def SetName( self, *args ):
        for arg in args:
            self.optionlst.append( arg )

    def Parsing( self, input="Hist" ):
        try:
            self.opt = self.parser.parse_args(sys.argv[1:])
        except:
            print "Error processing arguments!"
            self.parser.print_help()
            raise
        
        self.inputname  = "{}_{}".format( input, self.opt.lepton  ) + "_{}"
        for option in self.optionlst :
            arg = getattr( self.opt, option )
            if arg:
                if isinstance( arg, bool ):
                    self.inputname  += "_" + option 
                else:
                    self.inputname  += "_" + option + "_" + arg 
    
    def GetOption( self, option ):
        try:
            arg = getattr( self.opt, option )
        except AttributeError:
            print "Error processing arguments!"
            raise
        if arg:
            return arg

    # /*******************************************************************************
    # *   Get file name
    # *******************************************************************************/
    def GetFileName( self, sample, type="root", dir="results" ):
        file = self.inputname.format( sample )
        file = "{}/{}.{}".format(dir, file, type)
        return file

    def GetResultName( self, topic, output="Stack", type="pdf", dir="results"):
        self.outputname = "{}_{}".format( output, self.opt.lepton ) + "_{}"
        for option in self.optionlst :
            arg = getattr( self.opt, option )
            if arg:
                if isinstance( arg, bool ):
                    self.outputname += "_" + option
                else:
                    self.outputname += "_" + option + "_" + arg
        
        file = self.outputname.format( topic )
        return "{}/{}.{}".format( dir, file, type )

    # /*******************************************************************************
    # *   Default settings for lumi and entry
    # /*******************************************************************************
    def LeptonType( self ):
        return self.opt.lepton

    def Entry( self ):
        lep  = "e" if self.opt.lepton == "el" else "#mu"
        try :
            bjet = "2"
            if self.opt.region:
                bjet = "0"
        except:
            pass
        return "1 {}, #geq 4 jets ( {} b jets )".format( lep, bjet )

    def Lumi( self ):
        lumi = 35700. if self.opt.lepton == "el" else 35900.
        return lumi

    
