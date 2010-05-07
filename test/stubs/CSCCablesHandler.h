#ifndef CSC_CABLES_SRC_IMPL_H
#define CSC_CABLES_SRC_IMPL_H

#include <vector>
#include <string>
#include <iostream>
#include <typeinfo>

#include "CondCore/PopCon/interface/PopConSourceHandler.h"
#include "CondFormats/CSCObjects/interface/CSCobject.h"
#include "CondFormats/DataRecord/interface/CSCCablesRcd.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "OnlineDB/CSCCondDB/interface/CSCCableValues.h"
#include "OnlineDB/CSCCondDB/interface/CSCMap1.h"

namespace popcon
{
  class CSCCablesImpl : public popcon::PopConSourceHandler<CSCCables>
    {
      
    public:
      void getNewObjects();
      std::string id() const { return m_name;}
      ~CSCCablesImpl(); 
      CSCCablesImpl(const edm::ParameterSet& pset);
            
    private:
      std::string m_name;
    };
}
#endif
