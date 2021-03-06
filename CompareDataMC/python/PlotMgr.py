from ROOT import TFile
from collections import deque
from ManagerUtils.PlotUtils.PlotUtils import *

class Plotmgr:
    def __init__( self ):
        self.sampledict = dict() 
    
    # /*******************************************************************************
    # *   Getting object out of root file
    # *******************************************************************************/
    def SetObjlst( self, filename, objnamelst, sample ):
        objlst=deque()
        file = TFile.Open( filename, 'read' )
        print ">> Processing file: {}".format( filename )
        for objname in objnamelst:
            obj = file.Get( objname )
            obj.SetDirectory(0)
            objlst.append( obj )
        file.Close()

        self.sampledict[ sample ] = objlst

    def GetObj( self, sample ):
        for s, objlst in self.sampledict.iteritems():
            if sample in s:
                return objlst.popleft()

    def RemoveObj( self, sample ):
        samplelst = [ x for x in self.sampledict if sample in x ]
        for s in samplelst:
            del self.sampledict[ s ]

    def GetMergedObj( self, sample ):        
        histlst=[]
        for s, objlst in self.sampledict.iteritems():
            if sample in s:
                histlst.append( objlst.popleft() )

        hist = histlst.pop()
        for h in histlst:
            hist.Add(h)
        return hist
