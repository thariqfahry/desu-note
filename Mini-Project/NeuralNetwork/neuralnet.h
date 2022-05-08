/*
 * neuralnet.h
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
#ifndef NEURALNET_H_
#define NEURALNET_H_

#include "matrix.h"

/**
 * infer
 *
 * Calculates the inference vector of the 28x28 matrix inf_vec.
 * 
 * Currrently implemented to calculate the inference vector of a 28x28
 * handwritten character using a network trained on the EMNIST dataset.
 *
 * The inference vector is expected to have values from -0.5 to +0.5,
 * representing pixel intensity.
 * 
 * Inputs:
 *      inf_vec: The inference vector.
 *
 * Outputs:
 *      A 1xN probability vector containing a probability distribution of the
 *      probability of each class, where N is the number of possible classes.
 *      Sums to 1. Currently implemented to output a 1x47 vector representing
 *      the EMNIST_ByMerge classes, whose mapping is given in emnistmap[47]
 *      in neuralnet.c.
 */
Matrix infer(Matrix* inf_vec);

/**
 * relu
 *
 * Calculates the Rectified Linear Unit activation max(0,x) on each element
 * of the input matrix.
 * 
 * Inputs:
 *      mat: The input matrix, which the result will also be stored in.
 * 
 */
void relu(Matrix* mat);

/**
 * softmax
 *
 * Calculates the softmax function exp(mat)/sum(exp(mat)) for the input matrix.
 * 
 * Inputs:
 *      mat: The input matrix, which the result will also be stored in.
 * 
 */
void softmax(Matrix* mat);

/**
 * argmax
 *
 * Returns the index of the largest element in the input matrix.
 * 
 * Inputs:
 *      mat: The input matrix.
 * 
 * Outputs:
 *      Integer representing the index of the largest element in the matrix.
 * 
 */
int argmax(Matrix* mat);

/**
 * argmax2char
 *
 * Maps an argmax output from the current EMNIST-train neural network in 
 * the infer() function to the character it represents, based on emnistmap.
 * 
 * Inputs:
 *      _argmax: Integer representing the argmax'ed output from the infer
 *      function.
 * 
 * Outputs:
 *      Char that this integer maps to, based on emnistmap.
 * 
 */
char argmax2char(int _argmax);

/**
 * convolve
 *
 * Perform a convolution operation of kernel kernel over matrix mat.
 * When the convolution operation encounters pixels beyond the edge of the 
 * matrix mat, they are treated as having values of their nearest neighbours
 * that are within the matrix mat.
 * 
 * The kernel is assumed to be symmetric.
 * 
 * Inputs:
 *      mat: The matrix to perform convolution on.
 *      kernel: The convolution kernel.
 *      
 *      result: Matrix to store the result in. Will have the same dimension
 *      as mat.
 * 
 */
void convolve(Matrix* mat, Matrix* kernel, Matrix* result);

/**
 * gaussian
 *
 * Performs a gaussian convolution over matrix mat using the kernel
 * _gauss[3][3] defined in neuralnet.c.
 * 
 * Inputs:
 *      mat: The input matrix, which the result will also be stored in.
 * 
 */
void gaussian(Matrix* mat);

#endif
