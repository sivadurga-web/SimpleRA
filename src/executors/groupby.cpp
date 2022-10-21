#include "global.h"

/**
 * @brief 
 * SYNTAX: R <- GROUP BY attribute FROM table_name RETURN MAX|MIN|SUM|AVG(attribute)
 */
bool syntacticparseGROUPBY() 
{
    logger.log("syntacticParseGROUPBY");
    if (tokenizedQuery.size() < 9) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    if (tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM" || tokenizedQuery[7] != "RETURN") {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    string last_ele = tokenizedQuery[8];
    if (last_ele.size() <=4) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    string op = last_ele.substr(0,3);
    if (op == "MAX") {
        parsedQuery.groupbyOP = MAX;
    }
    else if (op == "MIN") {
        parsedQuery.groupbyOP = MIN;
    }
    else if (op == "SUM") {
        parsedQuery.groupbyOP = SUM;
    }
    else if (op == "AVG") {
        parsedQuery.groupbyOP = AVG;
    }
    else {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    string attr = last_ele.substr(3);
    if (attr.length() <=2) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    if (attr[0] != '('  || attr[attr.length()-1] != ')') {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = GROUPBY;
    parsedQuery.groupbyResultRelationName = tokenizedQuery[0];
    parsedQuery.groupbyGroupAttribute = tokenizedQuery[4];
    parsedQuery.groupbyRelationName = tokenizedQuery[6];
    parsedQuery.groupbyRetAttribute = attr.substr(1,attr.length()-2);
    return true;
}

bool semanticParseGROUPBY()
{
    logger.log("semanticParseGROUPBY");
    //The resultant table shouldn't exist and the table argument should
    if (tableCatalogue.isTable(parsedQuery.groupbyResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.groupbyRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    
    if(!tableCatalogue.isColumnFromTable(parsedQuery.groupbyGroupAttribute, parsedQuery.groupbyRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.groupbyRetAttribute, parsedQuery.groupbyRelationName)) {
        cout << "SEMANTIC ERROR: Attribute doesn't exist" << endl;
        return false;
    }
    return true;
}

int getGrpValue(int x, int y, GroupbyType type) {
    if (type == MAX) return max(x,y);
    if (type == MIN) return min(x,y);
    return x+y;
}

void executeGROUPBY()
{
    logger.log("executeGROUPBY");
    Table table = *(tableCatalogue.getTable(parsedQuery.groupbyRelationName));
    unordered_map<int,pair<int,int>> mp;
    
    // get the indices of the grouping attribute and return attribute
    vector<string> columns = table.columns;
    int ret_att_index = 0, grp_att_index = 0;
    for (int i = 0; i < columns.size(); i++) {
        if (columns[i] == parsedQuery.groupbyGroupAttribute) {
            grp_att_index = i;
        }
        if (columns[i] == parsedQuery.groupbyRetAttribute) {
            ret_att_index = i;
        }
    }

    // preparing resultant columns
    vector<string> resColumns;
    resColumns.push_back(parsedQuery.groupbyGroupAttribute);
    GroupbyType op = parsedQuery.groupbyOP;
    if (op == MAX) resColumns.push_back("MAX"+parsedQuery.groupbyRetAttribute);
    else if (op == MIN) resColumns.push_back("MIN"+parsedQuery.groupbyRetAttribute);
    else if (op == AVG) resColumns.push_back("AVG"+parsedQuery.groupbyRetAttribute);
    else if (op == SUM) resColumns.push_back("SUM"+parsedQuery.groupbyRetAttribute);

    // Resultant table creation with column names
    Table *resRel = new Table(parsedQuery.groupbyResultRelationName, resColumns);
    Cursor  cursor = table.getCursor();
    vector<int> row = cursor.getNext();
    while (!row.empty()) {
        if (mp.find(row[grp_att_index]) == mp.end()) {
            mp[row[grp_att_index]] = {row[ret_att_index], 1};
        }
        else {
        mp[row[grp_att_index]].second++;
        mp[row[grp_att_index]].first = getGrpValue(row[ret_att_index], mp[row[grp_att_index]].first, op);
        }
      
        row = cursor.getNext();
    }
    vector<int> v(2);
    for (auto x:mp) {
        v[0] = x.first;
        if (op == AVG) v[1] = (x.second.first/x.second.second);
        else v[1] = x.second.first;
        resRel->writeRow<int>(v);
    }
    if (resRel->blockify()) {
        tableCatalogue.insertTable(resRel);
    }
    else {
        cout<<"Empty Table"<<endl;
        resRel->unload();
        delete resRel;
    }
    cout << "Number of Read Block Access: " << READ_ACCESS_COUNT << endl;
    cout << "Number of Write Block Access: " << WRITE_ACCESS_COUNT << endl;
    return;
}