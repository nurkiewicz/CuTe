//----------------------------------------------------------------------------

///@file
///Definitions of classes used as an extensions in OpenGL window.
///
///@par License:
///@verbatim
///MyOGL - My OpenGL utility, simple OpenGL Windows framework
///Copyright (C) 2005-06 Tomasz Nurkiewicz
///For full license text see license.txt.
///
///This program is free software; you can redistribute it and/or modify it under the terms of
///the GNU General Public License as published by the Free Software Foundation;
///either version 2 of the License, or (at your option) any later version.
///
///This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
///without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
///See the GNU General Public License for more details.
///
///You should have received a copy of the GNU General Public License along with this program;
///if not, write to the Free Software Foundation, Inc.,
///59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
///@endverbatim
///
///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
///@date Jul 2005-Mar 2006

//----------------------------------------------------------------------------

#include <boost/filesystem/operations.hpp>
#include "extensions.h"
using namespace MyOGL;
using namespace std;
namespace filesys = boost::filesystem;

//----------------------------------------------------------------------------

const int MyOGL::TEXTURES = 0x0001;
const int MyOGL::BITMAP_FONTS = 0x0002;
const int MyOGL::OUTLINE_FONTS = 0x0004;
const int MyOGL::FPS_COUNTER = 0x0008;
const int MyOGL::QUADRICS = 0x0010;

//----------------------------------------------------------------------------

Extensions::Extensions(int iExtensionsFlags, HDC iParentHDC):
	extensionsFlags(iExtensionsFlags), parentHDC(iParentHDC)
	{
	if(enabled(TEXTURES))
		winTextures = new Textures;
	if(enabled(BITMAP_FONTS))
		winBitmapFonts = new BitmapFonts(parentHDC);
	if(enabled(OUTLINE_FONTS))
		winOutlineFonts = new OutlineFonts(parentHDC);
	if(enabled(FPS_COUNTER))
		winFpsCounter = new FPS;
	if(enabled(QUADRICS))
		winQuadrics = new Quadrics(enabled(TEXTURES));
	}

Extensions::~Extensions()
	{
	if(enabled(TEXTURES))
		delete winTextures;
	if(enabled(BITMAP_FONTS))
		delete winBitmapFonts;
	if(enabled(OUTLINE_FONTS))
		delete winOutlineFonts;
	if(enabled(FPS_COUNTER))
		delete winFpsCounter;
	if(enabled(QUADRICS))
		delete winQuadrics;
	}

Textures &Extensions::textures() const
	{
	if(!enabled(TEXTURES))
		throw Exception("Textures are not enabled");
	return *winTextures;
	}

BitmapFonts &Extensions::bitmapFonts() const
	{
	if(!enabled(BITMAP_FONTS))
		throw Exception("Bitmap fonts are not enabled");
	return *winBitmapFonts;
	}

MyOGL::OutlineFonts &Extensions::outlineFonts() const
	{
	if(!enabled(OUTLINE_FONTS))
		throw Exception("Outline fonts are not enabled");
	return *winOutlineFonts;
	}

FPS &Extensions::fpsCounter() const
	{
	if(!enabled(FPS_COUNTER))
		throw Exception("FPS counter is not enabled");
	return *winFpsCounter;
	}

MyOGL::Quadrics &Extensions::quadrics() const
	{
	if(!enabled(QUADRICS))
		throw Exception("Quadrics are not enabled");
	return *winQuadrics;
	}

//---------------------------------------------------------------------------

Textures::Textures(): texturesLoaded(0)	{}

AUX_RGBImageRec *Textures::loadBMP(const std::string &fileName)
	{
	if(!filesys::exists(fileName))
		throw Textures::TexEx("Can't load texture: " + fileName);
	return auxDIBImageLoad(fileName.c_str());
	}

int Textures::load(const std::string &fileName, int minParam, int magParam)
	{
	texturesId.push_back(0);
	AUX_RGBImageRec *textureImage;
	textureImage = loadBMP(fileName);
	glGenTextures(1, &texturesId[texturesLoaded]);
	glBindTexture(GL_TEXTURE_2D, texturesId[texturesLoaded]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magParam);
	if(minParam != GL_LINEAR_MIPMAP_NEAREST)
		glTexImage2D(GL_TEXTURE_2D, 0, 3, textureImage->sizeX, textureImage->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, textureImage->data);
	else
		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, textureImage->sizeX, textureImage->sizeY, GL_RGB,GL_UNSIGNED_BYTE, textureImage->data);
	free(textureImage->data);
	free(textureImage);
	return ++texturesLoaded;
	}

void Textures::select(int textureNum) const
	{
	if(textureNum < texturesLoaded)
		glBindTexture(GL_TEXTURE_2D, texturesId[textureNum]);
	else
		throw Textures::TexEx("Specified texture does not exist");
	}

//----------------------------------------------------------------------------

Fonts::~Fonts()	{} //Definition of pure virtual function must be given.

void Fonts::put(std::string s)
	{
	for(string::iterator i = s.begin(); i != s.end(); ++i)
		put(*i);
	}

//----------------------------------------------------------------------------

BitmapFonts::~BitmapFonts()
	{
	for(vector<GLuint>::iterator i = fontBases.begin(); i != fontBases.end(); ++i)
		glDeleteLists(*i, AVAIL_CHARS_COUNT);
	}

int BitmapFonts::load(const std::string &fontName, int fontSize)
	{
	fontBases.push_back(glGenLists(AVAIL_CHARS_COUNT));
	HFONT temp, font = CreateFont(-fontSize, 0, 0, 0, FW_BOLD, false, false, false, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, fontName.c_str());
	temp = static_cast<HFONT>(SelectObject(parentHDC, font));
	wglUseFontBitmaps(parentHDC, FIRST_AVAIL_CHAR, AVAIL_CHARS_COUNT, fontBases.back());
	SelectObject(parentHDC, temp);
	DeleteObject(font);
	return fontBases.size() - 1;
	}

void BitmapFonts::select(int fontNum)
	{
	if((fontNum >= static_cast<int>(fontBases.size())) || (fontNum < 0))
		throw BitmapFontsEx("Specified font does not exist");
	baseIndex = fontNum;
	}

BitmapFonts& BitmapFonts::pos(float x, float y)
	{
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -2.0);
	glRasterPos2f(x, y * 3 / 4);
	return *this;
	}

void BitmapFonts::put(unsigned char c)
	{
	glDisable(GL_DEPTH_TEST);
	if(baseIndex >= fontBases.size())
		throw BitmapFontsEx("No fonts were loaded");
	if(!validChar(c))
		throw BitmapFontsEx("Specified character is not supported");
	glCallList(fontBases[baseIndex] - FIRST_AVAIL_CHAR + c);
	glEnable(GL_DEPTH_TEST);
	}

//----------------------------------------------------------------------------

OutlineFonts::~OutlineFonts()
	{
	for(vector<OutlineFontInfo>::iterator i = fontsInfo.begin(); i != fontsInfo.end(); ++i)
		glDeleteLists(i->base + FIRST_AVAIL_CHAR, AVAIL_CHARS_COUNT);
	}

int OutlineFonts::load(const std::string &fontName, int fontSize, float iThickness)
	{
	fontsInfo.push_back(OutlineFontInfo());
	fontsInfo.back().base = glGenLists(AVAIL_CHARS_COUNT);
	fontsInfo.back().thickness = iThickness;
	HFONT font = CreateFont(-fontSize, 0, 0, 0, FW_BOLD, false, false, false, DEFAULT_CHARSET, OUT_TT_PRECIS,
		CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE | DEFAULT_PITCH, fontName.c_str());
	SelectObject(parentHDC, font);
	wglUseFontOutlines(parentHDC, FIRST_AVAIL_CHAR, AVAIL_CHARS_COUNT, fontsInfo.back().base, 0.0f,
		fontsInfo.back().thickness, WGL_FONT_POLYGONS, fontsInfo.back().gmf + FIRST_AVAIL_CHAR);
	return fontsInfo.size() - 1;
	}

void OutlineFonts::select(int fontNum)
	{
	if((fontNum >= static_cast<int>(fontsInfo.size())) || (fontNum < 0))
		throw OutlineFontsEx("Specified font does not exist");
	baseIndex = fontNum;
	}

void OutlineFonts::put(unsigned char c)
	{
	if(baseIndex >= fontsInfo.size())
		throw OutlineFontsEx("No fonts were loaded");
	if(!validChar(c))
		throw OutlineFontsEx("Specified character is not supported");
	if(useTextures_)
		{
		glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
		glEnable(GL_TEXTURE_GEN_S);
		glEnable(GL_TEXTURE_GEN_T);
		textures->enable();
		textures->select(textureNum);
		glCallList(fontsInfo[baseIndex].base + c - FIRST_AVAIL_CHAR);
		textures->disable();
		glDisable(GL_TEXTURE_GEN_T);
		glDisable(GL_TEXTURE_GEN_S);
		}
	else
		glCallList(fontsInfo[baseIndex].base + c - FIRST_AVAIL_CHAR);
	}

float OutlineFonts::width(unsigned char c)
	{
	if(!validChar(c))
		throw OutlineFontsEx("Specified character is not supported");
	return fontsInfo[baseIndex].gmf[c].gmfCellIncX;
	}

float OutlineFonts::width(const std::string &s)
	{
	float sum = 0.0;
	for(string::const_iterator i = s.begin(); i != s.end(); ++i)
		sum += width(*i);
	return sum;
	}

void OutlineFonts::useTextures(Textures &iTextures, int iTextureNum)
	{
	useTextures_ = true;
	textures = &iTextures;
	textureNum = iTextureNum;
	}

//----------------------------------------------------------------------------

Quadrics::Quadrics(bool enableTextures): qDetails(16)
	{
	qObj = gluNewQuadric(); 
	gluQuadricNormals(qObj, GLU_SMOOTH);
	gluQuadricTexture(qObj, enableTextures);
	}

//----------------------------------------------------------------------------
