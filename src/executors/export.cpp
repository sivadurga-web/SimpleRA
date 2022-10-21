#include "global.h"

/**
 * @brief 
 * SYNTAX: EXPORT <relation_name> 
 */

bool syntacticParseEXPORT()
{
    logger.log("syntacticParseEXPORT");
    if (tokenizedQuery.size() > 3 || tokenizedQuery.size() < 2) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    else if (tokenizedQuery.size() == 3  && tokenizedQuery[1] != "MATRIX") {
        cout << "SYNTAX ERROR" << endl;
    }
    else if (tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX") {
        parsedQuery.queryType = EXPORT_MATRIX;
        parsedQuery.exportRelationName = "MAT_"+tokenizedQuery[2]; 
        parsedQuery.printType = "MATRIX";
    }
    else {
        parsedQuery.queryType = EXPORT;
        parsedQuery.printType = "TABLE";
        parsedQuery.exportRelationName = tokenizedQuery[1];
    }
    return true;
}

bool semanticParseEXPORT()
{
    logger.log("semanticParseEXPORT");
    //Table should exist
    if (tableCatalogue.isTable(parsedQuery.exportRelationName))
        return true;
    cout << "SEMANTIC ERROR: No such relation exists" << endl;
    return false;
}

void executeEXPORT()
{
    logger.log("executeEXPORT");
    Table* table = tableCatalogue.getTable(parsedQuery.exportRelationName);
    if (parsedQuery.queryType == EXPORT_MATRIX) table->makeMatPermanent();
    else
    table->makePermanent();
    return;
}