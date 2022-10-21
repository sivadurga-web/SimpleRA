#include "global.h"
/**
 * @brief 
 * SYNTAX: R <- JOIN relation_name1, relation_name2 ON column_name1 bin_op column_name2
 */
bool printSyntaxError(){
    cout << "SYNTAX ERROR" << endl;
    return false;
}
bool syntacticParseJOIN()
{

    logger.log("syntacticParseJOIN");
    if (tokenizedQuery.size() != 13) return printSyntaxError();
    if (tokenizedQuery[3] != "USING") return printSyntaxError();
    if (tokenizedQuery[4] != "NESTED" && tokenizedQuery[4] != "PARTHASH") return printSyntaxError();
    if (tokenizedQuery[7] != "ON") return printSyntaxError();
    if (tokenizedQuery[11] != "BUFFER") return printSyntaxError();
    
    parsedQuery.queryType = JOIN;
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    parsedQuery.joinFirstRelationName = tokenizedQuery[5];
    parsedQuery.joinSecondRelationName = tokenizedQuery[6];
    parsedQuery.joinFirstColumnName = tokenizedQuery[8];
    parsedQuery.joinSecondColumnName = tokenizedQuery[10];
    parsedQuery.joinBufferSize = stoi(tokenizedQuery[12]);
    if (tokenizedQuery[4] == "NESTED")
        parsedQuery.joinType = NESTED;
    else 
        parsedQuery.joinType = PARTHASH;
    
    string binaryOperator = tokenizedQuery[9];
    if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.joinBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN()
{
    logger.log("semanticParseJOIN");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) || !tableCatalogue.isTable(parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

vector<string> getResultTableColumns(vector<string> & f_col, vector<string> &s_col, string f_name, string s_name) {
  vector<string> resColumns;
  int ind = 0;
  for (int i = 0; i < f_col.size();i++) {
    resColumns.push_back(f_col[i]);
    // resColumns.push_back(f_name + "_" + f_col[i]);
  }  
  for (int i = 0; i < s_col.size();i++) {
    resColumns.push_back(s_col[i]);
    // resColumns.push_back(s_name + "_" + s_col[i]);
  }
  return resColumns;
}
vector<int> getResultRow(vector<int> & f_col, vector<int> & s_col) {
    vector<int> resRow;
    for (auto x :f_col) resRow.push_back(x);
    for (auto x: s_col) resRow.push_back(x);
    return resRow;
}
bool toInsert(int x, int y, BinaryOperator op) {
    if (op == EQUAL) return x == y;
    if (op == NOT_EQUAL) return x!= y;
    if (op == GREATER_THAN) return x > y;
    if (op == LESS_THAN) return x < y;
    if (op == GEQ) return x >=y;
    if (op == LEQ) return x <= y;
    return false;
}

void nestedJoin() {
    // Read the two tables
    logger.log("nestedJoin");
    Table outerRel = *(tableCatalogue.getTable(parsedQuery.joinFirstRelationName));
    Table innerRel = *(tableCatalogue.getTable(parsedQuery.joinSecondRelationName));
    string outerColName = parsedQuery.joinFirstColumnName;
    string innerColName = parsedQuery.joinSecondColumnName;
    bool swapped = false;
    
    if (outerRel.rowCount > innerRel.rowCount) {
        swapped = true;
        swap(outerColName, innerColName);
        Table temp = outerRel;
        outerRel = innerRel;
        innerRel = temp;
    }
    // preparing the parameters
    int buff_size = parsedQuery.joinBufferSize;
    BinaryOperator op = parsedQuery.joinBinaryOperator;
    int outBlocks = buff_size -2;
    int numOfAccess = ceil((double)outerRel.blockCount/outBlocks);
    int curr_out_ind = 0, outerColInd = 0, innerColInd = 0;
    // Finding the indexes of the respective columns
    outerColInd = outerRel.getColumnIndex(outerColName);
    innerColInd = innerRel.getColumnIndex(innerColName);

    // Resultant table Creation
    string res_table_name = parsedQuery.joinResultRelationName;
    // Outerloop columns first based on query
    vector<string> resColumns;
    if (swapped) {
        resColumns = getResultTableColumns(innerRel.columns, outerRel.columns, innerRel.tableName, outerRel.tableName);
    }
    else {
        resColumns = getResultTableColumns(outerRel.columns, innerRel.columns, outerRel.tableName, innerRel.tableName);
    }
    Table *resRel = new Table(res_table_name, resColumns);
    Cursor cursor1 = outerRel.getCursor();
    vector<int> row;
    vector<int> outerRelRow;
    vector<int> innerRelRow;
    for (int outerLoop = 0; outerLoop < numOfAccess; outerLoop++) {
        vector<vector<int>> outerRows;
        for (int iter = 0; iter < outBlocks*outerRel.maxRowsPerBlock; iter++) {
            outerRelRow = cursor1.getNext();
            
            if (!outerRelRow.empty()) {
                outerRows.push_back(outerRelRow);
            }
            else break;
        }
        Cursor cursor2 = innerRel.getCursor();
        innerRelRow = cursor2.getNext();
        while (!innerRelRow.empty()) {
            for (auto outRow: outerRows) {
                bool flag = 0;
                if (swapped) 
                    flag = toInsert(innerRelRow[innerColInd], outRow[outerColInd], op);
                else 
                    flag = toInsert(outRow[outerColInd], innerRelRow[innerColInd],op);
                if (flag) {
                    if (swapped) 
                        row = getResultRow(innerRelRow, outRow);
                    else 
                        row = getResultRow(outRow, innerRelRow);
                    
                    resRel->writeRow<int>(row);
                }

            }
            innerRelRow = cursor2.getNext();
        }      
    }
    
    if (resRel->blockify()) {
    tableCatalogue.insertTable(resRel);
    }
    else {
        cout<<"Empty Table"<<endl;
        resRel->unload();
        delete resRel;
    }
    return;

}

vector<Table* > partition(Table table, int ind, int M) {
    int maxRowsPerBlock = table.maxRowsPerBlock;
    vector<Table* > tables(M, NULL);
    string table_part_name = table.tableName + "_PART_";
    vector<int> row(table.columnCount,0);
    // Read the rows in the table and start filling the buckets
    Cursor cursor = table.getCursor();
    row = cursor.getNext();
    while (!row.empty()) {
        int bucket_num = row[ind] % M;
        if (tables[bucket_num] == NULL){
            string table_name = table_part_name + to_string(bucket_num);
            tables[bucket_num] = new Table(table_name, table.columns);
        }
        tables[bucket_num]->writeRow(row);
        row = cursor.getNext();
    }
    for (int i = 0; i < M;i++){
        if (tables[i] != NULL && tables[i]->blockify()) {
            tableCatalogue.insertTable(tables[i]);
        }
    }
    return tables;
}

void parthashJoin() {
    logger.log("partitionHash");
    if (parsedQuery.joinBinaryOperator != EQUAL) {
        nestedJoin();
        return ;
    }
    // reading the tables
    Table table1 = *(tableCatalogue.getTable(parsedQuery.joinFirstRelationName));
    Table table2 = *(tableCatalogue.getTable(parsedQuery.joinSecondRelationName));
    int ind1 = table1.getColumnIndex(parsedQuery.joinFirstColumnName);
    int ind2 = table2.getColumnIndex(parsedQuery.joinSecondColumnName);
    int M = parsedQuery.joinBufferSize-1;
    // partitioning the table
    vector<Table*> part_table1 = partition(table1, ind1, M);
    vector<Table*> part_table2 = partition(table2, ind2, M);
    // joining the partitions using nested join with unordered map;
    vector<string> ResColumns = table1.columns;
    ResColumns.insert(ResColumns.end(), table2.columns.begin(), table2.columns.end());
    Table* ResRel = new Table(parsedQuery.joinResultRelationName, ResColumns);
    vector<int> ResRow;
    vector<int> row;
    for (int bucket_num = 0 ; bucket_num < M; bucket_num++) {
        int B = M-1, attr_ind1 = ind1, attr_ind2 = ind2;

        Table* table1_part = part_table1[bucket_num];

        Table* table2_part = part_table2[bucket_num];
        bool isSwapped = false;
        if (table1_part->blockCount > table2_part->blockCount) {
            swap(table1_part, table2_part);
            isSwapped = true;
            swap(attr_ind1, attr_ind2);
        }
        
        Cursor cursor_part1 = table1_part->getCursor();
        for (int i = 0; i < table1_part->blockCount; i+= B) {
            unordered_map<int, vector<vector<int>>> mp;
            for (int j = 0; j < (table1_part->maxRowsPerBlock)*B;j++) {

                row = cursor_part1.getNext();
                if (row.empty()) break;
                mp[row[attr_ind1]].push_back(row);
            }
            
            Cursor cursor_part2 = table2_part->getCursor();
            row = cursor_part2.getNext();
            while (!row.empty()) {
               
                for (auto x: mp[row[attr_ind2]]) {
                    if (isSwapped) {
                        ResRow.insert(ResRow.begin(), row.begin(), row.end());
                        ResRow.insert(ResRow.end(), x.begin(), x.end());
                    }
                    else {
                        ResRow.insert(ResRow.end(), x.begin(), x.end());
                        ResRow.insert(ResRow.end(), row.begin(), row.end());
                    }
                ResRel->writeRow<int>(ResRow);

                ResRow.clear();

                }

                row = cursor_part2.getNext();
            }
        }
    }
    if (ResRel->blockify()){
        tableCatalogue.insertTable(ResRel);
    }
    else {
        cout<<"Empty Table"<<endl;
        ResRel->unload();
        delete ResRel;
    }
    for (auto x: part_table1) 
    {
        tableCatalogue.deleteTable(x->tableName);
    }
    for (auto x: part_table2) 
        tableCatalogue.deleteTable(x->tableName);

    return;

}

void executeJOIN()
{
    logger.log("executeJOIN");
    if (parsedQuery.joinType == NESTED) nestedJoin();
    else if (parsedQuery.joinType == PARTHASH) parthashJoin();
    cout << "Number of Read Block Access: " << READ_ACCESS_COUNT << endl;
    cout << "Number of Write Block Access: " << WRITE_ACCESS_COUNT << endl;
    return;
}