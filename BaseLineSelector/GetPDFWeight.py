#!/bin/env python
import sys
import argparse
import ROOT
from DataFormats.FWLite import Events, Runs, Handle

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
    handle  = Handle ('LHERunInfoProduct')
    label = ("externalLHEProducer")
    
    run.getByLabel (label, handle)
    myLHERunInfoProduct = handle.product()
    
    with open('data/PDF_info_ttbar.txt', 'a') as output:
        for lhe in myLHERunInfoProduct:
            output.write( lhe )


if __name__ == '__main__':
    main(sys.argv)
