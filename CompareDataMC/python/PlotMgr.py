from ROOT import TFile
from collections import deque
from ManagerUtils.PlotUtils.PlotUtils import *

class Plotmgr:
    def __init__( self ):
        self.sampledict = dict() 
    
    # /*******************************************************************************
    # *   Getting object out of root file
    # *******************************************************************************/
    def SetObjlst( self, filename, objnamelst, sample, dir="" ):
        objlst=deque()
        file = TFile.Open( filename, 'read' )
        print ">> Adding file: \033[0;31m{}\033[0m as {}".format( filename, sample )
        for objname in objnamelst:
            temp = dir + "/" + objname if dir else objname
            obj = file.Get( temp )
            try:
                obj.SetDirectory(0)
            except:
                print "{} is not defined in {}".format( objname, filename )
                raise
            if obj.GetEntries() == 0:
                print "{} in {} is empty".format( objname, filename )
            objlst.append( obj )
        file.Close()

        self.sampledict[ sample ] = objlst

    def GetObj( self, sample ):
        for s, objlst in self.sampledict.iteritems():
            if sample == s:
                return objlst.popleft()

    def GetSamplelst( self ):
        for s in self.sampledict:
            print s
   
    def RemoveObj( self, *sample ):
        samplelst = [ x for x in self.sampledict if all( y in x for y in sample ) ]
        
        for s in samplelst:
            print ">> Removing sample: \033[0;31m{}\033[0m".format( s )
            del self.sampledict[ s ]

    def GetMergedObj( self, *sample ):        
        histlst=[]
        for s, objlst in self.sampledict.iteritems():
            if all( sam in s for sam in sample ):
                histlst.append( objlst.popleft() )
        hist = histlst.pop()
        for h in histlst:
            hist.Add(h)
        return hist

    def CustomTitle( self, hist ):
        ytitle = hist.GetYaxis().GetTitle()
        try:
            t, p = ytitle.split( "(" )
        except:
            return 
        title = t.split()
        title.insert( 1, p.strip(")") )
        hist.GetYaxis().SetTitle( " ".join( title ) )

