#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD relation_name
 */
bool syntacticParseLOAD()
{
    logger.log("syntacticParseLOAD");
    if (tokenizedQuery.size() > 3 || tokenizedQuery.size() < 2) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    else if (tokenizedQuery.size() == 3  && tokenizedQuery[1] != "MATRIX") {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    else if (tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX") {
        parsedQuery.queryType = LOAD_MATRIX;
        parsedQuery.loadRelationName = "MAT_"+tokenizedQuery[2]; 
        parsedQuery.printType = "MATRIX";
    }
    else {
        parsedQuery.queryType = LOAD;
        parsedQuery.printType = "TABLE";
        parsedQuery.loadRelationName = tokenizedQuery[1];
    }
    return true;
}

bool semanticParseLOAD()
{
    logger.log("semanticParseLOAD");
    if (tableCatalogue.isTable(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Relation already exists" << endl;
        return false;
    }
    if (parsedQuery.queryType == LOAD_MATRIX) {
        if (!isFileExists(parsedQuery.loadRelationName.substr(4))) {
            cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
            return false;
        }   
        return true;
    }
    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOAD()
{
    logger.log("executeLOAD");

    Table *table = new Table(parsedQuery.loadRelationName);
    if (table->load())
    {
        tableCatalogue.insertTable(table);

        cout << "Loaded "<<parsedQuery.printType << ". Column Count: " << table->columnCount << " Row Count: " << table->rowCount << endl;
    }
    return;
}