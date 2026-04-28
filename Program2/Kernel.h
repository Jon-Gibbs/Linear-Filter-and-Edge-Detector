#pragma once
/*
Author: Jon Gibbs
File Contents:
This is the header file for the Kernal
The kernel is used to apply kernels to an image from the image.h library
*/
#include "Image.h"
#include <vector>
class Kernel {
public:
	//Creates a new Kernel object
	//precondition: sufficeient sotrage in memory for a kernal object
	//postcondition: a kernal object with a custom kernel
	Kernel(Image* k, int or_x, int or_y);

	//Destroys kernel
	//precondition: a kernal object
	//postcondition: no more kernel
	~Kernel();

	//convolves a given image with the kernel
	//precondition: a .gif image
	//postcondition: a smoothed version of the original image
	Image convolve(Image& im);
	
	//Helps the kernel be applied to an indididual pixel
	//precondition: a .gif image from the Image library
	//postcondition: a new pixel that has had the Kernel applied to it
	void apply_kernel(Image& input, Image& output, int r, int c);
	


private:
	int or_x=1;
	int or_y=1;
	Image *weights = nullptr;
};