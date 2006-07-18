/** 
 * Analyzer for reading mean ADC from CFEB connectivity .
 * author O.Boeriu 14/07/06 
 * ripped from Jeremy's and Rick's analyzers
 *   
 */
#include <iostream>
#include <fstream>
#include <vector>
#include "string"

#include <FWCore/Framework/interface/Frameworkfwd.h>
#include <FWCore/Framework/interface/MakerMacros.h>
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Handle.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/CSCDigi/interface/CSCStripDigi.h"
#include "DataFormats/CSCDigi/interface/CSCStripDigiCollection.h"
#include "DataFormats/FEDRawData/interface/FEDRawData.h"
#include "DataFormats/FEDRawData/interface/FEDNumbering.h"
#include "DataFormats/FEDRawData/interface/FEDRawDataCollection.h"
#include "IORawData/CSCCommissioning/src/FileReaderDDU.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDDUEventData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDCCEventData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCEventData.h"
#include "EventFilter/CSCRawToDigi/interface/CSCDMBHeader.h"
#include "OnlineDB/CSCCondDB/interface/CSCCFEBConnectivityAnalyzer.h"

CSCCFEBConnectivityAnalyzer::CSCCFEBConnectivityAnalyzer(edm::ParameterSet const& conf) {

  debug = conf.getUntrackedParameter<bool>("debug",false);
  eventNumber = 0,strip=0;
  evt = 0,Nddu=0,misMatch=0;
  chamber=0,layer=0,reportedChambers =0;
  length = 1, NChambers=0;
   
  //initialize arrays
  for (int ii=0;ii<DDU_con;ii++){
    for (int i=0; i<CHAMBERS_con; i++){
      for (int j=0; j<LAYERS_con; j++){
	for (int k=0; k<STRIPS_con;k++){
	  adcMin[ii][i][j][k]    = 9999999.0;
	  adcMax[ii][i][j][k]    = -9999999.0;
	  adcMean_max[ii][i][j][k]=0.0;
	  adcMean_min[ii][i][j][k]=0.0;
	  diff[ii][i][j][k]      = 0.0;
	}
      }
    }
  }
}

void CSCCFEBConnectivityAnalyzer::analyze(edm::Event const& e, edm::EventSetup const& iSetup) {
  
  // These declarations create handles to the types of records that you want
  // to retrieve from event "e".
  //
  edm::Handle<CSCStripDigiCollection> strips;
  
  e.getByLabel("cscunpacker","MuonCSCStripDigi",strips);
  
  edm::Handle<FEDRawDataCollection> rawdata;
  e.getByType(rawdata);
  //event =e.id().event();
  for (int id=FEDNumbering::getCSCFEDIds().first;
       id<=FEDNumbering::getCSCFEDIds().second; ++id){ //for each of our DCCs
    
    /// Take a reference to this FED's data
    const FEDRawData& fedData = rawdata->FEDData(id);
    
    if (fedData.size()){ ///unpack data 
      
      ///get a pointer to data and pass it to constructor for unpacking
      CSCDCCEventData dccData((short unsigned int *) fedData.data()); 
      
      const std::vector<CSCDDUEventData> & dduData = dccData.dduData(); 
      evt++;
      
      for (unsigned int iDDU=0; iDDU<dduData.size(); ++iDDU) {  ///loop over DDUs
	///get a reference to chamber data
	const std::vector<CSCEventData> & cscData = dduData[iDDU].cscData();
	Nddu=dduData.size();
	reportedChambers += dduData[iDDU].header().ncsc();
	NChambers = cscData.size();
	int repChambers = dduData[iDDU].header().ncsc();
	std::cout << " Reported Chambers = " << repChambers <<"   "<<NChambers<< std::endl;
	if (NChambers!=repChambers) {std::cout<< "misMatched size!!!" << std::endl; misMatch++;continue;}
	
	for (chamber=0; chamber<NChambers; chamber++) {//loop over all DMBs  
	  
	  for(layer = 1; layer <= 6; ++layer) {//loop over all layers in chambers
	    	    
	    std::vector<CSCStripDigi> digis = cscData[chamber].stripDigis(layer) ;
	    const CSCDMBHeader &thisDMBheader = cscData[chamber].dmbHeader();
	     
	    if (thisDMBheader.cfebAvailable()){//check that CFEB data exists
	      
	      dmbID[chamber]   = cscData[chamber].dmbHeader().dmbID(); //get DMB ID
	      crateID[chamber] = cscData[chamber].dmbHeader().crateID(); //get crate ID
	      if(crateID[chamber] == 255) continue; //255 doesn't exist
	      
	      for (unsigned int i=0; i<digis.size(); i++){
		size[chamber] = digis.size();
		strip = digis[i].getStrip();
		std::vector<int> adc = digis[i].getADCCounts();
		
		for(unsigned int k=0; k<adc.size(); k++){
		  
		  if(adc[k] > adcMax[iDDU][chamber][layer-1][strip-1]) {
		    adcMax[iDDU][chamber][layer-1][strip-1]= adc[k];
		  }
		  
		  if(adc[k] < adcMin[iDDU][chamber][layer-1][strip-1]){
		    adcMin[iDDU][chamber][layer-1][strip-1]= adc[k];
		  }
		  
		}//end timebins loop

		adcMean_max[iDDU][chamber][layer-1][strip-1] += adcMax[iDDU][chamber][layer-1][strip-1]/20.;
		adcMean_min[iDDU][chamber][layer-1][strip-1] += adcMin[iDDU][chamber][layer-1][strip-1]/20.;
	      
	      }//end digis size
	    }//end if cfeb.available 
	  }//end loop over layers
        }//end loop over chambers
	
	if((evt-1)%20==0){
	  for(int iii=0;iii<DDU_con;iii++){
	    for(int ii=0; ii<CHAMBERS_con; ii++){
	      for(int jj=0; jj<LAYERS_con; jj++){
		for(int kk=0; kk<STRIPS_con; kk++){
		  adcMean_max[iii][ii][jj][kk]=0.0;
		  adcMean_min[iii][ii][jj][kk]=0.0;
		}
	      }
	    }
	  }
	}
	
	eventNumber++;
	edm::LogInfo ("CSCCFEBConnectivityAnalyzer")  << "end of event number " << eventNumber;
	
      }
    }
  }
}

