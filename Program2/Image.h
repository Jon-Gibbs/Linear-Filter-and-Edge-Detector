// Image.h
// by Clark Olson 2002-2014
//
// This file describes the interface to a set of library
// functions working with images.  Functionality includes
// reading and writing GIF images, modifying and copying
// images.

#pragma once

#include <string>
using namespace std;

// Each pixel color is stored as a value from 0..255 (an unsigned char)
typedef unsigned char byte;

// Each pixel has 3 color bands: red, green, and blue
// (if it is a full color image) and an average color: grey
// that describes the overall intensity of the pixel.
typedef union {
	struct {	// A pixel can store 4 bytes of data:
		byte red;		//  intensity of the red component
		byte green;		//  intensity of the green component
		byte blue;		//  intensity of the blue component
		byte grey;		//  average intensity of the pixel
	};
	int intVal;		// Alternatively, a pixel can store an int
	float floatVal; // or a float
} pixel;

// A simple image data structure:
//   rows is the height of the image (the number of rows of pixels)
//   cols is the width of the image (the number of columns of pixels)
//   pixels is a 2D array of the image pixels
//     The first dimension of pixels varies from 0 to rows-1
//     The second dimension of pixels varies from 0 to cols-1
//   The pixel at row i and column j is accessed by pixels[i][j].
//   With the following definition:
//	image *myimage;
//   We could access the red component of the pixel at row 10, column 20 by:
//	myimage->pixels[10][20].red
struct image { 
  int   rows, cols;         /* pic size */
  pixel **pixels;		   /* image data */
};


// Image class to read, write, and otherwise work with images.
class Image
{
public:
	// Default constructor
	// Postconditions: creates an image with 0 rows and 0 columns
	Image();

	// Constructor
	// Preconditions: rows, cols describe the desired size of the new image
	//				  rows, cols must be greater than zero
	// Postconditions: if sufficient memory is available, a new image
	// is returned using newly allocated memory.
	// Each pixel has red = 0, green =0, blue = 0.
	// Otherwise, the returned image has:
	// rows = 0, cols = 0, pixels = nullptr.
	Image(int rows, int cols);
	
	// Constructor
	// Preconditions: filename refers to a file that stores a GIF image
	// Postconditions: if sufficient memory is available, a new image
	// is returned corresponding to the values in the file.
	// Otherwise, the returned image has:
	// rows = 0, cols =0, pixels = nullptr.
	Image(string filename);

	// Copy constructor
	// Postconditions: if sufficient memory is available, a new image
	// is created with the same pixel values as in the input image.
	// Otherwise, the new image has:
	// rows = 0, cols =0, pixels = nullptr.
	Image(Image const &anImage);

	// Destructor
	// Postconditions:  all allocated memory is deallocated.
	~Image();

	// getRows (accessor)
	// Postconditions: returns the number of rows in the image
	int getRows() const;

	// getCols (accessor)
	// Postconditions: returns the number of columns in the image
	int getCols() const;

	// writeimage
	// Preconditions: filename refers to a valid location to store an image
	// Postconditions: a GIF file is stored corresponding to the pixel values
	//				   in the image.  Note well: color GIF images are stored with
	//				   lossy compression, so the image stored may not be exactly
	//				   the same as the pixel values passed to the function.
	void writeImage(string filename) const;

	// writeGreyImage
	// Preconditions: filename refers to a valid location to store an image
	// Postconditions: a GIF file is stored corresponding the the grey pixel
	//				   values in the image.  Greylevel GIF images
	//				   are compressed losslessly.  So, if the image is read back
	//				   you will get exactly the same (black-and-white) image
	//                 that was written.
	void writeGreyImage(string filename) const;
	
	// writeFloatImage
	// Preconditions: filename refers to a valid location to store an image
	// Postconditions:  a GIF file is stored corresponding to the floatVals
	//					stored at each pixel in the image.  If fmax is the
	//					maximum floatVal in the image and fmin is the minimum
	//					floatVal in the image, then each pixel is stored as
	//					255 * (fVal - fmin) / (fmax - fmin)
	void writeFloatImage(string filename) const;
	
	// writeIntImage
	// Preconditions: filename refers to a valid location to store an image
	// Postconditions:  a GIF file is stored corresponding to the intVals
	//					stored at each pixel in the image.  If imax is the
	//					maximum intVal in the image and fmin is the minimum
	//					intVal in the image, then each pixel is stored as
	//					255 * (iVal - imin) / (imax - imin)
	void writeIntImage(string filename) const;
	
	// getPixel (accessor)
	// Preconditions: row and col are greater than (or equal to) zero
	//				  row < getRow(), col < getCol()
	// Postconditions: the returned pixel corresponds to the pixel values stored
	//				   at the correct row and column.
	pixel getPixel(int row, int col) const;

	// getFloat (accessor)
	// Preconditions: row and col are greater than (or equal to) zero
	//				  row < getRow(), col < getCol()
	// Postconditions: the returned pixel corresponds to the pixel values stored
	//				   at the correct row and column.
	float getFloat(int row, int col) const;

	// getInt (accessor)
	// Preconditions: row and col are greater than (or equal to) zero
	//				  row < getRow(), col < getCol()
	// Postconditions: the returned pixel corresponds to the pixel values stored
	//				   at the correct row and column.
	int getInt(int row, int col) const;

	// setPixel
	// Preconditions: row and col are greater than (or equal to) zero
	//				  row < getRow(), col < getCol()
	// Postconditions: the appropriate pixel of the image is set to newValue.
	void setPixel(int row, int col, pixel newValue);
	
	// setPixel
	// Preconditions: row and col are greater than (or equal to) zero
	//				  row < getRow(), col < getCol()
	// Postconditions: the appropriate pixel of the image is set to have
	//				   the color specified by the red, green, and blue inputs
	void setPixel(int row, int col, byte red, byte green, byte blue);

	// setGrey
	// Preconditions: row and col are greater than (or equal to) zero
	//				  row < getRow(), col < getCol()
	// Postconditions: the appropriate pixel of the image is set to have
	//				   the greylevel specified by the input byte
	void setGrey(int row, int col, byte grey);

	// setInt
	// Preconditions: row and col are greater than (or equal to) zero
	//				  row < getRow(), col < getCol()
	// Postconditions: the appropriate pixel of the image is set to have
	//				   the intVal specified by the input integer
	void setInt(int row, int col, int intVal);

	// setFloat
	// Preconditions: row and col are greater than (or equal to) zero
	//				  row < getRow(), col < getCol()
	// Postconditions: the appropriate pixel of the image is set to have
	//				   the floatVal specified by the input float
	void setFloat(int row, int col, float floatVal);

	// operator==
	// Preconditions: none
	// Postconditions: returns true if the two images have the same number
	//				   of rows and columns and the same color at every single
	//				   corresponding pixel.
	bool operator==(const Image &a) const;

	// operator=
	// Preconditions: none
	// Postconditions: assigns the value of the rhs to the lhs and returns the value
	Image &operator=(const Image &rhs);

	// photonegative
	// Preconditions: none
	// Postconditions: returns a new image containing an image of the same
	//				   size, but with every single pixel color inverted.
	Image photonegative() const;

private:
	image I;	// The private image data.
};
