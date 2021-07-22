#pragma once

#include "Globals.h"
#include <string>
#include <vector>

class TextureImporter
{
public:

	TextureImporter();
	~TextureImporter();

	uint* CheckerImage();
	uint* LoadTexture();

public:



};

class TextureInfo
{
public:
	uint tex_ID;
	uint tex_width;
	uint tex_height;
	std::string path;
};