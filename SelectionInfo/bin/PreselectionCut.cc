#include "TChain.h"
#include "Selection/SelectionInfo/interface/SelectionInfo.h"
#include "TChain.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]){



    TChain* chain = new TChain("bprimeKit/root");

    chain->Add(argv[1]);
    
    InitPreCut(chain, argv[2]);

    
}
