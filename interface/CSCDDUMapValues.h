#ifndef _CSCDDUMAPVALUES_H
#define _CSCDDUMAPVALUES_H

#include <memory>
#include "FWCore/Framework/interface/SourceFactory.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/ESProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetupRecordIntervalFinder.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include <DataFormats/MuonDetId/interface/CSCDetId.h>
#include "CondFormats/CSCObjects/interface/CSCDDUMap.h"
#include "CondFormats/DataRecord/interface/CSCDDUMapRcd.h"

class CSCDDUMapValues: public edm::ESProducer, public edm::EventSetupRecordIntervalFinder  {
 public:
  CSCDDUMapValues(const edm::ParameterSet&);
  ~CSCDDUMapValues();
  
  void fillDDUMap();

  typedef const  CSCDDUMap * ReturnType;
  
  ReturnType produceDDUMap(const CSCDDUMapRcd&);
  
 private:
  // ----------member data ---------------------------
  void setIntervalFor(const edm::eventsetup::EventSetupRecordKey &, const edm::IOVSyncValue&, edm::ValidityInterval & );
  CSCDDUMap *mapobj ;

};

#endif