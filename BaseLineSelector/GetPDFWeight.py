#!/bin/env python
import sys
import argparse
import ROOT
from DataFormats.FWLite import Events, Runs, Handle
import numpy as np
def main(args):

    parser = argparse.ArgumentParser("")

    parser.add_argument(
            '-t', '--test',
            help='testing command',
            action='store_true'
            )
    try:
        opt = parser.parse_args(args[1:])
    except:
        parser.print_help()
        raise

    run = Runs ('data/ttbar.root')
    handle = Handle('LHERunInfoProduct')
    label = ("externalLHEProducer")
    
    run.getByLabel (label, handle)
    myLHERunInfoProduct = handle.product()
   
    print myLHERunInfoProduct.heprup().PDFSUP.first

    with open( "data/PDF_info_ttbar.txt", "w+" ) as output:
        for lhe in myLHERunInfoProduct:
            output.write( lhe )

    return
    events = Events( 'data/ttbar.root' )
    handle = Handle('LHEEventProduct')
    label  = ("externalLHEProducer")
    
    for evt in events:
        lst1 = []
        lst2 = []
        evt.getByLabel( label, handle )
        lhehandle = handle.product()

        lhesize = lhehandle.weights().size()
        for i in range( lhesize ):
            id = lhehandle.weights().at( i ).id
            if 2000 <= int(id) < 2101:
                print id, lhehandle.weights().at( i ).wgt
                # lst1.append( lhehandle.weights().at( i ).wgt /  lhehandle.originalXWGTUP() )        
                # lst2.append( lhehandle.weights().at( i ).wgt /  lhehandle.originalXWGTUP() - 1 )        
            if int(id) == 2101:
                print "as1", id, lhehandle.weights().at( i ).wgt
            if int(id) == 2102:
                print "as2", id, lhehandle.weights().at( i ).wgt
        return
        # y1 = np.array( lst1 )
        # y2 = np.array( lst2 )
        # print np.sqrt(np.mean(y1**2))
        # print np.sqrt(np.sum(y2**2)) + 1

if __name__ == '__main__':
    main(sys.argv)
