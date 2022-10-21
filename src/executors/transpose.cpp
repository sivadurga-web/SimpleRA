#include "global.h"
/**
 * @brief 
 * SYNTAX: Transpose two matrices
 */
bool syntacticParseCROSS_TRANSPOSE()
{
    logger.log("syntacticParseCROSS_TRANSPOSE");
    if (tokenizedQuery.size() != 3) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    
    parsedQuery.queryType = CROSS_TRANSPOSE;
    parsedQuery.crossTransposeFirstRelationName = "MAT_" + tokenizedQuery[1];
    parsedQuery.crossTransposeSecondRelationName = "MAT_" + tokenizedQuery[2];
    parsedQuery.printType = "MATRIX";
    return true;
}

bool semanticParseCROSS_TRANSPOSE()
{
    logger.log("semanticParseCROSS_TRANSPOSE");
    cout << parsedQuery.crossSecondRelationName << endl;
    if (!tableCatalogue.isTable(parsedQuery.crossTransposeFirstRelationName))
    {
        cout << "SEMANTIC ERROR: Matrix 1 doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.crossTransposeSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Matrix 2 file doesn't exist" << endl;
        return false;
    }
    Table* table1 = tableCatalogue.getTable(parsedQuery.crossTransposeFirstRelationName);
    Table* table2 = tableCatalogue.getTable(parsedQuery.crossTransposeSecondRelationName);
    if (table1-> columnCount != table1->rowCount || table2->columnCount != table2->rowCount) {
        cout << "SEMANTIC ERROR: Matrix/Matrices not square" << endl;
        return false;
    }
    if (table1->columnCount != table2->columnCount) {
        cout << "SEMANTIC ERROR: Dimensions are not matching" << endl;
        return false;
    }

    return true;
}

void executeCROSS_TRANSPOSE()
{
    logger.log("executeCROSS_TRANSPOSE");

    Table* table1 = tableCatalogue.getTable(parsedQuery.crossTransposeFirstRelationName);
    Table* table2 = tableCatalogue.getTable(parsedQuery.crossTransposeSecondRelationName);
    if (table1->tableName == table2->tableName) {
        Cursor cursor1 = table1->getCursor();
        Cursor cursor2 = table2->getCursor();
        long long i = 0, j = 0, block_num1 = 0, index1 = 0, block_num2 = 0, index2 = 0;
        vector<int> b1 = cursor1.getBlock(0), b2;
        
        while (i < table1->rowCount ) {
            block_num1 = (i*table1->columnCount+j)/table1->maxElements;
            index1     = (i*table1->columnCount+j)%table1->maxElements;
            block_num2 = (j*table1->columnCount+i)/table1->maxElements;
            index2     = (j*table1->columnCount+i)%table1->maxElements;
            b1 = cursor1.getBlock(block_num1);
            if (block_num1 != block_num2) {
                b2 = cursor2.getBlock(block_num2);
                
                swap(b1[index1], b2[index2]);
                bufferManager.deleteFile(table2->tableName, cursor2.pageIndex);
                bufferManager.clear();
                bufferManager.writeMatrixPage(table2->tableName, cursor2.pageIndex, b2);
            }
            else swap(b1[index1], b1[index2]);
            bufferManager.deleteFile(table1->tableName, cursor1.pageIndex);
            bufferManager.clear();
            bufferManager.writeMatrixPage(table1->tableName, cursor1.pageIndex, b1);

            j++;
            if (j == i+1) {
                i++;
                j=0;
            }
        }
        bufferManager.deleteFile(table1->tableName, cursor1.pageIndex);

        bufferManager.writeMatrixPage(table1->tableName, cursor1.pageIndex,b1);
    }
    else {
        Cursor cursor1 = table1->getCursor();
        Cursor cursor2 = table2->getCursor();
        long long i = 0, j = 0, block_num1 = 0, index1 = 0, block_num2 = 0, index2 = 0;
        vector<int> b1 = cursor1.getBlock(0), b2;
        while (i < table1->rowCount ) {
            block_num1 = (i*table1->columnCount+j)/table1->maxElements;
            
            index1     = (i*table1->columnCount+j)%table1->maxElements;
            block_num2 = (j*table1->columnCount+i)/table1->maxElements;
            index2     = (j*table1->columnCount+i)%table1->maxElements;
            if (block_num1 != cursor1.pageIndex) {
                bufferManager.deleteFile(table1->tableName, cursor1.pageIndex);
                bufferManager.writeMatrixPage(table1->tableName, cursor1.pageIndex, b1);
                b1 = cursor1.getBlock(block_num1);
                
            }
            b2 = cursor2.getBlock(block_num2);
            swap(b1[index1], b2[index2]);

            bufferManager.deleteFile(table2->tableName, cursor2.pageIndex);
            bufferManager.clear();
            bufferManager.writeMatrixPage(table2->tableName, cursor2.pageIndex, b2);
            j++;
            if (j == table1->columnCount) {
                i++;
                j=0;
            }
        }
        bufferManager.deleteFile(table1->tableName, cursor1.pageIndex);

        bufferManager.writeMatrixPage(table1->tableName, cursor1.pageIndex,b1);
        
    }
    return;
}