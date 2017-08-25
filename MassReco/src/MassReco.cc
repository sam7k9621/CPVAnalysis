#include "CPVAnalysis/MassReco/interface/MassReco.h"
#include <climits>

using namespace std;
using namespace sel;

/*******************************************************************************
*   MassReco
*******************************************************************************/
/* Data */

extern bool checkPU(){

    if( SelMgr().CheckOption("vertex") )
        return SelMgr().vsize() > SelMgr().GetOption<double>("vertex");
    else 
        return true;
}

extern bool passChi2Upper(const double& chi2, const double& upper){

    if( SelMgr().CheckOption("chi2") )
        return chi2 <= SelMgr().GetOption<double>("chi2");

    else if(upper != 0)
        return chi2 <= upper;
   
    else
        return true;
}

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

extern bool isIsolated(const int& muidx, const int& jetidx){

    return SelMgr().isIsoLepton(muidx,jetidx);
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

            else if(mask & 0x08){
                bjetidx.push_back(j);
            }
        }

        return ( jetidx.size() >= 2 && bjetidx.size() == 2 );
}

extern tuple<double,double,int> 
getChi2Info(const int& muidx, const vector<int>& jetidx, const vector<int>& bjetidx){

        //Get TLorentzVector from index
        TLorentzVector         muonhandle = SelMgr().getLorentzLep(muidx);
        vector<TLorentzVector> jethandle  = SelMgr().getLorentzJet(jetidx);
        vector<TLorentzVector> bjethandle = SelMgr().getLorentzJet(bjetidx);
        
        //Mass constrain method - find hadronic b 
        double chi2mass = INT_MAX;
        double seltmass = 0;
        int had_b = -1;

        for(unsigned int i=0;i<jethandle.size();i++){
            for(unsigned int j=(i+1);j<jethandle.size();j++){
                for(unsigned int k=0;k<bjethandle.size();k++){
    
                    double t_mass = (jethandle[i]+jethandle[j]+bjethandle[k]).M();
                    double w_mass = (jethandle[i]+jethandle[j]).M();
                    double chi_t  = (t_mass-172.5)/16.3;
                    double chi_w  = (w_mass-82.9 )/9.5;

                    if( (chi_t*chi_t + chi_w*chi_w) < chi2mass ){

                        chi2mass = (chi_t*chi_t + chi_w*chi_w);
                        seltmass = t_mass;
                        had_b    = k;
                    }
                }
            }
        }

        return make_tuple(chi2mass, seltmass, had_b);
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

