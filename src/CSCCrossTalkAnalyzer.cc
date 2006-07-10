#include <iostream>
#include <fstream>
#include <vector>
#include "string"

#include <FWCore/Framework/interface/Frameworkfwd.h>
#include <FWCore/Framework/interface/MakerMacros.h>
#include "FWCore/MessageLogger/interface/MessageLogger.h"
//#include "FWCore/MessageLogger/data/MessageLogger.cfi"
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
#include "OnlineDB/CSCCondDB/interface/CSCCrossTalkAnalyzer.h"

//bool CSCCrossTalkAnalyzer::debug=false;

CSCCrossTalkAnalyzer::CSCCrossTalkAnalyzer(edm::ParameterSet const& conf) {

  debug = conf.getUntrackedParameter<bool>("debug",false);
  eventNumber=0, Nddu=0,chamber=0;
  strip=0,misMatch=0,max1 =-9999999.,max2=-9999999.;
  layer=0,reportedChambers=0;
  length=1,myevt=0,flag=-9;
  aPeak=0.0,sumFive=0.0;
  pedMean=0.0,evt=0,NChambers=0;

   //definition of histograms
  xtime = TH1F("time", "time", 50, 0, 500 );
  pulse_shape_ch1 = TH2F("pulse shape_ch1","pulse shape_ch1", 100,-100,500,100,-100,1100);
  pulse_shape_ch2 = TH2F("pulse shape_ch2","pulse shape_ch2", 100,-100,500,100,-100,1100);
  pulse_shape_ch3 = TH2F("pulse shape_ch3","pulse shape_ch3", 100,-100,500,100,-100,1100);
  pulse_shape_ch4 = TH2F("pulse shape_ch4","pulse shape_ch4", 100,-100,500,100,-100,1100);
  pulse_shape_ch5 = TH2F("pulse shape_ch5","pulse shape_ch5", 100,-100,500,100,-100,1100);
  pulse_shape_ch6 = TH2F("pulse shape_ch6","pulse shape_ch6", 100,-100,500,100,-100,1100);
  pulse_shape_ch7 = TH2F("pulse shape_ch7","pulse shape_ch7", 100,-100,500,100,-100,1100);
  pulse_shape_ch8 = TH2F("pulse shape_ch8","pulse shape_ch8", 100,-100,500,100,-100,1100);
  pulse_shape_ch9 = TH2F("pulse shape_ch9","pulse shape_ch9", 100,-100,500,100,-100,1100);
  ped_mean_all    = TH1F("pedMean","Mean baseline noise", 100,300,900);
  maxADC          = TH1F("maxADC","Peak ADC", 100,800,1300);


  for (int i=0;i<480;i++){
    new_xtalk_intercept_right[i] = -999.;
    new_xtalk_intercept_left[i]  = -999.;
    new_xtalk_slope_right[i]     = -999.;
    new_xtalk_slope_left[i]      = -999.;
    new_rchi2[i]                 = -999.;
    new_lchi2[i]                 = -999.;
    newPeakTime[i]               = -999.;
    newMeanPeakTime[i]           = -999.;
    newPed[i]                    = 0  ;
    newRMS[i]                    = 0.0;
    newPeakRMS[i]                = 0.0;
    newPeak[i]                   = 0.0;
    newSumFive[i]                = 0.0;
  }
  
  for (int l=0; l<TIMEBINS_xt; l++){
    myTime[l] = 0.0;
    myADC[l]  = 0.0;
    myTbin[l] = 0;
  }
  
  for (int i=0;i<CHAMBERS_xt;i++){
    size[i]                      = 0;
  }
  
  for (int iii=0;iii<DDU_xt;iii++){
    for (int i=0; i<CHAMBERS_xt; i++){
      for (int j=0; j<LAYERS_xt; j++){
	for (int k=0; k<STRIPS_xt; k++){
	  for (int l=0; l<TIMEBINS_xt*20; l++){
	    thetime[iii][i][j][k][l]       = 0.0;
	    thebins[iii][i][j][k][l]       = 0  ;
	    theadccountsc[iii][i][j][k][l] = 0  ;
	    theadccountsl[iii][i][j][k][l] = 0  ;
	    theadccountsr[iii][i][j][k][l] = 0  ;
	    arrayOfPed[iii][i][j][k]       = 0.;
	    arrayOfPedSquare[iii][i][j][k] = 0.;
	    arrayPed[iii][i][j][k]         = 0.;
	    arrayPeak[iii][i][j][k]        = 0.;
	    arrayOfPeak[iii][i][j][k]      = 0.; 
	    arrayOfPeakSquare[iii][i][j][k]= 0.;
	    arraySumFive[iii][i][j][k]     = 0.;
	    
	  }
	}
      }
    }
  }
  
  
  for (int iii=0;iii<DDU_xt;iii++){
    for (int i=0; i<CHAMBERS_xt; i++){
      for (int j=0; j<LAYERS_xt; j++){
	for (int k=0; k<STRIPS_xt; k++){
	  xtalk_intercept_left[iii][i][j][k]  = -999.;
	  xtalk_intercept_right[iii][i][j][k] = -999.;
	  xtalk_slope_left[iii][i][j][k]      = -999.;
	  xtalk_slope_right[iii][i][j][k]     = -999.;
	  xtalk_chi2_left[iii][i][j][k]       = -999.;
	  xtalk_chi2_right[iii][i][j][k]      = -999.;
	  myPeakTime[iii][i][j][k]            =  0.0 ;
	  myMeanPeakTime[iii][i][j][k]        =  0.0 ;
	  array_meanPeakTime[iii][i][j][k]    = -999.;
	}
      }
    }  
  }
}

void CSCCrossTalkAnalyzer::analyze(edm::Event const& e, edm::EventSetup const& iSetup) {
 
  edm::Handle<CSCStripDigiCollection> strips;
  e.getByLabel("cscunpacker","MuonCSCStripDigi",strips);
  edm::Handle<FEDRawDataCollection> rawdata;
  e.getByType(rawdata); //before 0_7_0_pre4 use getByLabel("DaqSource", rawdata)
  myevt=e.id().event();

  for (int id=FEDNumbering::getCSCFEDIds().first;
       id<=FEDNumbering::getCSCFEDIds().second; ++id){ //for each of our DCCs
    
    
    /// Take a reference to this FED's data
    const FEDRawData& fedData = rawdata->FEDData(id);
    if (fedData.size()){ ///unpack data 
      
      ///get a pointer to data and pass it to constructor for unpacking
      CSCDCCEventData dccData((short unsigned int *) fedData.data()); 
      
      const std::vector<CSCDDUEventData> & dduData = dccData.dduData(); 
      
      evt++;  
 
      for (unsigned int iDDU=0; iDDU<dduData.size(); ++iDDU) { 
	
	///get a reference to chamber data
	const std::vector<CSCEventData> & cscData = dduData[iDDU].cscData();
	Nddu = dduData.size();
	reportedChambers += dduData[iDDU].header().ncsc();
	NChambers = cscData.size();
	int repChambers = dduData[iDDU].header().ncsc();
	std::cout << " Reported Chambers = " << repChambers <<"   "<<NChambers<< std::endl;
	if (NChambers!=repChambers) { std::cout<< "misMatched size!!!" << std::endl; misMatch++; continue;}

	for (int chamber = 0; chamber < NChambers; chamber++){
	  
	  for (int layer = 1; layer <= 6; layer++){
	    
	    std::vector<CSCStripDigi> digis = cscData[chamber].stripDigis(layer) ;
	    const CSCDMBHeader &thisDMBheader = cscData[chamber].dmbHeader();
	    
            if (thisDMBheader.cfebAvailable()){
              dmbID[chamber] = cscData[chamber].dmbHeader().dmbID();
              crateID[chamber] = cscData[chamber].dmbHeader().crateID();
              if(crateID[chamber] == 255) continue;
	      
              for (unsigned int i=0; i<digis.size(); i++){
                size[chamber] = digis.size();
		int strip = digis[i].getStrip();
                std::vector<int> adc = digis[i].getADCCounts();
		pedMean1 =(adc[0]+adc[1])/2;
		int offset = evt / 20;
                int smain[5],splus[5],sminus[5]; //5 for CFEBs
                for(int s=0;s<5;s++) smain[s]  = s*16+offset;
                for(int s=0;s<5;s++) splus[s]  = s*16+offset+1;
                for(int s=0;s<5;s++) sminus[s] = s*16+offset-1;
                int iuse=-99;
                for(int s=0; s<5; s++) {if(strip-1==smain[s])  iuse=smain[s];}
                for(int s=0; s<5; s++) {if(strip-1==splus[s])  iuse=smain[s];}
                for(int s=0; s<5; s++) {if(strip-1==sminus[s]) iuse=smain[s];}
		
                if(iuse!=-99){
		  
                  for(unsigned int k=0;k<adc.size();k++){
                    time = (50. * k)-((evt%20)* 6.25)+116.5;
		    pedMean =(adc[0]+adc[1])/2;
		    ped_mean_all.Fill(pedMean);  
		    xtime.Fill(time);
		    if(chamber==0  && strip==1) pulse_shape_ch1.Fill(time,adc[k]-pedMean);
		    if(chamber==1  && strip==1) pulse_shape_ch2.Fill(time,adc[k]-pedMean);
		    if(chamber==2  && strip==1) pulse_shape_ch3.Fill(time,adc[k]-pedMean);
		    if(chamber==3  && strip==1) pulse_shape_ch4.Fill(time,adc[k]-pedMean);
		    if(chamber==4  && strip==1) pulse_shape_ch5.Fill(time,adc[k]-pedMean);
		    if(chamber==5  && strip==1) pulse_shape_ch6.Fill(time,adc[k]-pedMean);
		    if(chamber==6  && strip==1) pulse_shape_ch7.Fill(time,adc[k]-pedMean);
		    if(chamber==7  && strip==1) pulse_shape_ch8.Fill(time,adc[k]-pedMean);
		    if(chamber==8  && strip==1) pulse_shape_ch9.Fill(time,adc[k]-pedMean);
		    
		    myTime[k]=time;
		    myADC[k]=adc[k];
		    myTbin[k]=k;

		    aPeak = adc[3];
		    if (max1 < aPeak) {
		      max1 = aPeak;
		    }
		    sumFive = adc[2]+adc[3]+adc[4];
		    
		    if (max2<sumFive){
		      max2=sumFive;
		    }

		    maxADC.Fill(max1-pedMean1);

                    int kk=8*k-evt%20+19;//19 to zero everything, for binning 120
		    
                    thebins[iDDU][chamber][layer-1][strip-1][kk] = 8*k-evt%20+19;
		    thetime[iDDU][chamber][layer-1][strip-1][kk] = time;
		    
                    if(iuse==strip-1)  theadccountsc[iDDU][chamber][layer-1][iuse][kk] = adc[k];
                    if(iuse==strip)    theadccountsr[iDDU][chamber][layer-1][iuse][kk] = adc[k];
                    if(iuse==strip-2)  theadccountsl[iDDU][chamber][layer-1][iuse][kk] = adc[k];
		  }//adc.size()
		}//end iuse!=99
	
		arrayPed[iDDU][chamber][layer-1][strip-1] = pedMean1;	
		arrayOfPed[iDDU][chamber][layer-1][strip-1] += pedMean1;
		arrayOfPedSquare[iDDU][chamber][layer-1][strip-1] += pedMean1*pedMean1 ;
		arrayPeak[iDDU][chamber][layer-1][strip-1] = max1-pedMean1;
		arrayOfPeak[iDDU][chamber][layer-1][strip-1] += max1-pedMean1;
		arrayOfPeakSquare[iDDU][chamber][layer-1][strip-1] += (max1-pedMean1)*(max1-pedMean1);
		arraySumFive[iDDU][chamber][layer-1][strip-1] = (max2-pedMean1)/(max1-pedMean1);
		
	      }//end loop over digis
            }//end cfeb.available loop
          }//end loop over layers
        }//end loop over chambers

	eventNumber++;
	edm::LogInfo ("CSCCrossTalkAnalyzer")  << "end of event number " << eventNumber;

      }
    }
  }
}
