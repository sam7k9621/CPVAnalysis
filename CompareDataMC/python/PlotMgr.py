from ROOT import TFile
from collections import deque
from ManagerUtils.PlotUtils.PlotUtils import *

class Plotmgr:
    def __init__( self ):
        self.sampledict = dict() 
    
    # /*******************************************************************************
    # *   Getting object out of root file
    # *******************************************************************************/
    def SetObjlst( self, filename, objnamelst ):
        objlst=deque()
        file = TFile.Open( filename, 'read' )
        for objname in objnamelst:
            obj = file.Get( objname )
            obj.SetDirectory(0)
            objlst.append( obj )
        file.Close()

        self.sampledict[ filename ] = objlst

    def GetObj( self, sample ):
        for s, objlst in self.sampledict.iteritems():
            if sample in s:
                return objlst.popleft()

    def GetMergedObj( self, sample ):        
        
        histlst=[]
        for s, objlst in self.sampledict.iteritems():
            if sample in s:
                histlst.append( objlst.popleft() )

        hist = histlst.pop()
        for h in histlst:
            hist.Add(h)
        return hist
