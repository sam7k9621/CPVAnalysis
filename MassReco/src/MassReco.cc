#include "CPVAnalysis/MassReco/interface/MassReco.h"

using namespace std;
using namespace sel;

/*******************************************************************************
*   MassReco
*******************************************************************************/

/* Data */

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

/* MC */

extern bool passMCMuon(vector<int>& mu){
  
    //muon : muon, mother = w, grandmother = t


    int lepidx  = 0;
    int mlepidx = 0;
    int glepidx = 0;


    for(int i=0;i<SelMgr().lsize();i++){
        SelMgr().SetIndex(i);

        lepidx = SelMgr().getGenParticle();
       

        if(lepidx  > 0)
            mlepidx = SelMgr().getDirectMother(lepidx);
        else
            continue ;
        
        if(mlepidx > 0) 
            glepidx = SelMgr().getDirectMother(mlepidx);
        else
            continue ;



        if (    
                SelMgr().isXGenParticle(lepidx, 13) &&  
                SelMgr().isXGenParticle(mlepidx,24) &&
                SelMgr().isXGenParticle(glepidx,6)
           )
            
            mu.push_back(lepidx);
    
    }

    return mu.size() >= 1 ;
}

extern bool passMCJet(vector<int>& jet, vector<int>& bjet){
    

    //bjet : parton = b, mother = t
    //jet : mother = w, grandmother = t
    
    int jetidx  = 0;
    int mjetidx = 0;
    int gjetidx = 0;

    //cout<<"jet num "<<SelMgr().jsize()<<endl;
    for(int i=0;i<SelMgr().jsize();i++){
        SelMgr().SetIndex(i);

        //SelMgr().showJetInfo(i);
        jetidx = SelMgr().getGenParton();
        
        if(jetidx >= 0)
            mjetidx = SelMgr().getDirectMother(jetidx);
        else
            continue ;
        
        if(mjetidx >= 0) 
            gjetidx = SelMgr().getDirectMother(mjetidx) ;
        else
            continue ;
        
        if (
                SelMgr().isXGenParticle(jetidx, 5) &&
                SelMgr().isXGenParticle(mjetidx,6) 
           )
        {    
            bjet.push_back(jetidx);
            continue;
        }

        if(gjetidx < 0)
            continue;

        if (
                SelMgr().isXGenParticle(mjetidx, 24) &&
                SelMgr().isXGenParticle(gjetidx, 6)
           )
        {
            jet.push_back(jetidx);
        }
    }

    return ( bjet.size() >= 2 && jet.size() >= 2 ) ;
}

extern bool hasCommonT_lep(const int& bjetidx, const int& lepidx){
    
   return SelMgr().isCommonMo( bjetidx, SelMgr().getDirectMother(lepidx), 6 );
}

