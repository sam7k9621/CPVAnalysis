#ifndef SELECTOR_H
#define SELECTOR_H

#include "ManagerUtils/ArgHelper/interface/Common.hpp"
#include "ManagerUtils/ArgHelper/interface/Parsermgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Pathmgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Readmgr.hpp"

#include "CPVAnalysis/BaseLineSelector/interface/BaseLineMgr.h"

#include "TH1D.h"

class Selector : public mgr::Pathmgr,
                 public mgr::Readmgr,
                 public mgr::Parsermgr {

    public:

        /*******************************************************************************
        *   Initialization
        *******************************************************************************/

        Selector( const std::string&, const std::string& );
        ~Selector();

        Selector( const Selector& )            = delete;
        Selector& operator=( const Selector& ) = delete;

        void         AddSample( const string&, TChain* );
        BaseLineMgr* GetSample()   { return _sample; }
        
        /*******************************************************************************
        *   Common
        *******************************************************************************/
        std::string  GetResultsName( const std::string&, const std::string& );
        std::string  Discript( TH1* );
        
        /*******************************************************************************
        *   Looping event
        *******************************************************************************/
        void         process( const int&, const int& );
        void         ChangeFile(const std::string&);
        void         GetEntry(const int&);
        int          GetEntries(){ return GetSample()->GetEntries(); }
    private:

        BaseLineMgr* _sample;
};

#endif
