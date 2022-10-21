# REPORT

## Part - 2

### Page Layout

The 2D matrix is stored in the form of a linear 1D array, the position of $matrix2D[i,j]$ (0-indexing) will be at $matrix1D[i* number\textunderscore of\textunderscore columns+j]$, and the 1D array is divided into blocks(pages) of size of $block\textunderscore size$, the maximum number of elements that can be stored in a block(page) is $\frac{block\textunderscore size}{sizeof(int)}$ . The page also stores the number of rows and number of columns in the matrix, as well as the maximum number of elements that a single block(page) can store.

### CROSS_TRANSPOSE operation

The CROSS_TRANSPOSE operation is implemented in the following way. 

Say, A, B are the two 2D square matrices of same dimension, loaded used LOAD MATRIX, that needs to be cross transposed. 

Suppose we want to transpose $A[i][j]$ (0-indexing) with $B[j][i]$,


 $Page\textunderscore index$ of  $A[i][j] =  \lfloor \frac{i*number Of Columns+j}{maxNoElements}\rfloor$ 

 $Index$ of $A[i][j]$ in that $Page\textunderscore index$ is $(i*number Of Columns+j)$ \% $maxNoElements$

 Similarly, we can get the $Page\textunderscore index$, $index$ in that page of $B[i][j]$. We swap the values at those indices by reading these two pages. Then we rewrite these pages into the $temp$ directory. By doing it for all elements of the matrix, we will get the CROSS_TRANSPOSE of the two matrices. For reducing the number of page writings, we iterated linearly in one matrix and write only when the elements in each page are completely transposed. For the other matrix, page rewriting is done for every swap. 

 ## Part - 3
## A - Page Layout and Compression technique for sparse matrix

As sparse matrix consists more zeros unlike the dense matrices, memory wastage can be avoided by storing non-zero elements as triplet(Row, Column, Value) in the pages. Storing in this way compresses the 2D matrix as the majority of sparse elements are zeroes, which we are not storing in the pages. The page layout is similar to the page layout used in part 2 with the only difference is that we store only non-zero elements along with their row, column instead of only element. Now, each page stores only non-zero elements and the number of elements that can be stored in a page is one third of max number of elements that the block stored in part - 2.

## B - Tranpose of sparse matrix

A 2D array is used to represent a sparse matrix in which there are three rows named as 
Row: Index of the row, where a non-zero element is located
Column: Index of the column, where the non-zero element is located.
Value: Value of the non-zero element located at index – (row, column).
(b) To find the transpose, we can go through each block and swap the values at row, column of each triplet. First we will  store all the elements that are needs to be filled in the form of a map or some indexing, then for each position that needs to be filled, we will naively search through all the pages to know the element that needs to be stored at that page. The other way for doing this after swapping the row, columns is doing sorting the triplets based on the updated rows and columns based on rows first and then columns using some naive sorting algorithm, where only two blocks are loaded in buffer manager at any time. The time complexity for doing this is O(N^2) where N is the number of non-zero elements of sparse matrix. 
