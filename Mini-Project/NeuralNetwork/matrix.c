/*
 * matrix.c
 *
 * Library containing matrix creation and manipulation functions.
 * 
 * Created on: 15 April 2022
 *      Author: Thariq Fahry
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"


/*
 * Functions in this library all use pointers to structs of type Matrix. This
 * was done in order to reduce memory leaks, since a matrix operation can free
 * the memory used by the operand matrices once it is done with the calculation.
*/


Matrix newmat(int rows, int cols, bool fill_random, float fill_value) {
    int i, j;
    Matrix mat;

    if(fill_random && (fill_value<=0)) printf("ERROR: fill_value must be > 0 if fill_random is true.\n");

    mat.rows = rows; mat.cols = cols;
    mat.elements = calloc(rows * cols, sizeof(float));

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            if (fill_random) mat.elements[i * cols + j] = ((float)(rand() % ((int)fill_value*10)))/10 - fill_value/2;
            else mat.elements[i * cols + j] = fill_value;
        }
    }
    return mat;
}

Matrix loadmat(float* raw_array, int rows, int cols){
    Matrix mat;

    mat.rows = rows; mat.cols = cols;
    mat.elements = raw_array;
    return mat;
}


float* e(Matrix* mat, int row, int col) {
    return &(mat->elements[row * mat->cols + col]);
}

void printmat(Matrix* mat, bool compact) {
	int i, j;
	printf("----%dx%d Matrix----------\n",mat->rows,mat->cols);
    for (i = 0; i < mat->rows; i++) {
        for (j = 0; j < mat->cols; j++) {
            if (compact) printf( "%2d", (int)rintf(*e(mat, i, j)*10) );
            else printf("%7.3f", *e(mat, i, j));
        }
        printf("\n");
    }
}

void matadd(Matrix* mata, Matrix* matb, Matrix* matresult) {
    int i;

    Matrix mattemp;
    Matrix* mattemp_p = &mattemp;

    if (!((mata->rows == matb->rows) && (mata->cols == matb->cols))) {
        printf("Matrices cannot be added. mata is %ix%i and matb is %ix%i.\n", mata->rows, mata->cols, matb->rows, matb->cols);
        return;
    }

    mattemp_p->rows = mata->rows;
    mattemp_p->cols = mata->cols;
    mattemp_p->elements = calloc(mattemp_p->rows * mattemp_p->cols, sizeof(float));

    for (i = 0; i < mata->rows * mata->cols; i++) {
        *e(mattemp_p, 0, i) = *e(mata, 0, i) + *e(matb, 0, i);
    }

    matresult->rows = mattemp_p->rows;
    matresult->cols = mattemp_p->cols;
    free(matresult->elements);
    matresult->elements = mattemp_p->elements;
}

void matmul(Matrix* mata, Matrix* matb, Matrix* matresult) {
    int i, j, k;
    float temp;

    Matrix mattemp;
    Matrix* mattemp_p = &mattemp;

    if (!(mata->cols == matb->rows)) {
        printf("Matrices cannot be multiplied. mata has %i columns and matb has %i rows.\n", mata->cols, matb->rows);
        return;
    }

    mattemp_p->rows = mata->rows;
    mattemp_p->cols = matb->cols;
    mattemp_p->elements = calloc(mattemp_p->rows * mattemp_p->cols, sizeof(float));

    for (i = 0; i < mata->rows; i++) {
        for (j = 0; j < matb->cols; j++) {
            temp = 0;
            for (k = 0; k < mata->cols; k++) {
                temp += *e(mata, i, k) * (*e(matb, k, j));
            }
            *e(mattemp_p, i, j) = temp;
        }
    }

    matresult->rows = mattemp_p->rows;
    matresult->cols = mattemp_p->cols;
    free(matresult->elements);
    matresult->elements = mattemp_p->elements;
}


void set_region(Matrix* mat, int rowleft, int coltop, int height, int width, float value) {
    int i, j;

    for (i = 0; i < height; i++) {
        for (j = 0; i < width; i++) {

            if ((rowleft+i < mat->rows-1) && (coltop + i < mat->cols-1)){
            *e(mat, rowleft + i, coltop + j) = value;
            }
        }
    }
}
