// -*- C++ -*-
//
// Package:    CPVAnalysis/CheckGenJet
// Class:      CheckGenJet
//
/**\class CheckGenJet CheckGenJet.cc CPVAnalysis/CheckGenJet/plugins/CheckGenJet.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Sam_Chen Pu Health_ b02202044_0927376243
//         Created:  Tue, 25 Jul 2017 09:09:56 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/PatCandidates/interface/Jet.h"
#include <vector>
#include <iostream>
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class CheckGenJet : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
    public:
        explicit CheckGenJet( const edm::ParameterSet& );
        ~CheckGenJet();

        static void fillDescriptions( edm::ConfigurationDescriptions& descriptions );

        void getJetInfo( const pat::Jet& );

    private:
        virtual void beginJob() override;
        virtual void analyze( const edm::Event&, const edm::EventSetup& ) override;
        virtual void endJob() override;


        const edm::EDGetToken _jetsrc;
        edm::Handle<std::vector<pat::Jet> > _jethandle;

        // ----------member data ---------------------------
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
CheckGenJet::CheckGenJet( const edm::ParameterSet& iConfig ):
    _jetsrc( consumes<std::vector<pat::Jet> >( iConfig.getParameter<edm::InputTag>( "jetsrc" ) ) ) {
    //now do what ever initialization is needed
    usesResource( "TFileService" );
}


CheckGenJet::~CheckGenJet() {
    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)
}


//
// member functions
//

// ------------ method called for each event  ------------
void
CheckGenJet::analyze( const edm::Event& iEvent, const edm::EventSetup& iSetup ) {
    iEvent.getByToken( _jetsrc, _jethandle );

    for( const auto& jet : *_jethandle ) {
        getJetInfo( jet );
    }
}

void
CheckGenJet::getJetInfo( const pat::Jet& jet ) {
    using namespace std;

    if( jet.genJet() == NULL ) {
        cout << "empty" << endl;
    }

    else {
        cout << "pt  " << jet.genJet()->pt() << endl;
        cout << "eta " << jet.genJet()->eta() << endl;
        cout << "phi " << jet.genJet()->phi() << endl;
    }
}

// ------------ method called once each job just before starting event loop  ------------
void
CheckGenJet::beginJob() {
}

// ------------ method called once each job just after ending the event loop  ------------
void
CheckGenJet::endJob() {
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
CheckGenJet::fillDescriptions( edm::ConfigurationDescriptions& descriptions ) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault( desc );
}

//define this as a plug-in
DEFINE_FWK_MODULE( CheckGenJet );
