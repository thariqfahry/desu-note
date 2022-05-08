/*
 * matrix.h
 *
 * Library containing matrix creation and manipulation functions.
 * 
 * Created on: 15 April 2022
 *      Author: Thariq Fahry
 * 
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdbool.h>

/**
 * Matrix struct
 *
 * Main struct used in the Matrix library. Contains the number of rows and
 * columns in the matrix, and a pointer to float array containing rows*columns
 * elements.
 *
 */
typedef struct {
    int rows;
    int cols;
    float* elements;
} Matrix;

/**
 * newmat
 *
 * Create and allocate memory for a new matrix with rows rows and cols columns.
 *
 * Inputs:
 *      rows: The number of rows in the matrix.
 *      cols: The number of columns in the matrix.
 *      fill_random: Whether to fill the matrix with random data. If true, the 
 *           matrix will be filled with random numbers from 0 to fill_value. If 
 *           false, the matrix will be filled with fill_value.
 *      fill_value: The number that is either the max random value, or the number
 *      to fill the matrix with, depending on fill_random.
 * 
 * Outputs:
 *      A pointer to the newly created Matrix struct.
 * 
 */
Matrix newmat(int rows, int cols, bool fill_random, float fill_value);

/**
 * loadmat
 *
 * Initialise a matrix struct from an existing array.
 * 
 * Inputs:
 *      raw_array: A pointer to an existing floating-point array containing 
 *          rows*cols elements.
 *      rows: The number of rows in the matrix.
 *      cols: The number of columns in the matrix.
 * 
 * Outputs:
 *      A pointer to the Matrix struct created from raw_array.
 * 
 */
Matrix loadmat(float* raw_array, int rows, int cols);

/**
 * e = "Element". Shortened name due to frequent use.
 *
 * Access element[row, col] of the matrix mat.
 * 
 * Inputs:
 *      mat: A matrix struct.
 *      row: The row number of the element to access.
 *      col: The column number of the element to access.
 * 
 * Outputs:
 *      A pointer to element row, col in the matrix mat.
 * 
 */
float* e(Matrix* mat, int row, int col);

/**
 * printmat
 *
 * Neatly print a matrix to stdout.
 * 
 * Inputs:
 *      mat: The matrix to print.
 *      compact: If true, the matrix will be printed in compact form where
 *      each element takes up a maxumum of 3 characters, useful for large
 *      matrices. If false, each elemetn will be printed to .3f floating
 *      point precision.
 * 
 */
void printmat(Matrix* mat, bool compact);

/**
 * matadd
 *
 * Adds two matrices element-wise. Both matrices must have the same dimension.
 * 
 * Inputs:
 *      mata: Matrix to add.
 *      matb: Matrix to add.
 *      
 *      matresult: Matrix to store the result in. Will have the same dimension
 *      as the input matrices.
 * 
 */
void matadd(Matrix* mata, Matrix* matb, Matrix* matresult);

/**
 * matmul
 *
 * Multiply two matrices. mata must have the same number of columns as 
 * matb's rows.
 * 
 * Inputs:
 *      mata: The multiplicand matrix.
 *      matb: The multiplier matrix.
 *      
 *      matresult: Matrix to store the result in. Will have the same number
 *      of rows as mata, and the same number of columns as matb.
 * 
 */
void matmul(Matrix* mata, Matrix* matb, Matrix* matresult);

/**
 * set_region
 *
 * Sets a sub-region of matrix mat to a value.
 * 
 * Inputs:
 *      mata: Matrix to set the sub-region on.
 *      rowleft: Left edte of the sub-region.
 *      coltop: Top edge of the sub-region.
 *      height: Number of rows in the sub-region,
 *      width: Number of columns in the sub-region.
 *      value: Value to set each element in the sub-region to.
 * 
 */
void set_region(Matrix* mat, int rowleft, int coltop, int height, int width, float value);

#endif
