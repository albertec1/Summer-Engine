#include "TextureImporter.h"
#include "Globals.h"
#include "Application.h"
#include "Dependencies/Assimp/include/material.h"
#include "Dependencies/Assimp/include/texture.h"

#include <cmath>

#pragma comment( lib, "DevIL.lib" )
#include "Dependencies/Devil/include/ilu.h"
#pragma comment( lib, "ILU.lib" )
#include "Dependencies\Devil\include\ilut.h"
#pragma comment( lib, "ILUT.lib" )

//Devil

TextureImporter::TextureImporter()
{

}

TextureImporter::~TextureImporter()
{

}

uint* TextureImporter::LoadTexture()
{
	return 0;
}

uint* TextureImporter::CheckerImage()
{
	return 0;
}