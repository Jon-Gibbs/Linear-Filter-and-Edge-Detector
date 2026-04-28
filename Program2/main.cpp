/*
Author: Jon Gibbs
File Contents:
The file containes methods to smooth an image and apply edge detection
It also includes sample usage of the Kernel class
*/
#include "Image.h"
#include "Kernel.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <cmath>
using namespace::std;

    //Destroys Kernel
    //precondition: a kernal object
    //postcondition: no more kernel
    Kernel::~Kernel() {
        delete weights;
    }

    //Creates a Kernel object
    //precondition: sufficeient sotrage in memory for a kernal object
    //postcondition: a kernal object with a custom kernel
    Kernel::Kernel(Image* k, int or_x, int or_y) {
        weights = k;
        this->or_x = or_x;
        this->or_y = or_y;
    }

    //Convolves an image with the Kernel obj's Kernel
    //precondition: a .gif image
    //postcondition: a smoothed version of the original image
    Image Kernel::convolve(Image& im) {
        //begin with a copy of the image
        Image output = Image(im);
        //apply smoothing via convolution using our matrix
        //for each pixel in the image, look at the surrounding images, apply the weights, and write the new pixel value to the image
        int row = im.getRows();
        int col = im.getCols();
        for (int i = 0; i < row; i++) {
            for (int j = 0; j < col; j++) {
                //apply the kernel to each pixel
                apply_kernel(im,output ,i, j);
            }
        }
        return output;
    }

    //Helper function that applies the Kernel to an individual pixel in the image
    //precondition: a pixel location, an input image, and an output image
    //postcondition: a float value at r,c in the output image obtained by applying the kernel
    void Kernel::apply_kernel(Image& input, Image& output, int r, int c) {
        int img_r = 0;
        int img_c = 0;
        float running_total = 0.0;
        //loop through your kernel and mulitply the specified weight
        for (int ki = 0; ki < weights->getRows(); ki++) {
            for (int kj = 0; kj < weights->getCols(); kj++) {
                img_r = r - or_y + ki;
                img_c = c - or_x + kj;
                if (img_r < 0) {
                    img_r = 0;
                }
                if (img_r >= input.getRows()) {
                    img_r = input.getRows() - 1;
                }
                if (img_c < 0) {
                    img_c = 0;
                }
                if (img_c >= input.getCols()) {
                    img_c = input.getCols() - 1;
                }
                running_total += input.getPixel(img_r, img_c).floatVal * weights->getFloat(ki, kj);
            }
        }
        output.setFloat(r, c, running_total);
    }

    //Takes an image written in RGBG bytes and converts the pixels to store float values 
    //precondition: a .gif image formated in RGBG
    //Postcondition: a .gif image formatted in float
    Image grey_to_float(Image im) {
        Image output = Image(im);
        for (int i = 0; i < im.getRows(); i++) {
            for (int j = 0; j < im.getCols(); j++) {
                output.setFloat(i, j, im.getPixel(i, j).grey);
            }
        }
        return output;
    }

    //Takes an image written in float and converts the pixels to store RGBG bytes
    //precondition: a .gif image formated in float
    //Postcondition: a .gif image formatted in RGBG
    Image float_to_grey(Image im) {
        Image output = Image(im);
        for (int i = 0; i < im.getRows(); i++) {
            for (int j = 0; j < im.getCols(); j++) {
                float val = im.getPixel(i, j).floatVal;
                if (val < 0) {
                    val = 0l;
                }
                if (val > 255) {
                    val = 255;
                }
                output.setGrey(i, j, (byte)val);
            }
        }
        return output;
    }

    //Given a fractional pixel location, this function returns a new pixel that it is a proportional mix of the pixels surrounding the fractional pixel location
    //precondition: an image containing pixels with rgb values and a vector with float coordinates corresponding to a certain pixel in the image 
    //postcondition: a pixel containing the proper float values given the coordinate
    pixel bilinear_interpolation(const Image& image, float row, float col) {
        pixel result;
        //clamp row and collumn
        if (row < 0) {
            row = 0;
        }
        if (col < 0) {
            col = 0;
        }
        if (row >= image.getRows()) {
            row = (float)image.getRows() - 1.0f;
        }
        if (col >= image.getCols()) {
            col = (float)image.getCols() - 1.0f;
        }

        //Define the rows and collumns that our "true pixel" sits between
        int top_pixel_row = int(row);
        int bottom_pixel_row = top_pixel_row + 1;
        int left_pixel_col = int(col);
        int right_pixel_col = left_pixel_col + 1;
        if (bottom_pixel_row >= image.getRows()) {
            bottom_pixel_row = image.getRows() - 1;
        }
        if (right_pixel_col >= image.getCols()) {
            right_pixel_col = image.getCols() - 1;
        }

        //Find the fractional difference for the rows and collumns
        float alpha = row - top_pixel_row;
        float beta = col - left_pixel_col;

        //If our pixel is outside the bounds of the image, return a black pixel
        if (bottom_pixel_row >= image.getRows() || row < 0 || right_pixel_col >= image.getCols() || col < 0) {
            result.floatVal = 0;
            return result;
        }

        //Get the 4 pixels our "true pixel" sits between
        pixel p00 = image.getPixel(top_pixel_row, left_pixel_col);    // top left
        pixel p01 = image.getPixel(top_pixel_row, right_pixel_col);  // top right
        pixel p10 = image.getPixel(bottom_pixel_row, left_pixel_col);    // bottom left
        pixel p11 = image.getPixel(bottom_pixel_row, right_pixel_col);  // bottom right

        //Calculate the cooresponding float value for the pixel using bilinear interpolation
        result.floatVal = ((1 - alpha) * (1 - beta) * p00.floatVal + (1 - alpha) * (beta)*p01.floatVal + (alpha) * (1 - beta) * p10.floatVal + (alpha) * (beta)*p11.floatVal);
        return result;
    }

    //Takes in an image and returns an image with just the edges outlined
    //precondition: a .gif floating image
    //postcondition: a .gif with outlined edges in floating format
    Image edge_detection(Image& input) {
        Image output = Image(input);
        Image gx(input);
        Image gy(input);

        Image* vertMag = new Image(3, 1);
        vertMag->setFloat(0, 0, -1);
        vertMag->setFloat(1, 0, 0);
        vertMag->setFloat(2, 0, 1);
        Kernel vertMagKernel(vertMag, 0, 1);

        Image* horzMag = new Image(1, 3);
        horzMag->setFloat(0, 0, -1);
        horzMag->setFloat(0, 1, 0);
        horzMag->setFloat(0, 2, 1);
        Kernel horzMagKernel(horzMag, 1, 0);

        gx = horzMagKernel.convolve(gx);
        gy = vertMagKernel.convolve(gy);
        //find the magnitude of each pixel
        for (int i = 0; i < input.getRows(); i++) {
            for (int j = 0; j < input.getCols(); j++) {
                //Get the magnitude of the current pixel
                float mag = sqrt((gx.getFloat(i, j) * gx.getFloat(i, j)) + (gy.getFloat(i, j) * gy.getFloat(i, j)));
                //Check if magnitude is greater than 10, if not move to the next pixel
                if (mag < 10.f) {
                    output.setFloat(i, j, 0);
                    continue;
                }

                //If the mag is 10, determine if it is a local maxima
                //make unit vector out of  gx and gy
                float unit_x = gx.getFloat(i, j) / mag;
                float unit_y = gy.getFloat(i, j) / mag;

                //Perform bilinear interpolation to get the magnitude of neighbor 1
                float neighbor_1_x = unit_x + j;
                float neighbor_1_y = unit_y + i;
                pixel neighbor_1_gx = bilinear_interpolation(gx, neighbor_1_y, neighbor_1_x);
                pixel neighbor_1_gy = bilinear_interpolation(gy, neighbor_1_y, neighbor_1_x);
                float neighbor_1_mag = sqrt((neighbor_1_gx.floatVal * neighbor_1_gx.floatVal) + (neighbor_1_gy.floatVal * neighbor_1_gy.floatVal));

                //Perform bilinear interpolation to get the magnitude of neighbor 2
                float neighbor_2_x = -1 * unit_x + j;
                float neighbor_2_y = -1 * unit_y + i;
                pixel neighbor_2_gx = bilinear_interpolation(gx, neighbor_2_y, neighbor_2_x);
                pixel neighbor_2_gy = bilinear_interpolation(gy, neighbor_2_y, neighbor_2_x);
                float neighbor_2_mag = sqrt((neighbor_2_gx.floatVal * neighbor_2_gx.floatVal) + (neighbor_2_gy.floatVal * neighbor_2_gy.floatVal));

                //If current pixel mag is greater than the magnitude of either neighbor's magnitude, it is 255, otherwise, it is 0
                if (mag >= neighbor_1_mag && mag >= neighbor_2_mag) {
                    output.setFloat(i, j, 255);
                }
                else {
                    output.setFloat(i, j, 0);
                }
            }
        }
        return output;
    }

    //This main contains sample usage of the Kernel class and helper functions found in main.cpp. Given an image, we smooth it and run edge detection on it
    //precondition: 6 arguments referring to the transformation factors for the image
    //postcondition: a new image called "output1.gif" stored in the source directory that is a trasnformed version of "test1.gif"
    int main(int argc, char* argv[]) {
        // read iteration count from command line
        int iterations = atoi(argv[1]);

        // vertical kernel [1/4, 1/2, 1/4] — 3 rows, 1 col, origin at middle row
        Image* vertWeights = new Image(3, 1);
        vertWeights->setFloat(0, 0, 0.25f);
        vertWeights->setFloat(1, 0, 0.50f);
        vertWeights->setFloat(2, 0, 0.25f);
        Kernel vertKernel(vertWeights, 0, 1); // origin: col=0, row=1 (middle)

        // horizontal kernel [1/4, 1/2, 1/4] — 1 row, 3 cols, origin at middle col
        Image* horizWeights= new Image(1, 3);
        horizWeights->setFloat(0, 0, 0.25f);
        horizWeights->setFloat(0, 1, 0.50f);
        horizWeights->setFloat(0, 2, 0.25f);
        Kernel horizKernel(horizWeights, 1, 0); // origin: col=1 (middle), row=0

        // load image and convert to float
        Image input("test2.gif");

        // apply smoothing kernels iteratively
        Image output = grey_to_float(input);

        // smooth by convolution
        for (int i = 0; i < iterations; i++) {
            output = vertKernel.convolve(output);
            output = horizKernel.convolve(output);
        }
        //make our edge image from copy of smoothed image
        Image edge(output);
        //run edge detection algorithm
        edge = edge_detection(edge);
        //convert both images from grey to float
        edge = float_to_grey(edge);
        output = float_to_grey(output);
        //write images to disk
        output.writeGreyImage("smooth.gif");
        edge.writeGreyImage("edges.gif");
        return 0;
    }