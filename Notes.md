201158825 Thariq Fahry
# Mini-Project
## Major TODOs
- All variable and function names (except e.g. matrix funcs) to Python style snake case
- Go through source files and do cleanup and TODOs
- Restructure directories; make neuralnet and Python directory

# Remaining tasks


- Unit tests (Rust-inspired), move 'M' inference vector here
- Comments
- Commits
- Report
- Presentation

## An MNIST digit recognizer
Basic scope:
- since TF is in C++ and cannot be compiled to the HPS, a FCFFNN needs to be reimplemented in C
- Touch screen driver library
    - README in driver folder
- FCFFNN library with basic params such as num layers, layer size and helper functions to load training data, infer, etc

Extended scope:
- Bigger model with EMNIST or Japanese MNIST
- More sophisticated NN library: convolution kernels, etc (this will crystallise upon reading)
- Hardware acceleration for touch screen
- Hardware acceleration for neural network operations
- Generalised touch screen driver completely decoupled from LT24

**Libs**  
https://lexborisov.github.io/Modest/  
https://stackoverflow.com/questions/9398650/use-a-html-renderer-in-an-embedded-environment

## Citations
ADC driver based on: https://www.topwaydisplay.com/en/blog/how-to-lcd-touch-screen
[^1]: https://www.analog.com/media/en/technical-documentation/application-notes/an-1021.pdf
[^2]: https://stackoverflow.com/questions/51124448/how-to-create-a-matrix-structure-in-c
FatFS copied from module Drivers directory and modified.
Private Timer copied from module Drivers directory and modified.
https://www.kindpng.com/imgv/iRxwmbT_transparent-kanna-kamui-png-png-download/

## Log

### Making a touch screen driver
- Online code arcane, manufacturer provided drivers bad
- Requires SPI peripheral (usually in FPGA) but can be bitbanged in software
- Datasheet has initialisation procedure
- Combined various driver code from internet
- Problem 1: Direction bits not properly set
    - Initially, control register not changing at all
        - Forgot to set direction bits
        - Set direction bits correctly
    - Control register now changes, but DOUT still just returns 1s 
    - TODO Check working by verifying BUSY goes high after input word and checking if identity returns
    - BUSY did return but identity didn't return; so the peripheral works up to the point of the input word but doesn't return correct output
    - in fact, identity also returns all 1s in DOUT; perhaps DOUT's direction bit is wrong?
        - It is; corrected
- Problem 2: Calibration & higher resolution
    - Used 12bit mode, verified with ident
    - Calibrated: cite calibration code [^1]
        - Accuracy still somewhat lacking
    - Obervational  test with handwriting
        - touchscreen error increases towards right side; TODO try differential signalling or multiiple calibrations, verifying with a handwriting test each time
    ~~- Calibration noise mitigation = accept/reject calibration~~
    - Datasheet says maybe reduce DCLK rate to reduce noise due to settling
        - Yes!
        - Calibration now results in perfect accuracy


- Problem 3: Better refresh rate
    - Unneeded; handwriting test confirmed this
    - If time available, look into continuous signalling and see if it
    changes anything


### Making a neural network inference library

- Exporting a trained model
    - Training the model
    - Exporting the weight matrix from Keras

- Reimplementing inference in C - just matrix multiplication
    - Checking that the C inference matches the Keras inference: 100 out-of-sample inference test
    - We want to use the smallest possible C floating point dtype for our matrix struct to reduce memory footprint; Python has arbitrary-precision floating point; NumPy has float32; some Keras-created arrays are float64; we casted this to float32 - no loss of prediction accuracy when checked
    - Checked matrix float multiplication using non-srand()

    - Implementing efficient matrix multiplication in C
        - Initial naive implementation; large matrix multiplication test timed with hardware timer
        - We could have used loop tiling [^3] but this was deemed unnecessary since our use case does not require great performance

- Checking for memory leaks - simple test sufficient
    - Repeatedly ran various matrix functions after compiling in GCC. Observed memory usage via Task Manager. Some functions had memory leaks.


### Making a Desu Noto interface
- japanese desu noto + turtle + bright colors
- ridiculously hard to keep track of state


### Conclusion
- touchscreen library - quite straighforward
- challenging to write your own matrix library
- dynamically allocating memory risks leaks, and finding causes (e.g. non-deallocation) is highly challenging