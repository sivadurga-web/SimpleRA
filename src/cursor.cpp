#include "global.h"

Cursor::Cursor(string tableName, int pageIndex)
{
    logger.log("Cursor::Cursor");
    this->page = bufferManager.getPage(tableName, pageIndex);
    this->pagePointer = 0;
    this->tableName = tableName;
    this->pageIndex = pageIndex;
}

/**
 * @brief This function reads the next row from the page. The index of the
 * current row read from the page is indicated by the pagePointer(points to row
 * in page the cursor is pointing to).
 *
 * @return vector<int> 
 */
vector<int> Cursor::getNext()
{
    logger.log("Cursor::getNext");
    vector<int> result = this->page.getRow(this->pagePointer);
    this->pagePointer++;
    if(result.empty()){
        tableCatalogue.getTable(this->tableName)->getNextPage(this);
        if(!this->pagePointer){
            result = this->page.getRow(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}
/**
 * @brief Function that loads Page indicated by pageIndex. Now the cursor starts
 * reading from the new page.
 *
 * @param pageIndex 
 */
void Cursor::nextPage(int pageIndex)
{
    logger.log("Cursor::nextPage");
    this->page = bufferManager.getPage(this->tableName, pageIndex);
    this->pageIndex = pageIndex;
    this->pagePointer = 0;
}

vector<int> Cursor::getMatNext(int row,int count, int maxElements, int columnCount)
{
    logger.log("Cursor::getMatNext");
    int pageIndex = ((row)*columnCount)/maxElements;
    int ind       = (row*columnCount)%maxElements;
    // if ((row*columnCount)%maxElements ==0) {
    //     pageIndex
    // }
    this->nextMatPage(pageIndex);
    vector<int> result = this->page.getMatRow(this->pagePointer);
    vector<int> ans;
    while (ans.size() < count) {
        ans.push_back(result[ind]);
        ind++;
        if (ind == maxElements) {
            ind = 0;
            this->nextMatPage(this->pageIndex+1);
            result = this->page.getMatRow(this->pagePointer);
        }
    }
    return ans;
}

void Cursor::nextMatPage(int pageIndex)
{
    logger.log("Cursor::nextMatPage");
    this->page = bufferManager.getPage(this->tableName, pageIndex);
    this->pageIndex = pageIndex;
    this->pagePointer = 0;
}

vector<int> Cursor::getBlock(int pageIndex) {
    logger.log("Cursor::getMatNext");
    this->nextMatPage(pageIndex);
    vector<int> result = this->page.getMatRow(this->pagePointer);
    return result;
}