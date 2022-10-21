#include "tableCatalogue.h"

using namespace std;

enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    GROUPBY,
    EXPORT,
    INDEX,
    JOIN,
    LIST,
    LOAD,
    PRINT,
    PROJECTION,
    RENAME,
    SELECTION,
    SORT,
    SOURCE,
    LOAD_MATRIX,
    PRINT_MATRIX,
    EXPORT_MATRIX,
    CROSS_TRANSPOSE,
    UNDETERMINED
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

enum GroupbyType
{
    MAX,
    MIN,
    SUM, 
    AVG,
    NO_GROUPBY_CLAUSE
};

enum JoinType {
    NESTED,
    PARTHASH,
    NO_JOIN_TYPE
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";
    string crossTransposeFirstRelationName = ""; //
    string crossTransposeSecondRelationName= "";//
    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    string groupbyResultRelationName = "";
    string groupbyGroupAttribute = "";
    string groupbyRelationName = "";
    GroupbyType groupbyOP = NO_GROUPBY_CLAUSE;
    string groupbyRetAttribute = "";


    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";
    int joinBufferSize = 0;
    JoinType joinType = NO_JOIN_TYPE;

    string loadRelationName = "";

    string printRelationName = "";
    string printType = ""; // recently added
    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
    string sortResultRelationName = "";
    string sortColumnName = "";
    string sortRelationName = "";

    string sourceFileName = "";

    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticparseGROUPBY();
bool syntacticParseEXPORT();
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParsePRINT();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseSOURCE();
bool syntacticParseCROSS_TRANSPOSE();
bool isFileExists(string tableName);
bool isQueryFile(string fileName);
