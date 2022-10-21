#include"global.h"

bool semanticParse(){
    logger.log("semanticParse");
    switch(parsedQuery.queryType){
        case CLEAR: return semanticParseCLEAR();
        case CROSS: return semanticParseCROSS();
        case CROSS_TRANSPOSE: return semanticParseCROSS_TRANSPOSE();
        case DISTINCT: return semanticParseDISTINCT();
        case GROUPBY: return semanticParseGROUPBY();
        case EXPORT_MATRIX: return semanticParseEXPORT();
        case EXPORT: return semanticParseEXPORT();
        case INDEX: return semanticParseINDEX();
        case JOIN: return semanticParseJOIN();
        case LIST: return semanticParseLIST();
        case LOAD_MATRIX: return semanticParseLOAD();
        case LOAD: return semanticParseLOAD();
        case PRINT_MATRIX: return semanticParsePRINT();
        case PRINT: return semanticParsePRINT();
        case PROJECTION: return semanticParsePROJECTION();
        case RENAME: return semanticParseRENAME();
        case SELECTION: return semanticParseSELECTION();
        case SORT: return semanticParseSORT();
        case SOURCE: return semanticParseSOURCE();
        default: cout<<"SEMANTIC ERROR"<<endl;
    }

    return false;
}