#
# Code to train the EMNIST neural network.
# Author: Thariq Fahry
#
import numpy as np, tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Flatten, Lambda
from tensorflow.keras.utils import to_categorical
from tensorflow.keras.layers.experimental.preprocessing import Rescaling
    
#%%
import idx2numpy
import numpy as np
train_images = idx2numpy.convert_from_file("emnist/emnist-bymerge-train-images-idx3-ubyte")
train_labels = idx2numpy.convert_from_file("emnist/emnist-bymerge-train-labels-idx1-ubyte")

test_images = idx2numpy.convert_from_file("emnist/emnist-bymerge-test-images-idx3-ubyte")
test_labels = idx2numpy.convert_from_file("emnist/emnist-bymerge-test-labels-idx1-ubyte")


train_images = np.array([slc.T for slc in train_images])
test_images = np.array([slc.T for slc in test_images])
#%%

num_classes = to_categorical(test_labels).shape[1]

model = Sequential([
    Rescaling(scale=1/255, offset=-.5),
    Flatten(input_shape=(28, 28)),
    Dense(256, activation='relu'),
    Dense(128, activation='relu'),
    Dense(num_classes, activation = 'softmax'),
])

model.compile(optimizer='sgd' ,loss='sparse_categorical_crossentropy')
model.fit(train_images,to_categorical(train_labels),)
model.evaluate(test_images,to_categorical(test_labels))
model.save_weights('emnist.h5')

#%% 
def relu(x):
    return np.amax((x, np.zeros(x.shape)), 
            axis=0)


def infer(_i, _w):
    # [(784, 256), (256,), (256, 128), (128,), (128, 47), (47,)]
    
    i =_i.astype(np.float32)
    w = [_.astype(np.float32) for _ in _w]
    
    i = (i / 255) - 0.5
    
    # https://keras.io/api/layers/core_layers/dense/
    fl = relu(np.dot(i, w[0])   + w[1])
    sl = relu(np.dot(fl,w[2])  + w[3])
    tl = np.dot(sl,     w[4])       + w[5]
    
    return tl

#infer(model.get_weights(), test_images[1])

#%%
import matplotlib.pyplot as plt

with open ("emnist/emnist-bymerge-mapping.txt") as f:
    m = f.readlines()
    
m = [_.strip().split() for _ in m]
m = {int(key):int(value) for key,value in m}

plt.imshow(test_images[0])

chr(m[np.argmax(model.predict(test_images[1].reshape(-1,784)))])
chr(m[np.argmax(msm(infer(test_images[1].reshape(-1,784), model.get_weights())))])

#%%
#!cd C:\UsersThariq_\Workspace\ELEC5620M-Mini-Project-thariqfahry\Mini-Project\CustomProjectDrivers\HelperFunctions

import io
FILENAME_=  "weights"

def write_carray(filename, array, variable_name, overwrite = False):
    assert type(array) is np.ndarray
    s = io.BytesIO()
    
    fileheader = f'#include "{FILENAME_}.h"\n' if overwrite else ''
    dimensions = "".join([f"[{dim}]" for dim in np.shape(array)])
    
    if len(array.shape) == 1:
        np.savetxt(s, array, fmt = "%12.8f",delimiter=",",newline=",")
        s = \
        fileheader + "\n\nfloat "+ variable_name + dimensions +"={\n" + \
        s.getvalue().decode()[:-1] + "\n};\n"
        
    else:
        np.savetxt(s, array, fmt = "%12.8f",delimiter=",",newline="},\n{")
        s = \
        fileheader + "\n\nfloat "+ variable_name + dimensions +"={\n{" + \
        s.getvalue().decode()[:-1] + "};\n"
        
    with open(filename, "w" if overwrite else "a") as f:    
        f.write(s)


iv =  ((test_images[1] / 255) - 0.5).reshape(784,)

write_carray(f"{FILENAME_}.c", iv, "_iv", True)

variable_names = ["_l1w","_l1b","_l2w","_l2b","_l3w","_l3b"]
for array, variable_name in zip(model.get_weights(), variable_names):
    write_carray(f"{FILENAME_}.c", array, variable_name)


headerheader = """
#ifndef WEIGHTS_H
#define WEIGHTS_H

"""

with open(f"{FILENAME_}.h", "w") as f:
    f.write(headerheader)
    
    f.write("extern float _iv[784];\n\n")
    
    for array, variable_name in zip(model.get_weights(), variable_names):
        dimensions = "".join([f"[{dim}]" for dim in np.shape(array)])
        f.write("extern float "+variable_name+dimensions+";\n\n")
    
    f.write("#endif\n")
    