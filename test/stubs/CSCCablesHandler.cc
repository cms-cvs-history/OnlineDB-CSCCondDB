#include "OnlineDB/CSCCondDB/test/stubs/CSCCablesHandler.h"
#include "FWCore/ParameterSet/interface/ParameterSetfwd.h"
#include<iostream>

#include "CondFormats/CSCObjects/interface/CSCCables.h"
#include "OnlineDB/CSCCondDB/interface/CSCCableValues.h"

popcon::CSCCablesImpl::CSCCablesImpl(const edm::ParameterSet& pset): m_name(pset.getUntrackedParameter<std::string>("name","CSCCablesImpl"))
{}

popcon::CSCCablesImpl::~CSCCablesImpl()
{
}

void popcon::CSCCablesImpl::getNewObjects()
{

  std::cout << "------- CSC src - > getNewObjects\n"<<m_name;
  
  // fill object from file
  CSCCables * mycables = CSCCableValues::fillCables();
  
  //check whats already inside of database  
  std::cerr<<"got offlineInfo"<<std::endl;
  std::cerr << tagInfo().name << " , last object valid since " 
	    << tagInfo().lastInterval.first << std::endl; 
  
  unsigned int snc;
  
  std::cout << "Source implementation test ::getNewObjects : enter since ? \n";
  std::cin >> snc;
 
  
  m_to_transfer.push_back(std::make_pair((CSCCables*)mycables,snc));
  
  std::cout << "------- " << m_name << "CSC src - > getNewObjects -----------\n"<< std::endl;
}
