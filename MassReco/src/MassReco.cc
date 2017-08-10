#include "CPVAnalysis/MassReco/interface/MassReco.h"

using namespace std;
using namespace sel;

extern bool passFullMuon(vector<int>& muidx){
    
    for(int i=0;i<SelMgr().lsize();i++){
        SelMgr().SetIndex(i);
            
        if(SelMgr().lep_type() == 13){
            
            if(passMuTight()){
                muidx.push_back(i);
                continue;
            }
            
            if(passMuLoose()){
                return false;
            }
        }
         
        else if(SelMgr().lep_type() == 11){

            if( passElLoose() ){
                return false;
            }
        }
    }

    return muidx.size() > 0;
}

extern bool passFullJet(vector<int>& jetidx, vector<int>& bjetidx, const int& muidx){

        for(int j=0;j<SelMgr().jsize();j++){
            SelMgr().SetIndex(j);

            //Cleaning against leptons (isolated lepton)
            if( !isIsolated(muidx,j) )
                continue;

            int mask = 0x01;

            if(passJet()){
                mask <<= 1;
            }
            
            if(passBJet()){
                mask <<= 2;
            }
    
            if(mask & 0x02){
                jetidx.push_back(j);
            }

            if(mask & 0x08){
                bjetidx.push_back(j);
            }
        }

        return ( jetidx.size() >= 2 && bjetidx.size() == 2 );
}

