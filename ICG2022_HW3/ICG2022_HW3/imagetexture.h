#ifndef IMAGE_TEXTURE_H
#define IMAGE_TEXTURE_H

#include "headers.h"
using namespace std;

// Texture Declarations.
class ImageTexture
{
public:
	// Texture Public Methods.
	ImageTexture(const std::string filePath);
	~ImageTexture();

	void Bind(GLenum textureUnit);
	void Preview();
	string GetPath() const { return texFilePath; }

private:
	// Texture Private Data.
	string texFilePath;
	GLuint textureObj;//openGl texture object(ID)
	int imageWidth;
	int imageHeight;
	int numChannels;
	cv::Mat texImage;
};

#endif

