#ifndef SELECTOR_H
#define SELECTOR_H

#include "ManagerUtils/ArgHelper/interface/Common.hpp"
#include "ManagerUtils/ArgHelper/interface/Parsermgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Pathmgr.hpp"
#include "ManagerUtils/ArgHelper/interface/Readmgr.hpp"

#include "CPVAnalysis/BaseLineSelector/interface/BaseLineMgr.h"

#include "TH1D.h"

typedef vector<shared_ptr<BaseLineMgr> > Samplelst;

class Selector : public mgr::Pathmgr,
                 public mgr::Readmgr,
                 public mgr::Parsermgr {

    public:

        /*******************************************************************************
        *   Initialization
        *******************************************************************************/

        Selector( const std::string& );
        ~Selector();

        Selector( const Selector& )            = delete;
        Selector& operator=( const Selector& ) = delete;

        /*******************************************************************************
        *   Common
        *******************************************************************************/
        void         process( const int&, const int& );
        std::string  GetResultsName( const std::string&, const std::string& );
        void         ChangeFile(const std::string&);
        void         AddSample( const string&, TChain* );
        Samplelst    GetSamplelst(){ return _samplelst; }
        std::vector<TH1*>         GetHistlst(const std::string&);
        BaseLineMgr* GetSample()   { return _samplelst.back().get(); }
        std::string  Discript( TH1* );
    private:

        Samplelst _samplelst;
};

#endif
