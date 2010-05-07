#include "CondCore/PopCon/interface/PopConAnalyzer.h"
#include "CSCCablesHandler.h"
#include "FWCore/Framework/interface/MakerMacros.h"

typedef popcon::PopConAnalyzer<popcon::CSCCablesImpl> CSCCablesPopConAnalyzer;

DEFINE_FWK_MODULE(CSCCablesPopConAnalyzer);
