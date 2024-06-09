#include "texture.h"

#include <iostream>

texture2D::texture2D() : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
{
	glGenTextures(1, &this->ID);
}

texture2D::texture2D(unsigned int ID)
{
	glBindTexture(GL_TEXTURE_2D, ID);
	int wrapS, wrapT, filterMin, filterMax, internalFormat, imageFormat;
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalFormat);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_IMAGE_FORMAT, &imageFormat);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &wrapS);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &wrapT);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &filterMin);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &filterMax);
	this->ID = ID;
	this->Wrap_S = wrapS;
	this->Wrap_T = wrapT;
	this->Filter_Min = filterMin;
	this->Filter_Max = filterMax;
	this->Internal_Format = internalFormat;
	this->Image_Format = imageFormat;
	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data)
{
	this->Width = width;
	this->Height = height;
	// create Texture
	glBindTexture(GL_TEXTURE_2D, this->ID);
	glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);
	// set Texture wrap and filter modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void texture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, this->ID);
}