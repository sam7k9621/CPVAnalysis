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
        self.optionlst.append( full ) 
        return self

    def AddFlag( self, abbr, full ):
        self.parser.add_argument( "-" + abbr, "--" + full, action='store_true' )
        self.optionlst.append( full ) 
        return self

    def Parsing( self, output="Simulation", input="Hist" ):
        try:
            self.opt = self.parser.parse_args(sys.argv[1:])
        except:
            print "Error processing arguments!"
            self.parser.print_help()
            raise
        
        self.inputname  = "{}_{}".format( input, self.opt.lepton  ) + "_{}"
        self.outputname = "{}_{}".format( output, self.opt.lepton ) + "_{}"
        for option in self.optionlst :
            arg = getattr( self.opt, option )
            if arg:
                if isinstance( arg, bool ):
                    self.inputname  += "_" + option 
                    self.outputname += "_" + option
                else:
                    self.inputname  += "_" + option + "_" + arg 
                    self.outputname += "_" + option + "_" + arg
    
    # /*******************************************************************************
    # *   Get file name
    # *******************************************************************************/
    def GetFileName( self, sample, type="root", dir="results" ):
        file = self.inputname.format( sample )
        file = "{}/{}.{}".format(dir, file, type)
        print ">> Processing root file : {}".format( file )
        return file

    def GetResultName( self, topic, type="pdf", dir="results"):
        file = self.outputname.format( topic )
        return "{}/{}.{}".format( dir, file, type )
    
    # /*******************************************************************************
    # *   Default settings for lumi and entry
    # /*******************************************************************************
    def LeptonType( self ):
        return self.opt.lepton

    def Entry( self ):
        lep  = "e" if self.opt.lepton is "el" else "#mu"
        try :
            bjet = "2"
            if opt.region:
                bjet = "0"
        except:
            pass
        return "1 {}, #geq 4 jets ( {} b jets )".format( lep, bjet )

    def Lumi( self ):
        lumi = 35700. if self.opt.lepton == "el" else 35900.
        return lumi

    
