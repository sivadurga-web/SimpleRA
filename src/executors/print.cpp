#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT relation_name
 */
bool syntacticParsePRINT()
{
    logger.log("syntacticParsePRINT");
    if (tokenizedQuery.size() > 3 || tokenizedQuery.size() < 2) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    else if (tokenizedQuery.size() == 3  && tokenizedQuery[1] != "MATRIX") {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    else if (tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX") {
        parsedQuery.queryType = PRINT_MATRIX;
        parsedQuery.printRelationName = "MAT_"+tokenizedQuery[2]; 
        parsedQuery.printType = "MATRIX";
    }
    else {
        parsedQuery.queryType = PRINT;
        parsedQuery.printType = "TABLE";
        parsedQuery.printRelationName = tokenizedQuery[1];
    }
    return true;
}

bool semanticParsePRINT()
{
    logger.log("semanticParsePRINT");
    if (!tableCatalogue.isTable(parsedQuery.printRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    return true;
}

void executePRINT()
{
    logger.log("executePRINT");
    Table* table = tableCatalogue.getTable(parsedQuery.printRelationName);
    if (parsedQuery.queryType == PRINT_MATRIX) 
        table->matPrint();
    else 
        table->print();
    return;
}
