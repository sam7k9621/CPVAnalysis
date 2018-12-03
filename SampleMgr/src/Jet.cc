#include "CPVAnalysis/SampleMgr/interface/SampleMgr.h"
#include <iostream>
#include <random>
using namespace std;

namespace mgr{
    /*******************************************************************************
    * Jet info
    *******************************************************************************/
    float 
    SampleMgr::JesUnc()
    {
        return _jet.Unc[ _idx ];
    }
    
    float
    SampleMgr::JERPt()
    {
        return _jet.JERPt[ _idx ];
    }

    float
    SampleMgr::JERScale()
    {
        return _jet.JERScale[ _idx ];
    }

    float
    SampleMgr::JetPt()
    {
        return _jet.Pt[ _idx ];
    }

    float
    SampleMgr::JetAbsEta()
    {
        return fabs( _jet.Eta[ _idx ] );
    }

    float
    SampleMgr::JetEta()
    {
        return _jet.Eta[ _idx ];
    }

    float
    SampleMgr::JetPhi()
    {
        return _jet.Phi[ _idx ];
    }

    float
    SampleMgr::GenJetPt()
    {
        return _jet.GenJetPt[ _idx ];
    }

    float
    SampleMgr::GenJetEta()
    {
        return _jet.GenJetEta[ _idx ];
    }

    float
    SampleMgr::GenJetPhi()
    {
        return _jet.GenJetPhi[ _idx ];
    }

    int
    SampleMgr::JetNConstituents()
    {
        return _jet.NConstituents[ _idx ];
    }

    float
    SampleMgr::JetNEF()
    {
        return _jet.NEF[ _idx ];
    }

    float
    SampleMgr::JetNHF()
    {
        return _jet.NHF[ _idx ];
    }

    float
    SampleMgr::JetCHF()
    {
        return _jet.CHF[ _idx ];
    }

    int
    SampleMgr::JetNCH()
    {
        return _jet.NCH[ _idx ];
    }

    float
    SampleMgr::JetCEF()
    {
        return _jet.CEF[ _idx ];
    }

    TLorentzVector
    SampleMgr::GetJetP4( const int& i )
    {
        return TLorentzVector( _jet.Px[ i ], _jet.Py[ i ], _jet.Pz[ i ], _jet.Energy[ i ] );
    }

    // Jet smeared
    void
    SampleMgr::SetJetPtEta( const float& pt, const float& eta )
    {
        _jet.Pt[ _idx ]  = pt;
        _jet.Eta[ _idx ] = eta;
    }

    // b-tagging
    float
    SampleMgr::JetCSV()
    {
        return _jet.pfCombinedInclusiveSecondaryVertexV2BJetTags[ _idx ];
    }
}
