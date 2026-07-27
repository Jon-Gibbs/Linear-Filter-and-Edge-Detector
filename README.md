# Overview
Given a .gif image, this C++ software applies industry standard image smoothing,
linear filtering, and edge detection techniques to give the user an edge image. This program must be run using the MSVC compiler.

# Example
Below, we see three different images of a cartoon dog. Our input image is fed to the program and smoothed out by making each pixel an average of the pixels that surround it. At this stage, we also convert the image to black and white for efficiency.
Lastly, we take the smoothed image and construct a map of image gradients that allow us to find areas in the image that change very quickly and mark them as edges in white.

## Input Image
<img width="800" height="600" alt="jake" src="https://github.com/user-attachments/assets/955f4ff2-f1ce-44ae-a069-574b138070d1" />

## Smoothed Image
<img width="800" height="600" alt="smooth" src="https://github.com/user-attachments/assets/8757c70e-f4ef-452e-959a-2333ed21a9a4" />

## Edge Image
<img width="800" height="600" alt="edges" src="https://github.com/user-attachments/assets/ba8b32a0-54ab-4894-9358-f2a76101f839" />
