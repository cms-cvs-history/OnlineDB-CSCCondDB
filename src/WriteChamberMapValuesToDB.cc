// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"


#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"

// DB includes
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CondCore/DBOutputService/interface/PoolDBOutputService.h"

// user include files
#include "CondFormats/CSCObjects/interface/CSCChamberMap.h"
#include "CondFormats/DataRecord/interface/CSCChamberMapRcd.h"
//For Checks
#include <DataFormats/MuonDetId/interface/CSCDetId.h>
#include "FWCore/MessageLogger/interface/MessageLogger.h"

//this one
#include "OnlineDB/CSCCondDB/interface/WriteChamberMapValuesToDB.h"

//
// static data member definitions
//

//
// constructors and destructor
//
WriteChamberMapValuesToDB::WriteChamberMapValuesToDB(const edm::ParameterSet& iConfig)
{
  //now do what ever initialization is needed
  newTagRequest_ = iConfig.getParameter< std::string > ("NewTagRequest");
}


WriteChamberMapValuesToDB::~WriteChamberMapValuesToDB()
{
  
  // do anything here that needs to be done at desctruction time
  // (e.g. close files, deallocate resources etc.)
  
}


//
// member functions
//

// ------------ method called to for each event  ------------
void WriteChamberMapValuesToDB::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  using namespace edm;
  // Intercalib constants
  edm::ESHandle<CSCChamberMap> ch_map;
  iSetup.get<CSCChamberMapRcd>().get(ch_map);
  const CSCChamberMap* mychamberMap = ch_map.product();
  
  edm::Service<cond::service::PoolDBOutputService> poolDbService;
  if(poolDbService.isAvailable() ){
    if (poolDbService->isNewTagRequest(newTagRequest_) ){
      std::cout<<" Creating a  new one "<<std::endl;
      poolDbService->createNewIOV<const CSCChamberMap>(mychamberMap, poolDbService->endOfTime(),newTagRequest_);
      std::cout<<"Done" << std::endl;
    }else{
      std::cout<<"Old One "<<std::endl;
      poolDbService->appendSinceTime<const CSCChamberMap>(mychamberMap, poolDbService->currentTime(),newTagRequest_);
    }
  }  
}


// ------------ method called once each job just before starting event loop  ------------
void WriteChamberMapValuesToDB::beginJob(const edm::EventSetup&)
{
}

// ------------ method called once each job just after ending the event loop  ------------
void WriteChamberMapValuesToDB::endJob() {
  std::cout << "Here is the end" << std::endl; 
}
