/*
 * weights.h & weights.c
 *
 * Weights and biases of the current EMNIST-trained neural network. 
 * Generated using emnist.py.
 * 
 * Created on: 15 April 2022
 *      Author: Thariq Fahry
 * 
 */
#ifndef WEIGHTS_H
#define WEIGHTS_H

// Sample inference vector. Contains a handwritten 'M'.
extern float _iv[784];

// Layer 1 weights.
extern float _l1w[784][256];

// Layer 1 biases.
extern float _l1b[256];

// Layer 2 weights.
extern float _l2w[256][128];

// Layer 2 biases.
extern float _l2b[128];

// Layer 3 weights.
extern float _l3w[128][47];

// Layer 3 biases.
extern float _l3b[47];

#endif
