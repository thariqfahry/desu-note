/*
 * neuralnet.c
 *
 * Library containing an interface for inference of a pre-trained feedforward 
 * artificial neural network, and neural network utility funcions.
 *
 * The current network in weights.h was trained on the Extended MNIST ByMerge
 * dataset, to recognize handwritten characters from 0-19 and A-Z.
 * https://www.nist.gov/itl/products-and-services/emnist-dataset
 * 
 * Created on: 15 April 2022
 *      Author: Thariq Fahry
 * 
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "neuralnet.h"
#include "weights.h"
#include "../HelperFunctions/HelperFunctions.h"

// Gaussian convolution kernel calculated using Python.
float _gauss[3][3] = 
{
{0.0111, 0.0833, 0.0111},
{0.0833, 0.6223, 0.0833},
{0.0111, 0.0833, 0.0111},
};

// Mapping of neuron argmaxes in the final layer to the character they represent.
// Taken from emnist-bymerge-mapping.txt from the EMNIST dataset.
int emnistmap[47] = 
{48,49,50,51,52,53,54,55,56,57,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,97,98,100,101,102,103,104,110,113,114,116};

Matrix infer(Matrix* inf_vec) {

    // Load the weight matrices of the pre-trained network. These constants
    // are defined in weights.h, and represent the current EMNIST_ByMerge-trained
    // handwriting recognition neural network.
    Matrix l1w = loadmat((float*)_l1w, 784, 256);
    Matrix l1b = loadmat((float*)_l1b, 1, 256);
    Matrix l2w = loadmat((float*)_l2w, 256, 128);
    Matrix l2b = loadmat((float*)_l2b, 1, 128);
    Matrix l3w = loadmat((float*)_l3w, 128, 47);
    Matrix l3b = loadmat((float*)_l3b, 1, 47);

    Matrix iv_f;                                    // Flattened inference vector.
    Matrix signal = newmat(1, 1, false, 0);         // Signal in each layer.

    // Only accept a 28x28 inference vector.
    if (inf_vec->rows * inf_vec->cols != 784) {
        printf("Inference vector must have 784 elements, not %d.\n", inf_vec->rows * inf_vec->cols);
        return signal;
    }

    // Flatten the inference vector to have just 1 row, since the neural 
    // network was trained on and can only take 1-D data.
    iv_f.rows = 1;
    iv_f.cols = inf_vec->rows * inf_vec->cols;
    iv_f.elements = inf_vec->elements;

    // Calculate the signal in layer 1. The 'signal' matrix is reused between
    // each operation as this avoids memory leaks by freeing up the result of 
    // the previous operation each time.
    matmul(&iv_f, &l1w, &signal);
    matadd(&signal, &l1b, &signal);
    relu(&signal);

    // Calculate the signal in layer 2.
    matmul(&signal, &l2w, &signal);
    matadd(&signal, &l2b, &signal);
    relu(&signal);

    // Calculate the signal in layer 3.
    matmul(&signal, &l3w, &signal);
    matadd(&signal, &l3b, &signal);

    // Apply a softmax to Layer 3's output to convert it to a probability
    // distribution.
    softmax(&signal);

    return signal;
}


void relu(Matrix* mat) {
    int i;

    for (i = 0; i < mat->rows * mat->cols; i++) {
        *e(mat, 0, i) = fmaxf(0, *e(mat, 0, i));
    }
}


void softmax(Matrix* mat) {
    int i;
    float sum=0;
    for (i = 0; i < mat->rows*mat->cols; i++){
        *e(mat, 0, i) = exp(*e(mat, 0, i));
    }
    for (i = 0; i < mat->rows*mat->cols; i++){
        sum+=*e(mat, 0, i);
    }
    for (i = 0; i < mat->rows*mat->cols; i++){
        *e(mat, 0, i) = *e(mat, 0, i)/sum;
    }
}


int argmax(Matrix* mat) {
    int i, _argmax;
    float tempmax;

    tempmax = *e(mat, 0, 0);
    _argmax = 0;

    for (i = 0; i < mat->cols * mat->rows; i++) {
        if (*e(mat, 0, i) > tempmax) {
            tempmax = *e(mat, 0, i);
            _argmax = i;
        };
    }

    return _argmax;
}


char argmax2char(int _argmax){
    return (char)emnistmap[_argmax];
}


void convolve(Matrix* mat, Matrix* kernel, Matrix* matresult) {
    int i, j, a, b, edge_size, kernel_origin;
    float accum;

    Matrix mattemp;
    Matrix* mattemp_p = &mattemp;

    // mattemp_p is the output matrix. 
    mattemp_p->rows = mat->rows;
    mattemp_p->cols = mat->cols;
    mattemp_p->elements = calloc(mattemp_p->rows * mattemp_p->cols, sizeof(float));

    // Assume the kernel is symmetric (kernel->rows == kernel->cols).
    edge_size = (kernel->rows)/2;
    kernel_origin = edge_size;

    for (i = 0; i < mat->rows; i++) {
        for (j = 0; j < mat->cols; j++) {

            //Initialise the accumulator to 0 for each matrix element.
            accum = 0;

            for (a = 0; a < kernel->rows; a++) {
                for (b = 0; b < kernel->cols; b++) {

                    // If any of the neighboring pixels for this pixel's 
                    // convolution fall outside the  boundaries of mat,
                    // treat those pixels as if they had the same value as 
                    // the pixel being convoluted.
                    if ( ((i+a-kernel_origin)<0) || ((i+a-kernel_origin)>(mat->rows-1)) || ((j+b-kernel_origin)<0) || ((j+b-kernel_origin)>(mat->cols-1)) )
                    {
                        accum +=  *e(kernel, a,b) * (*e(mat, i, j));
                    }

                    // Otherwise, do the convolution operation as usual and
                    // calculate the value of the accumulator.
                    else
                    {
                        accum += *e(kernel, a,b)*(*e(mat, i+(a-kernel_origin), j+(b-kernel_origin)));
                    }

                }
            }

            // Finally, set the value of the respective element in the
            // output matrix to the value of the accumulator.
            *e(mattemp_p, i, j) = accum;
        }
    }

    // Overwrite matresult's dimensions with the dimensions of the output
    // matrix from this operation.
    matresult->rows = mattemp_p->rows;
    matresult->cols = mattemp_p->cols;

    // Free matresult's old elements and replace them with those of the output
    // matrix.
    free(matresult->elements);
    matresult->elements = mattemp_p->elements;
}

void gaussian(Matrix* mat){
    Matrix gauss = loadmat((float*)_gauss, 3,3);
    convolve(mat, &gauss, mat);
}
