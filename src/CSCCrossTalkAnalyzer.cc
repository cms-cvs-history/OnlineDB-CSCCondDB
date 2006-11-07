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
#include "OnlineDB/CSCCondDB/interface/CSCCrossTalkAnalyzer.h"
#include "OnlineDB/CSCCondDB/interface/CSCOnlineDB.h"
#include "OnlineDB/CSCCondDB/interface/CSCxTalk.h"
#include "CondFormats/CSCObjects/interface/CSCcrosstalk.h"
#include "CondFormats/CSCObjects/interface/CSCPedestals.h"
#include "CondFormats/CSCObjects/interface/CSCobject.h"
#include "OnlineDB/CSCCondDB/interface/CSCMap.h"

CSCCrossTalkAnalyzer::CSCCrossTalkAnalyzer(edm::ParameterSet const& conf) {

  debug = conf.getUntrackedParameter<bool>("debug",false);
  eventNumber=0, Nddu=0,chamber=0;
  strip=0,misMatch=0,max1 =-9999999.,max2=-9999999., min1=9999999.;
  layer=0,reportedChambers=0;
  length=1,myevt=0,flagRMS=-9,flagNoise=-9;
  aPeak=0.0,sumFive=0.0,maxPed=-99999.0,maxRMS=-99999.0;
  maxPeakTime=-9999.0, maxPeakADC=-999.0;
  minPeakTime=9999.0, minPeakADC=999.0;
  pedMean=0.0,evt=0,NChambers=0;
  
  //definition of histograms
  xtime = TH1F("time", "time", 50, 0, 500 );
  pulseshape_ch1_cfeb1 = TH2F("pulse01","strip8",  100,-100,500,100,-100,1100);
  pulseshape_ch1_cfeb2 = TH2F("pulse02","strip24", 100,-100,500,100,-100,1100);
  pulseshape_ch1_cfeb3 = TH2F("pulse03","strip40", 100,-100,500,100,-100,1100);
  pulseshape_ch1_cfeb4 = TH2F("pulse04","strip56", 100,-100,500,100,-100,1100);
  pulseshape_ch1_cfeb5 = TH2F("pulse05","strip72", 100,-100,500,100,-100,1100);
  pulseshape_ch2_cfeb1 = TH2F("pulse11","strip8",  100,-100,500,100,-100,1100);
  pulseshape_ch2_cfeb2 = TH2F("pulse12","strip24", 100,-100,500,100,-100,1100);
  pulseshape_ch2_cfeb3 = TH2F("pulse13","strip40", 100,-100,500,100,-100,1100);
  pulseshape_ch2_cfeb4 = TH2F("pulse14","strip56", 100,-100,500,100,-100,1100);
  pulseshape_ch2_cfeb5 = TH2F("pulse15","strip72", 100,-100,500,100,-100,1100);
  pulseshape_ch3_cfeb1 = TH2F("pulse21","strip8",  100,-100,500,100,-100,1100);
  pulseshape_ch3_cfeb2 = TH2F("pulse22","strip24", 100,-100,500,100,-100,1100);
  pulseshape_ch3_cfeb3 = TH2F("pulse23","strip40", 100,-100,500,100,-100,1100);
  pulseshape_ch3_cfeb4 = TH2F("pulse24","strip56", 100,-100,500,100,-100,1100);
  pulseshape_ch3_cfeb5 = TH2F("pulse25","strip72", 100,-100,500,100,-100,1100);
  pulseshape_ch4_cfeb1 = TH2F("pulse31","strip8",  100,-100,500,100,-100,1100);
  pulseshape_ch4_cfeb2 = TH2F("pulse32","strip24", 100,-100,500,100,-100,1100);
  pulseshape_ch4_cfeb3 = TH2F("pulse33","strip40", 100,-100,500,100,-100,1100);
  pulseshape_ch4_cfeb4 = TH2F("pulse34","strip56", 100,-100,500,100,-100,1100);
  pulseshape_ch4_cfeb5 = TH2F("pulse35","strip72", 100,-100,500,100,-100,1100);
  pulseshape_ch5_cfeb1 = TH2F("pulse41","strip8",  100,-100,500,100,-100,1100);
  pulseshape_ch5_cfeb2 = TH2F("pulse42","strip24", 100,-100,500,100,-100,1100);
  pulseshape_ch5_cfeb3 = TH2F("pulse43","strip40", 100,-100,500,100,-100,1100);
  pulseshape_ch5_cfeb4 = TH2F("pulse44","strip56", 100,-100,500,100,-100,1100);
  pulseshape_ch5_cfeb5 = TH2F("pulse45","strip72", 100,-100,500,100,-100,1100);
  pulseshape_ch6_cfeb1 = TH2F("pulse51","strip8",  100,-100,500,100,-100,1100);
  pulseshape_ch6_cfeb2 = TH2F("pulse52","strip24", 100,-100,500,100,-100,1100);
  pulseshape_ch6_cfeb3 = TH2F("pulse53","strip40", 100,-100,500,100,-100,1100);
  pulseshape_ch6_cfeb4 = TH2F("pulse54","strip56", 100,-100,500,100,-100,1100);
  pulseshape_ch6_cfeb5 = TH2F("pulse55","strip72", 100,-100,500,100,-100,1100);
  pulseshape_ch7_cfeb1 = TH2F("pulse61","strip8",  100,-100,500,100,-100,1100);
  pulseshape_ch7_cfeb2 = TH2F("pulse62","strip24", 100,-100,500,100,-100,1100);
  pulseshape_ch7_cfeb3 = TH2F("pulse63","strip40", 100,-100,500,100,-100,1100);
  pulseshape_ch7_cfeb4 = TH2F("pulse64","strip56", 100,-100,500,100,-100,1100);
  pulseshape_ch7_cfeb5 = TH2F("pulse65","strip72", 100,-100,500,100,-100,1100);
  pulseshape_ch8_cfeb1 = TH2F("pulse71","strip8",  100,-100,500,100,-100,1100);
  pulseshape_ch8_cfeb2 = TH2F("pulse72","strip24", 100,-100,500,100,-100,1100);
  pulseshape_ch8_cfeb3 = TH2F("pulse73","strip40", 100,-100,500,100,-100,1100);
  pulseshape_ch8_cfeb4 = TH2F("pulse74","strip56", 100,-100,500,100,-100,1100);
  pulseshape_ch8_cfeb5 = TH2F("pulse75","strip72", 100,-100,500,100,-100,1100);
  pulseshape_ch9_cfeb1 = TH2F("pulse81","strip8",  100,-100,500,100,-100,1100);
  pulseshape_ch9_cfeb2 = TH2F("pulse82","strip24", 100,-100,500,100,-100,1100);
  pulseshape_ch9_cfeb3 = TH2F("pulse83","strip40", 100,-100,500,100,-100,1100);
  pulseshape_ch9_cfeb4 = TH2F("pulse84","strip56", 100,-100,500,100,-100,1100);
  pulseshape_ch9_cfeb5 = TH2F("pulse85","strip72", 100,-100,500,100,-100,1100);

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
    newPeakMin[i]                = 0.0; 
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
	    arrayPeakMin[iii][i][j][k]     = 0.;
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
                    time = (50. * k)-(((evt-1)%20)* 6.25)+116.5;
		    pedMean =(adc[0]+adc[1])/2;
		    ped_mean_all.Fill(pedMean);  
		    xtime.Fill(time);
		    if(chamber==0  && strip==8)  pulseshape_ch1_cfeb1.Fill(time,adc[k]-pedMean);
		    if(chamber==0  && strip==24) pulseshape_ch1_cfeb2.Fill(time,adc[k]-pedMean);
		    if(chamber==0  && strip==40) pulseshape_ch1_cfeb3.Fill(time,adc[k]-pedMean);
		    if(chamber==0  && strip==56) pulseshape_ch1_cfeb4.Fill(time,adc[k]-pedMean);
		    if(chamber==0  && strip==72) pulseshape_ch1_cfeb5.Fill(time,adc[k]-pedMean);
		    if(chamber==1  && strip==8)  pulseshape_ch2_cfeb1.Fill(time,adc[k]-pedMean);
		    if(chamber==1  && strip==24) pulseshape_ch2_cfeb2.Fill(time,adc[k]-pedMean);
		    if(chamber==1  && strip==40) pulseshape_ch2_cfeb3.Fill(time,adc[k]-pedMean);
		    if(chamber==1  && strip==56) pulseshape_ch2_cfeb4.Fill(time,adc[k]-pedMean);
		    if(chamber==1  && strip==72) pulseshape_ch2_cfeb5.Fill(time,adc[k]-pedMean);
		    if(chamber==2  && strip==8)  pulseshape_ch3_cfeb1.Fill(time,adc[k]-pedMean);
		    if(chamber==2  && strip==24) pulseshape_ch3_cfeb2.Fill(time,adc[k]-pedMean);
		    if(chamber==2  && strip==40) pulseshape_ch3_cfeb3.Fill(time,adc[k]-pedMean);
		    if(chamber==2  && strip==56) pulseshape_ch3_cfeb4.Fill(time,adc[k]-pedMean);
		    if(chamber==2  && strip==72) pulseshape_ch3_cfeb5.Fill(time,adc[k]-pedMean);
		    if(chamber==3  && strip==8)  pulseshape_ch4_cfeb1.Fill(time,adc[k]-pedMean);
		    if(chamber==3  && strip==24) pulseshape_ch4_cfeb2.Fill(time,adc[k]-pedMean);
		    if(chamber==3  && strip==40) pulseshape_ch4_cfeb3.Fill(time,adc[k]-pedMean);
		    if(chamber==3  && strip==56) pulseshape_ch4_cfeb4.Fill(time,adc[k]-pedMean);
		    if(chamber==3  && strip==72) pulseshape_ch4_cfeb5.Fill(time,adc[k]-pedMean);
		    if(chamber==4  && strip==8)  pulseshape_ch5_cfeb1.Fill(time,adc[k]-pedMean);
		    if(chamber==4  && strip==24) pulseshape_ch5_cfeb2.Fill(time,adc[k]-pedMean);
		    if(chamber==4  && strip==40) pulseshape_ch5_cfeb3.Fill(time,adc[k]-pedMean);
		    if(chamber==4  && strip==56) pulseshape_ch5_cfeb4.Fill(time,adc[k]-pedMean);
		    if(chamber==4  && strip==72) pulseshape_ch5_cfeb5.Fill(time,adc[k]-pedMean);
		    if(chamber==5  && strip==8)  pulseshape_ch6_cfeb1.Fill(time,adc[k]-pedMean);
		    if(chamber==5  && strip==24) pulseshape_ch6_cfeb2.Fill(time,adc[k]-pedMean);
		    if(chamber==5  && strip==40) pulseshape_ch6_cfeb3.Fill(time,adc[k]-pedMean);
		    if(chamber==5  && strip==56) pulseshape_ch6_cfeb4.Fill(time,adc[k]-pedMean);
		    if(chamber==5  && strip==72) pulseshape_ch6_cfeb5.Fill(time,adc[k]-pedMean);
		    if(chamber==6  && strip==8)  pulseshape_ch7_cfeb1.Fill(time,adc[k]-pedMean);
		    if(chamber==6  && strip==24) pulseshape_ch7_cfeb2.Fill(time,adc[k]-pedMean);
		    if(chamber==6  && strip==40) pulseshape_ch7_cfeb3.Fill(time,adc[k]-pedMean);
		    if(chamber==6  && strip==56) pulseshape_ch7_cfeb4.Fill(time,adc[k]-pedMean);
		    if(chamber==6  && strip==72) pulseshape_ch7_cfeb5.Fill(time,adc[k]-pedMean);
		    if(chamber==7  && strip==8)  pulseshape_ch8_cfeb1.Fill(time,adc[k]-pedMean);
		    if(chamber==7  && strip==24) pulseshape_ch8_cfeb2.Fill(time,adc[k]-pedMean);
		    if(chamber==7  && strip==40) pulseshape_ch8_cfeb3.Fill(time,adc[k]-pedMean);
		    if(chamber==7  && strip==56) pulseshape_ch8_cfeb4.Fill(time,adc[k]-pedMean);
		    if(chamber==7  && strip==72) pulseshape_ch8_cfeb5.Fill(time,adc[k]-pedMean);
		    if(chamber==8  && strip==8)  pulseshape_ch9_cfeb1.Fill(time,adc[k]-pedMean);
		    if(chamber==8  && strip==24) pulseshape_ch9_cfeb2.Fill(time,adc[k]-pedMean);
		    if(chamber==8  && strip==40) pulseshape_ch9_cfeb3.Fill(time,adc[k]-pedMean);
		    if(chamber==8  && strip==56) pulseshape_ch9_cfeb4.Fill(time,adc[k]-pedMean);
		    if(chamber==8  && strip==72) pulseshape_ch9_cfeb5.Fill(time,adc[k]-pedMean);
		    
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

		    if (min1 > aPeak){
		      min1=aPeak;
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
		arrayPeakMin[iDDU][chamber][layer-1][strip-1] = min1;
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

CSCCrossTalkAnalyzer::~CSCCrossTalkAnalyzer(){
  std::cout << "entering destructor " << std::endl;

  Conv binsConv;
  //get time of Run file for DB transfer
  filein.open("../test/CSCxtalk.cfg");
  filein.ignore(1000,'\n');
    
  while(filein != NULL){
    lines++;
    getline(filein,PSet);
    
    if (lines==3){
      name=PSet;  
    }
  }
  
  std::cout << "getting file name " << std::endl;

  //get name of run file from .cfg and name root output after that
  string::size_type runNameStart = name.find("\"",0);
  string::size_type runNameEnd   = name.find("bin",0);
  string::size_type rootStart    = name.find("Crosstalk",0);
  int nameSize = runNameEnd+2-runNameStart;
  int myRootSize = rootStart-runNameStart+8;
  std::string myname= name.substr(runNameStart+1,nameSize);
  std::string myRootName= name.substr(runNameStart+1,myRootSize);
  std::string myRootEnd = ".root";
  std::string runFile= myRootName;
  std::string myRootFileName = runFile+myRootEnd;
  const char *myNewName=myRootFileName.c_str();
  
  struct tm* clock;			    
  struct stat attrib;			    
  stat(myname.c_str(), &attrib);          
  clock = localtime(&(attrib.st_mtime));  
  std::string myTime=asctime(clock);

  //DB object and map
  CSCobject *cn = new CSCobject();
  CSCobject *cn1 = new CSCobject();
  cscmap *map = new cscmap();
  condbon *dbon = new condbon();

  //root ntuple
  TCalibCrossTalkEvt calib_evt;
  TFile calibfile(myNewName, "RECREATE");
  TTree calibtree("Calibration","Crosstalk");
  calibtree.Branch("EVENT", &calib_evt, "xtalk_slope_left/F:xtalk_slope_right/F:xtalk_int_left/F:xtalk_int_right/F:xtalk_chi2_left/F:xtalk_chi2_right/F:peakTime/F:strip/I:layer/I:cham/I:ddu/I:pedMean/F:pedRMS/F:peakRMS/F:maxADC/F:sum/F:id/I:flagRMS/I:flagNoise/I:MaxPed[9]/F:MaxRMS[9]/F:MaxPeakTime[9]/F:MinPeakTime[9]/F:MaxPeakADC[9]/F:MinPeakADC[9]/F");
  xtime.Write();
  ped_mean_all.Write();
  maxADC.Write();

  pulseshape_ch1_cfeb1.Write();
  pulseshape_ch1_cfeb2.Write();
  pulseshape_ch1_cfeb3.Write();
  pulseshape_ch1_cfeb4.Write();
  pulseshape_ch1_cfeb5.Write();
  pulseshape_ch2_cfeb1.Write();
  pulseshape_ch2_cfeb2.Write();
  pulseshape_ch2_cfeb3.Write();
  pulseshape_ch2_cfeb4.Write();
  pulseshape_ch2_cfeb5.Write();
  pulseshape_ch3_cfeb1.Write();
  pulseshape_ch3_cfeb2.Write();
  pulseshape_ch3_cfeb3.Write();
  pulseshape_ch3_cfeb4.Write();
  pulseshape_ch3_cfeb5.Write();
  pulseshape_ch4_cfeb1.Write();
  pulseshape_ch4_cfeb2.Write();
  pulseshape_ch4_cfeb3.Write();
  pulseshape_ch4_cfeb4.Write();
  pulseshape_ch4_cfeb5.Write();
  pulseshape_ch5_cfeb1.Write();
  pulseshape_ch5_cfeb2.Write();
  pulseshape_ch5_cfeb3.Write();
  pulseshape_ch5_cfeb4.Write();
  pulseshape_ch5_cfeb5.Write();
  pulseshape_ch6_cfeb1.Write();
  pulseshape_ch6_cfeb2.Write();
  pulseshape_ch6_cfeb3.Write();
  pulseshape_ch6_cfeb4.Write();
  pulseshape_ch6_cfeb5.Write();
  pulseshape_ch7_cfeb1.Write();
  pulseshape_ch7_cfeb2.Write();
  pulseshape_ch7_cfeb3.Write();
  pulseshape_ch7_cfeb4.Write();
  pulseshape_ch7_cfeb5.Write();
  pulseshape_ch8_cfeb1.Write();
  pulseshape_ch8_cfeb2.Write();
  pulseshape_ch8_cfeb3.Write();
  pulseshape_ch8_cfeb4.Write();
  pulseshape_ch8_cfeb5.Write();
  pulseshape_ch9_cfeb1.Write();
  pulseshape_ch9_cfeb2.Write();
  pulseshape_ch9_cfeb3.Write();
  pulseshape_ch9_cfeb4.Write();
  pulseshape_ch9_cfeb5.Write();

  ////////////////////////////////////////////////////////////////////iuse==strip-1
  // Now that we have filled our array, extract convd and nconvd
  float adc_ped_sub_left = -999.;
  float adc_ped_sub = -999.;
  float adc_ped_sub_right = -999.;
  int thebin;
  float sum=0.0;
  float mean=0;

  for (int iii=0; iii<Nddu; iii++){
    
    for (int i=0; i<NChambers; i++){
      
      //get chamber ID from DB mapping        
      int new_crateID = crateID[i];
      int new_dmbID   = dmbID[i];
      std::cout<<" Crate: "<<new_crateID<<" and DMB:  "<<new_dmbID<<std::endl;
      map->crate_chamber(new_crateID,new_dmbID,&chamber_id,&chamber_num,&sector);
      std::cout<<"Data is for chamber:: "<< chamber_num<<" in sector:  "<<sector<<std::endl;

      calib_evt.id = chamber_num;

      meanPedestal = 0.0;
      meanPeak     = 0.0;
      meanPeakSquare=0.0;
      meanPedestalSquare = 0.;
      theRMS      =0.0;
      thePedestal =0.0;
      theRSquare  =0.0;
      thePeak     =0.0;
      thePeakMin  =0.0;//minimum of the high's  
      thePeakRMS  =0.0;
      theSumFive  =0.0;
      
      for (int j=0; j<LAYERS_xt; j++){
	mean=0.,sum=0.;
	for (int s=0; s<size[i]; s++) {
	  //re-zero convd and nconvd
	  for (int m=0; m<3; m++){
	    for (int n=0; n<120; n++){
	      binsConv.convd[m][n]  = 0.;
	      binsConv.nconvd[m][n] = 0.;
	    }
	  }
	 
	  for (int l=0; l < TIMEBINS_xt*20; l++){
	    adc_ped_sub_left  = theadccountsl[iii][i][j][s][l] - (theadccountsl[iii][i][j][s][0] + theadccountsl[iii][i][j][s][1])/2.;
	    adc_ped_sub       = theadccountsc[iii][i][j][s][l] - (theadccountsc[iii][i][j][s][0] + theadccountsc[iii][i][j][s][1])/2.;
	    adc_ped_sub_right = theadccountsr[iii][i][j][s][l] - (theadccountsr[iii][i][j][s][0] + theadccountsr[iii][i][j][s][1])/2.;
	    
	    thebin=thebins[iii][i][j][s][l];
	    
	    if (thebin >= 0 && thebin < 120){
	      binsConv.convd[0][thebin]  += adc_ped_sub_left;
	      binsConv.nconvd[0][thebin] += 1.0;
	      
	      binsConv.convd[1][thebin]  += adc_ped_sub;
	      binsConv.nconvd[1][thebin] += 1.0;
	      
	      binsConv.convd[2][thebin]  += adc_ped_sub_right;
	      binsConv.nconvd[2][thebin] += 1.0;
	      
	    }
	  } //loop over timebins
	  
	  for (int m=0; m<3; m++){
	    for (int n=0; n<120; n++){
	      if(binsConv.nconvd[m][n]>1.0 && binsConv.nconvd[m][n] !=0.){
		binsConv.convd[m][n] = binsConv.convd[m][n]/binsConv.nconvd[m][n];
	      }
	    }
	  }
	  
	  // Call our functions first time to calculate mean pf peak time over a layer
	  float xl_temp_a = 0.0;
	  float xl_temp_b = 0.0;
	  float minl_temp = 0.0;
	  float xr_temp_a = 0.0;
	  float xr_temp_b = 0.0;
	  float minr_temp = 0.0;
	  float pTime     = 0.0;
	  
	  binsConv.mkbins(50.);
	  binsConv.convolution(&xl_temp_a, &xl_temp_b, &minl_temp, &xr_temp_a, &xr_temp_b, &minr_temp, &pTime);
	  myPeakTime[iii][i][j][s] = pTime;
	  sum=sum+myPeakTime[iii][i][j][s];
	  mean = sum/size[i];
	}
	
	int layer_id=chamber_num+j+1;
	if(sector==-100)continue;
	cn->obj[layer_id].resize(size[i]);
	cn1->obj[layer_id].resize(size[i]);
	for (int k=0; k<size[i]; k++){
	  // re-zero convd and nconvd 
	  for (int m=0; m<3; m++){
	    for (int n=0; n<120; n++){
	      binsConv.convd[m][n]  = 0.;
	      binsConv.nconvd[m][n] = 0.;
	    }
	  }
	  
	  for (int l=0; l < TIMEBINS_xt*20; l++){
	    adc_ped_sub_left  = theadccountsl[iii][i][j][k][l] - (theadccountsl[iii][i][j][k][0] + theadccountsl[iii][i][j][k][1])/2.;	  
	    adc_ped_sub       = theadccountsc[iii][i][j][k][l] - (theadccountsc[iii][i][j][k][0] + theadccountsc[iii][i][j][k][1])/2.;
	    adc_ped_sub_right = theadccountsr[iii][i][j][k][l] - (theadccountsr[iii][i][j][k][0] + theadccountsr[iii][i][j][k][1])/2.;
	    
	    thebin=thebins[iii][i][j][k][l];
	    
	    if (thebin >= 0 && thebin < 120){
	      binsConv.convd[0][thebin]  += adc_ped_sub_left;
	      binsConv.nconvd[0][thebin] += 1.0;
	      
	      binsConv.convd[1][thebin]  += adc_ped_sub;
	      binsConv.nconvd[1][thebin] += 1.0;
	      
	      binsConv.convd[2][thebin]  += adc_ped_sub_right;
	      binsConv.nconvd[2][thebin] += 1.0;
	      
	    }
	  } //loop over timebins
	  
	  for (int m=0; m<3; m++){
	    for (int n=0; n<120; n++){
	      if(binsConv.nconvd[m][n]>1.0 && binsConv.nconvd[m][n] !=0.){
		binsConv.convd[m][n] = binsConv.convd[m][n]/binsConv.nconvd[m][n];
	      }
	    }
	  }
	  //////////////////////////////////////////////////////////////////
	  // Call our functions a second time to calculate the cross-talk //
	  //////////////////////////////////////////////////////////////////
	  float xl_temp_a = 0.;
	  float xl_temp_b = 0.;
	  float minl_temp = 0.;
	  float xr_temp_a = 0.;
	  float xr_temp_b = 0.;
	  float minr_temp = 0.;
	  float pTime     = 0.;
	  
	  binsConv.mkbins(50.);
	  binsConv.convolution(&xl_temp_a, &xl_temp_b, &minl_temp, &xr_temp_a, &xr_temp_b, &minr_temp, &pTime);
	  
	  if (k==0){
	    xtalk_intercept_left[iii][i][j][k]  = 0.0;
	    xtalk_slope_left[iii][i][j][k]      = 0.0;
	    xtalk_chi2_left[iii][i][j][k]       = 0.0;
	    //right side is calculated
	    xtalk_slope_right[iii][i][j][k]     = xl_temp_b;
	    xtalk_intercept_right[iii][i][j][k] = xl_temp_a;
	    xtalk_chi2_right[iii][i][j][k]      = minl_temp;
	    myPeakTime[iii][i][j][k]            = pTime;
	  }else if(k==size[i]-1){
	    xtalk_intercept_right[iii][i][j][k]  = 0.0;
	    xtalk_slope_right[iii][i][j][k]      = 0.0;
	    xtalk_chi2_right[iii][i][j][k]       = 0.0;
	    //left side is calculated
	    xtalk_intercept_left[iii][i][j][k]   = xr_temp_a;
	    xtalk_slope_left[iii][i][j][k]       = xr_temp_b;
	    xtalk_chi2_left[iii][i][j][k]        = minr_temp;
	    myPeakTime[iii][i][j][k]             = pTime;
	  }else{
	    xtalk_intercept_left[iii][i][j][k]  = xl_temp_a;
	    xtalk_intercept_right[iii][i][j][k] = xr_temp_a;
	    xtalk_slope_left[iii][i][j][k]      = xl_temp_b;
	    xtalk_slope_right[iii][i][j][k]     = xr_temp_b;
	    xtalk_chi2_left[iii][i][j][k]       = minl_temp;
	    xtalk_chi2_right[iii][i][j][k]      = minr_temp;
	    myPeakTime[iii][i][j][k]            = pTime;
	  }
	  
	  fff = (j*size[i])+k;
	  float the_xtalk_left_a  = xtalk_intercept_left[iii][i][j][k];
	  float the_xtalk_right_a = xtalk_intercept_right[iii][i][j][k];
	  float the_xtalk_left_b  = xtalk_slope_left[iii][i][j][k];
	  float the_xtalk_right_b = xtalk_slope_right[iii][i][j][k];
	  float the_chi2_right    = xtalk_chi2_right[iii][i][j][k];
	  float the_chi2_left     = xtalk_chi2_left[iii][i][j][k];
	  float the_peakTime      = myPeakTime[iii][i][j][k]; 
	  
	  new_xtalk_intercept_right[fff] = the_xtalk_right_a ;
	  new_xtalk_intercept_left[fff]  = the_xtalk_left_a ;
	  new_xtalk_slope_right[fff]     = the_xtalk_right_b ;
	  new_xtalk_slope_left[fff]      = the_xtalk_left_b ;
	  new_rchi2[fff]                 = the_chi2_right;
	  new_lchi2[fff]                 = the_chi2_left;
	  newPeakTime[fff]               = the_peakTime;
	  newMeanPeakTime[fff]           = the_peakTime-mean;
	  
	  //pedestal info
	  thePedestal  = arrayPed[iii][i][j][k];
	  meanPedestal = arrayOfPed[iii][i][j][k]/evt;
	  newPed[fff]  = meanPedestal;
	  meanPedestalSquare = arrayOfPedSquare[iii][i][j][k] / evt;
	  theRMS       = sqrt(abs(meanPedestalSquare - meanPedestal*meanPedestal));

	  newRMS[fff]  = theRMS;
	  theRSquare   = (thePedestal-meanPedestal)*(thePedestal-meanPedestal)/(theRMS*theRMS*theRMS*theRMS);
	  thePeak      = arrayPeak[iii][i][j][k];
	  thePeakMin   = arrayPeakMin[iii][i][j][k];
	  meanPeak     = arrayOfPeak[iii][i][j][k] / evt;
	  meanPeakSquare = arrayOfPeakSquare[iii][i][j][k] / evt;
	  thePeakRMS   = sqrt(abs(meanPeakSquare - meanPeak*meanPeak));
	  newPeakRMS[fff] = thePeakRMS;
	  newPeak[fff] = thePeak;
	  newPeakMin[fff] = thePeakMin;
	  
	  theSumFive = arraySumFive[iii][i][j][k];
	  newSumFive[fff]=theSumFive;

	  if(maxPed<thePedestal){
	    maxPed=thePedestal;
	  }

	  if(maxRMS<theRMS){
	    maxRMS=theRMS;
	  }
	  
	  if(maxPeakTime<the_peakTime){
	    maxPeakTime=the_peakTime;
	  }
	  if(minPeakTime>the_peakTime){
	    minPeakTime=the_peakTime;
	      }

	  //introducing flags for RMS and baseline
	  if (theRMS >1.5 && theRMS <6.0)  flagRMS = 1; // ok
	  if (theRMS>6.0 && theRMS<20.0)   flagRMS = 2; // warning high CFEB noise
	  if (theRMS<1.5)                  flagRMS = 3; // warning/failure too low noise
	  if (theRMS>=20.0)                flagRMS = 4; // warning/failure too high noise
	  
	  if (meanPedestal <50.)                         flagNoise = 2; // warning/failure too low pedestal 
	  if (meanPedestal>50. && meanPedestal<200.)     flagNoise = 3; // warning low pedestal
	  if (meanPedestal >1000. && meanPedestal<3000.) flagNoise = 4; // warning high pedstal
	  if (meanPedestal>3000.)                        flagNoise = 5; // warning/failure too high pedestal 
	  if (meanPedestal>200. && meanPedestal<1000.)   flagNoise = 1; // ok

	  std::cout <<"Ch "<<i<<" L "<<j<<" S "<<k<<"  ped "<<meanPedestal<<" RMS "<<theRMS<<" maxADC "<<thePeak<< "minADC of Max" << thePeakMin <<" IntL "<<the_xtalk_left_a<<" SL "<<the_xtalk_left_b<<" IntR "<<the_xtalk_right_a<<" SR "<<the_xtalk_right_b<<" flagRMS "<<flagRMS<<std::endl;
	  calib_evt.xtalk_slope_left  = xtalk_slope_left[iii][i][j][k]; 
	  calib_evt.xtalk_slope_right = xtalk_slope_right[iii][i][j][k]; 
	  calib_evt.xtalk_int_left    = xtalk_intercept_left[iii][i][j][k];
	  calib_evt.xtalk_int_right   = xtalk_intercept_right[iii][i][j][k];
	  calib_evt.xtalk_chi2_left   = xtalk_chi2_left[iii][i][j][k];
	  calib_evt.xtalk_chi2_right  = xtalk_chi2_right[iii][i][j][k];
	  calib_evt.peakTime          = myPeakTime[iii][i][j][k];
	  calib_evt.cham              = i;
	  calib_evt.ddu               = iii;
	  calib_evt.layer             = j;
	  calib_evt.strip             = k;
	  calib_evt.flagRMS           = flagRMS;
	  calib_evt.flagNoise         = flagNoise;
	  calib_evt.pedMean           = newPed[fff];
	  calib_evt.pedRMS            = newRMS[fff];
	  calib_evt.peakRMS           = newPeakRMS[fff];
	  calib_evt.maxADC            = newPeak[fff];
	  calib_evt.sum               = newSumFive[fff];
	  calib_evt.MaxPed[i]         = maxPed;
	  calib_evt.MaxRMS[i]         = maxRMS;
	  calib_evt.MaxPeakTime[i]    = maxPeakTime;
	  calib_evt.MinPeakTime[i]    = minPeakTime;
	  calib_evt.MaxPeakADC[i]     = newPeak[fff];
	  calib_evt.MinPeakADC[i]     = newPeakMin[fff];

	  calibtree.Fill();
	  cn->obj[layer_id][k].resize(2);
	  cn->obj[layer_id][k][0] = meanPedestal;
	  cn->obj[layer_id][k][1] = theRMS;
	  cn1->obj[layer_id][k].resize(6);
	  cn1->obj[layer_id][k][0] = the_xtalk_right_b ;
	  cn1->obj[layer_id][k][1] = the_xtalk_right_a ;
	  cn1->obj[layer_id][k][2] = the_chi2_right;
	  cn1->obj[layer_id][k][3] = the_xtalk_left_b ;
	  cn1->obj[layer_id][k][4] = the_xtalk_left_a ;
	  cn1->obj[layer_id][k][5] = the_chi2_left;
	  
	}//loop over strips
      }//loop over layers
    }//chambers
  }//Nddu


  dbon->cdbon_last_record("pedestals",&record);
  std::cout<<"Last pedestal record "<<record<<std::endl;
  if(debug) dbon->cdbon_write(cn,"pedestals",11,myTime);
  dbon->cdbon_last_record("crosstalk",&record);
  if(debug) dbon->cdbon_write(cn1,"crosstalk",11,myTime);
  std::cout << "Last crosstalk record " << record << " for run file " << myname <<" saved "<< myTime << std::endl;  
  calibfile.Write();
  calibfile.Close();  

}  
