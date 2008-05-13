//----------------------------------------------------------------------------

///@file
///Declarations of classes used as an extensions in OpenGL window.
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

#ifndef MYOGL_EXTENSIONS_H
#define MYOGL_EXTENSIONS_H

//----------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN		///<For smaller executables and faster compilation
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <vector>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include "fpscounter.h"

//----------------------------------------------------------------------------

namespace MyOGL
	{

//----------------------------------------------------------------------------

	extern const int TEXTURES;		///<Enables textures in window
	extern const int BITMAP_FONTS;		///<Enables bitmap fonts in window
	extern const int OUTLINE_FONTS;		///<Enables outline fonts in window
	extern const int FPS_COUNTER;		///<Enables simple FPS indicator
	extern const int QUADRICS;		///<Enables OpenGL quadrics

//----------------------------------------------------------------------------

	///Exception class for MyOGL components.
	///This class is thrown when some critical errors are found.
	///Some other exception classes from this library inherit from it.
	class Exception: public std::runtime_error
		{
		private:
		public:
			///Constructor.
			///Sends specified message to its base class runtime_error
			///@param s message which will be available in exception from what() method
			Exception(const std::string &s): std::runtime_error(s)	{}
		};

//----------------------------------------------------------------------------

	///Specialization of template FPSCounter used in MyOGL project.
	typedef FPSCounter<4> FPS;

//---------------------------------------------------------------------------

	class Textures;
	class BitmapFonts;
	class OutlineFonts;
	class Quadrics;

//---------------------------------------------------------------------------

	///Simple container created to maintain different extensions such as textures, font, etc.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///This class is used internally by Window class; it creates desired types of extensions
	///and gives access to them.
	///@sa Window
	///@sa Textures
	///@sa BitmapFonts
	///@sa OutlineFonts
	///@sa FPSCounter
	class Extensions
		{
		public:
			///Constructor.
			///Creates objects which corresponds to extensions which were choosen.
			///@param iExtensionsFlags Bits corresponding to desired extensions
			///@param iParentHDC HDC is needed for some objects to work correctly (such as classes
			///derived from Fonts)
			Extensions(int iExtensionsFlags, HDC iParentHDC);
			///Destructor.
			///Disables extensions by simply destroying all previously created extension objects.
			///@sa extensionsFlags
			~Extensions();
			///Informs which extensions are enabled.
			///Extension must be enabled (its object created) in order to use it.
			///@return True if specified extension is enabled.
			///@par Example:
			///This is an example how to check whether the bitmap fonts are enabled:
			///@code
			/// if(extensions().enabled(BITMAP_FONTS))
			///		//bitmap fonts enabled
			///@endcode
			///@sa extensionsFlags
			bool enabled(int extensionCode) const	{return (extensionsFlags & extensionCode) != 0;}
			///Gives access to @ref winTextures object.
			///@return Reference to @ref winTextures object if textures were enabled;
			///if textures aren't enabled, throws an exception
			///@sa winTextures
			///@sa Window::Window()
			///@sa TEXTURES
			///@throws Window::Exception
			MyOGL::Textures &textures() const;
			///Gives access to @ref winBitmapFonts object.
			///@return Reference to @ref winBitmapFonts object if tbitmap fonts were enabled;
			///if bitmap fonts aren't enabled, throws an exception
			///@sa winTextures
			///@sa Window::Window()
			///@sa TEXTURES
			///@throws Window::Exception
			MyOGL::BitmapFonts &bitmapFonts() const;
			///Gives access to @ref winOutlineFonts object.
			///@return Reference to @ref winOutlineFonts object if outline fonts were enabled;
			///if outline fonts aren't enabled, throws an exception
			///@sa winOutlineFonts
			///@sa Window::Window()
			///@sa OUTLINE_FONTS
			///@throws Window::Exception
			MyOGL::OutlineFonts &outlineFonts() const;
			///Gives access to @ref winFpsCounter object.
			///@return Reference to @ref winFpsCounter object if FPS counter was enabled; if FPS counter
			///wasn't enabled, throws an exception
			///@sa winFpsCounter
			///@sa Window::Window()
			///@sa FPS_COUNTER
			///@throws Window::Exception
			MyOGL::FPS &fpsCounter() const;
			///Gives access to @ref winQuadrics object.
			///@return Reference to @ref winQuadrics object if quadrics were enabled; if quadrics
			///weren't enabled, throws an exception
			///@sa winQuadrics
			///@sa Window::Window()
			///@sa QUADRICS
			///@throws Window::Exception
			MyOGL::Quadrics &quadrics() const;
		private:
			///Hardware device context of a parent device.
			///This handle is used when creating font objects.
			const HDC parentHDC;
			///Bit flags of enabled extensions.
			///Rather using this variable directly, check which extensions are enabled
			///using enabled() public method
			///@sa enabled(int extensionCode)
			int extensionsFlags;
			///Pointer to an object used to control textures.
			///The object is created only if textures were enabled during creatiion of a window.
			///@sa Window::Window()
			///@sa TEXTURES
			Textures *winTextures;
			///Pointer to an object used to control bitmap fonts.
			///The object is created only if bitmap fonts were enabled during creatiion of a window.
			///@sa Window::Window()
			///@sa BITMAP_FONTS
			///@sa winOutlineFonts
			BitmapFonts *winBitmapFonts;
			///Pointer to an object used to control outline fonts.
			///The object is created only if outline fonts were enabled during creatiion of a window.
			///@sa Window::Window()
			///@sa OUTLINE_FONTS
			///@sa winBitmapFonts
			OutlineFonts *winOutlineFonts;
			///Pointer to an object used to check current and average framerate.
			///The object is created only if FPS counter was enabled during creatiion of a window.
			///@sa Window::Window()
			///@sa FPS_COUNTER
			FPS *winFpsCounter;
			///Pointer to an object used to draw simple quadrics.
			///The object is created only if quadrics were enabled during creatiion of a window.
			///@sa Window::Window()
			///@sa QUADRICS
			Quadrics *winQuadrics;
		};		//class Extensions

//----------------------------------------------------------------------------

	///Controlling and enabling textures in window.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///The class is used internally by Window class and there is no need to use it independently.
	///@par Example:
	///We assume that win is a reference to an object of type Window.
	///@code
	/// int t = win.extensions().textures().load("texture.bmp", GL_LINEAR, GL_LINEAR);
	/// win.extensions().textures().select(t);
	/// win.extensions().textures().enable();
	/// //Here put your code using texture
	/// win.extensions().textures().disable();
	///@endcode
	class Textures
		{
		private:
			///Stores count of loaded textures.
			///Variable is kind redundant, cause it stores same information as texturesId.size()
			///@sa load(const std::string &fileName, int minParam, int magParam)
			int texturesLoaded;
			///Array containing textures identificators.
			///Every texture has its unique id, which is saved in this vector
			///@sa select(int textureNum) const
			std::vector<unsigned int> texturesId;
			///Loads a BMP file into memory.
			///Opens a BMP file at specified location and returns pointer to an image data
			///@param fileName Full name and path of a BMP which you want to load
			///@return Newly created structure which contains image size and data.
			///@sa load(const std::string &fileName, int minParam, int magParam)
			///@throws Textures::TexEx
			AUX_RGBImageRec *loadBMP(const std::string &fileName);
		public:
			///Exception class for Textures
			///This class is thrown when some critical errors are found during using textures
			class TexEx: public Exception
				{
				private:
				public:
					///Constructor.
					///Sends specified message to its base class Exception
					///@param s message which will be available in exception from what() method
					TexEx(const std::string &s): Exception(s)	{}
				};
			///Constructor.
			Textures();
			///Enables textures.
			///Even if you enable textures while creating window (and even loading them into Textures object),
			///you still have to explicitly run enable() in order to use texturing.
			void enable()	{glEnable(GL_TEXTURE_2D);}
			///Disables textures.
			///Use if you want to use solid colors, lines, points, etc. after using textures.
			void disable()	{glDisable(GL_TEXTURE_2D);}
			///Loads texture from BMP file.
			///@param fileName Full name and path of a BMP which you want to load as a texture
			///@param minParam Used as a last parametr in call to GL function glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minParam);
			///@param magParam Used as a last parametr in call to GL function glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magParam);
			///@return Number of loaded texture (can be used in future in select() method)
			///@sa select(int textureNum)
			///@sa loadBMP(const std::string &fileName)
			///@throws Textures::TexEx
			int load(const std::string &fileName, int minParam, int magParam);
			///Selects a texture to use.
			///In contrast to BitmapFonts::select(), Textures class doesn't load its first object (texture) by default,
			///you must call select() explicitly.
			///@param textureNum Number returned from load(), can be from 0 to n - 1 (where n is the number of loaded textures)
			void select(int textureNum) const;
		};

//----------------------------------------------------------------------------

	///Pure virtual base class for managing fonts.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///There are various different types of fonts (bitmap, outline, etc.)
	///This class is a common interface for all of them.
	///More sophisticated font classes derive from this base class.
	class Fonts
		{
		protected:
			///First char which would be loaded and available to print onto the screen.
			///Normally it is blank char (space - ' ', code 32 in ASCII)
			///@sa load(const std::string &fontName, int fontSize)
			///@sa put(unsigned char c)
			///@sa put(const std::string s)
			static const unsigned char FIRST_AVAIL_CHAR = ' ';
			///Last char which would be loaded and available to print onto the screen.
			///Normally it is tilde ('~', code 126 is ASCII)
			///@sa load(const std::string &fontName, int fontSize)
			///@sa put(unsigned char c)
			///@sa put(const std::string s)
			static const unsigned char LAST_AVAIL_CHAR = 0xff;
			///How many chars are available ( <FIRST_AVAIL_CHAR ; LAST_AVAIL_CHAR> )
			static const int AVAIL_CHARS_COUNT = Fonts::LAST_AVAIL_CHAR - Fonts::FIRST_AVAIL_CHAR + 1;
			///Hardware device context of a parent device.
			///This handle is used when loading fonts.
			///@sa load(const std::string &fontName, int fontSize)
			const HDC parentHDC;
			///Currently used font.
			///It stores an index of a font (in range from 0 to n - 1, where n is a number of loaded
			///fonts), which is unique. It depends to derived class, how does this index will be used
			///(it might even be ignored, e.g. if class loads only one font)
			///It is 0 by default, so if you load only one font, you don't have to use select() method.
			///@sa select(int fontNum);
			unsigned int baseIndex;
			///Checks whether char is valid for putting onto the screen.
			///Char is valid if it was previously loaded in load() method.
			///This means that it has to bee in range <FIRST_AVAIL_CHAR;LAST_AVAIL_CHAR>
			///@param c Checked char
			///@return True if character is valid, otherwise false.
			static bool validChar(unsigned char c)	{return c >= FIRST_AVAIL_CHAR;}
		public:
			///Exception class for Fonts
			///This class is thrown when some critical errors are found during using fonts.
			///When some more class specific error occur, the derived class's exception are thrown
			///rather than this one.
			class FontsEx: public Exception
				{
				public:
					///Constructor.
					///Sends specified message to its base class Exception
					///@param s message which will be available in exception from what() method
					FontsEx(const std::string &s): Exception(s)	{}
				};
			///Base class constructor.
			Fonts(HDC iParentHDC): parentHDC(iParentHDC), baseIndex(0)	{}
			///Pure virtual destructor.
			virtual ~Fonts() = 0;
			///Selects which font to use.
			///This is pure virtual method. See its overriden versions in derived to class
			///for more details.
			///@param fontNum Font unique id number.
			virtual void select(int fontNum) = 0;
			///Loads new font from system fonts.
			///Pure virtual method, it must be overriden in all derived classes.
			///@param fontName System name of font you want to load
			///@param fontSize Size of font (in system points).
			///@return Unique font identifier.
			///See derived version of this method for more details.
			virtual int load(const std::string &fontName, int fontSize) = 0;
			///Puts character onto the screen using selected font.
			///@sa select(int fontNum)
			///@sa put(const std::string s)
			virtual void put(unsigned char c) = 0;
			///Puts whole std::string onto the screen.
			///If method is not overriden in derived classes, it simply calls virtual
			///put(unsigned char c) for every char in a string
			///@param s String to print
			///@sa put(unsigned char c)
			///@sa pos(float x, float y)
			///@sa select(int fontNum)
			virtual void put(const std::string s);
		};

//----------------------------------------------------------------------------

	///Simplified Fonts object output using overloaded operator<<().
	///Instead of using Fonts::put() you may use template function overloading operator<<() for
	///left hand Fonts& object. Then you can use Fonts object similar to ordinary streams.
	///@par Example:
	///@code
	/// glColor3f(1.0, 1.0, 0.0);
	/// win.extensions().bitmapFonts().pos(-0.3, 0.0);
	/// const std::string text = "OpenGL Bitmap Fonts";
	/// int x = 15;
	/// win.extensions().bitmapFonts() << text << ": " << x;
	///@endcode
	///Note that because operator<<() is a template function so you can output any data type
	///(std::string, int, const char*, etc.) which has overloaded operator<<() for left-hand
	///std::ostream& objects.
	///@sa Fonts::put()
	template<typename T>
	Fonts& operator<<(Fonts& fonts, const T& src)
		{
		fonts.put(boost::lexical_cast<std::string>(src));
		return fonts;
		}

//----------------------------------------------------------------------------

	///Controlling and enabling bitmap fonts in window.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///The class is used internally by Window class and there is no need to use it independently.
	///It allows to load switch many different fonts.
	///@par Example:
	///We assume that win is a reference to an object of type Window and bitmap fonts are enabled and loaded. 
	///@code
	/// glColor3f(1.0, 1.0, 0.0);
	/// win.extensions().bitmapFonts().pos(-0.3, 0.0);
	/// const std::string text = "OpenGL Bitmap Fonts";
	/// win.extensions().bitmapFonts().put(text);
	///@endcode
	///@note You don't have to use the select() method, if you load only one font (in opposition to Textures class).
	class BitmapFonts: public Fonts
		{
		private:
			///Stores different font bases in OpenGL display lists memory.
			///Every font is created as a set of many display lists (every char is a single display list).
			///Every position in this vector is a base of font, which is a pointer to the first char (normally space)
			///in display lists memory. Other characters are available at base + number addresses.
			///@sa baseIndex
			std::vector<GLuint> fontBases;
		public:

			///Exception class for BitmapFonts
			///This class is thrown when some critical errors are found during using bitmap fonts.
			class BitmapFontsEx: public Fonts::FontsEx
				{
				private:
				public:
					///Constructor.
					///Sends specified message to its base class Fonts::FontsEx
					///@param s message which will be available in exception from what() method
					BitmapFontsEx(const std::string &s): Fonts::FontsEx(s)	{}
				};

			///Constructor.
			///Parents HDC must be given to the object in order to work properly.
			///@param iParentHDC hardware device context.
			///@sa load(const std::string &fontName, int fontSize)
			BitmapFonts(HDC iParentHDC): Fonts(iParentHDC)	{}
			///Destructor.
			///Releases display lists memory for all fonts.
			~BitmapFonts();
			///Loads specified font.
			///@param fontName System font name, which you want to load
			///@param fontSize Size of font, which you want to use in your program. Note that you can't change the size of font in any way
			///(for example moving the viewport)
			///@return returns unique font number, which can be used in the future by select(int fontNum) method.
			///@throws BitmapFonts::BitmapFontsEx
			int load(const std::string &fontName, int fontSize);
			///Selects font which you want to use.
			///load(const std::string &fontName, int fontSize) method returns number, which is the font number
			///from 0 to n - 1, where n is a number of loaded bitmap fonts.
			///@note You don't have to call select(int fontNum), if you've loaded only one font
			///(it is selected by default).
			///@param fontNum unique bitmap font number
			///@throws BitmapFonts::BitmapFontsEx
			void select(int fontNum);
			///Positions text cursor in the screen.
			///This function is used to determinate the position of text you'll be putting onto the screen.
			///(x, y) are the coordinates on the screen, where (0, 0) is in the middle of a screen
			///and for example (-1.0, 1.0) is the upper left corner.
			///@param x X coordinate of cursor
			///@param y Y coordinate of cursor
			///@note Both x and y parameters should be in range [-1.0; 1.0]. Although this is recommended,
			///if you want to be more precise and for example put text more closer to the edge of a screen,
			///you may try to use bigger value, such as 1.05.
			///@return Reference to the currently used BitmapFonts objects. Thanks to this you can simply
			///write:
			///@code
			/// fonts.pos(0.0, 0.0) << "Message";
			///@endcode
			///which positions the text and inputs it in just one line of code.
			///@par
			///Second thing is that those value (x, y) are scaled for 4/3 screen ratio. If your viewport
			///has different ratio (or you're using e.g 1280x1024 resulotuon where this ratio is 5/4)
			///you may be obligated to choose the range for yourself and it may differ much than this
			///suggested.
			///@warning
			///You must specify color of text (using glColor...) <b>BEFORE</b> calling pos() method.
			///Otherwise color won't be applied. See example in Window::Window::Window()
			///@sa put(unsigned char c)
			///@sa put(const std::string s)
			BitmapFonts& pos(float x, float y);
			///Puts character onto the screen.
			///Character must be in range <FIRST_AVAIL_CHAR ; LAST_AVAIL_CHAR>, which is normally from space to tilde.
			///Otherwise exception will be thrown.
			///@param c Character, which will be shown.
			///@sa put(const std::string s)
			///@sa pos(float x, float y)
			///@sa select(int fontNum)
			///@throws BitmapFonts::BitmapFontsEx
			void put(unsigned char c);
			void put(const std::string s)	{Fonts::put(s);}
		};

//----------------------------------------------------------------------------

	///Enabling and controlling outline fonts in window.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///It allows to load switch many different outline fonts. Outline fonts are more flexible than bitmap
	///ones, because they are not flat bitmaps, but true 3D objects, which have thickness and can be
	///moved, rotated, etc.
	///The class is used internally by Window class and there is no need to use it independently.
	///@par Example:
	///We assume that win is a reference to an object of type Window and
	///outline fonts are enabled and loaded. 
	///@code
	/// const std::string text = "OpenGL Bitmap Fonts";
	/// glLoadIdentity();
	/// glTranslatef(-4.0, 0.0, -5.0);
	/// glRotatef(10.0, 1.0, 1.0, 1.0);
	/// glColor3f(1.0, 1.0, 0.0);
	/// win.extensions().outlineFonts().put(text);
	///@endcode
	///@note You don't have to use the select() method, if you load only one font
	///(in opposition to Textures class).
	class OutlineFonts: public Fonts
		{
		public:

			///Exception class for OutlineFonts
			///This class is thrown when some critical errors are found during using outline fonts.
			class OutlineFontsEx: public Fonts::FontsEx
				{
				public:
					///Constructor.
					///Sends specified message to its base class Fonts::FontsEx
					///@param s message which will be available in exception from what() method
					OutlineFontsEx(const std::string &s): Fonts::FontsEx(s)	{}
				};

			///Constructor.
			///@param iParentHDC Windows HDC is needed to obtain the fonts data.
			OutlineFonts(HDC iParentHDC): Fonts(iParentHDC), useTextures_(false)	{}
			///Destructor.
			///Releases display lists memory for all fonts.
			~OutlineFonts();
			///Selects outline font which you want to use.
			///load(const std::string &fontName, int fontSize) and
			///load(const std::string &fontName, int fontSize, float iThickness) method returns number,
			///which is the font number from 0 to n - 1, where n is a number of loaded bitmap fonts.
			///@note You don't have to call select(int fontNum), if you've loaded only one font
			///(it is selected by default).
			///@param fontNum unique bitmap font number
			///@throws OutlineFonts::OutlineFontsEx
			void select(int fontNum);
			///Loads specified font.
			///This method is less complex than load(const std::string &fontName, int fontSize, float iThickness)
			///which gives you ability to set thickness as well. You may use this method too, but remeber that
			///that it sets font thickness to default value which can't be changed.
			///@param fontName System font name, which you want to load
			///@param fontSize Size of font, which you want to use in your program.
			///@return returns unique font number, which can be used in the future by select(int fontNum) method.
			///@throws OutlineFonts::OutlineFontsEx
			///@sa load(const std::string &fontName, int fontSize, float iThickness)
			int load(const std::string &fontName, int fontSize)	{return load(fontName, fontSize, 0.2f);}
			///Loads specified font.
			///This method is more complex than the base classes overriden load(const std::string &fontName, int fontSize)
			///because it lets you to specify the font thickness.
			///@param fontName System font name, which you want to load
			///@param fontSize Size of font, which you want to use in your program.
			///@param iThickness Thickness (in Z plane, OpenGL units) of a font.
			///@return returns unique font number, which can be used in the future by select(int fontNum) method.
			///@throws OutlineFonts::OutlineFontsEx
			///@sa load(const std::string &fontName, int fontSize)
			int load(const std::string &fontName, int fontSize, float iThickness);
			///Puts character onto the screen.
			///Char's bottom left face corner is printed in (0.0, 0.0, 0.0) according to the current
			///viewport.
			///@param c Character to print
			///@sa width(unsigned char c)
			///@sa height(unsigned char c)
			///@sa depth(unsigned char c)
			void put(unsigned char c);
			void put(const std::string s)	{Fonts::put(s);}
			///Returns character width.
			///This information will be needed if you for example want to rotate character around its axis.
			///If you loaded multiple fonts, this method uses selected one. If you choose character which
			///was not loaded, exception will be thrown.
			///@return Character width in OpenGL units.
			///@throws OutlineFonts::OutlineFontsEx
			///@sa width(const std::string &s)
			///@sa height(unsigned char c)
			///@sa depth(unsigned char c)
			///@sa select(int fontNum)
			float width(unsigned char c);
			///Returns string width.
			///String width is the sum of width of all characters in the string. Character width is
			///gained using width(unsigned char c) function.
			///@return String width in OpenGL units.
			///@sa width(unsigned char c)
			float width(const std::string &s);
			///Returns character height.
			///This information will be needed if you for example want to rotate character around its axis.
			///If you loaded multiple fonts, this method uses selected one. If you choose character which
			///was not loaded, exception will be thrown.
			///@return Character height in OpenGL units.
			///@throws OutlineFonts::OutlineFontsEx
			///@warning For unknown reasons on my platform wglUseFontOutlines() functions always sets
			///gmfCellIncX (height) field of GLYPHMETRICSFLOAT structure to 0.0. That is why <b>I left this
			///function without definition</b>. If you know how to handle this problem, you might write your
			///own one.
			///@sa width(unsigned char c)
			///@sa depth(unsigned char c)
			///@sa select(int fontNum)
			float height(unsigned char c);
			///Returns character depth.
			///This information will be needed if you for example want to rotate character around its axis.
			///If you loaded multiple fonts, this method uses selected one. If you choose character which
			///was not loaded, exception will be thrown.
			///@return Character depth in OpenGL units.
			///@throws OutlineFonts::OutlineFontsEx
			///@sa width(unsigned char c)
			///@sa height(unsigned char c)
			///@sa select(int fontNum)
			float depth(unsigned char c)	{return fontsInfo[baseIndex].thickness;}
			///Enables texture mapping in outline fonts.
			///You can specify texture to be mapped onto the characters instead of just solid color.
			///@param iTextures Textures object to use during texture mapping
			///@param iTextureNum Texture number in Textures object to use. Before drawing each
			///Character Textures::select() method will be called and this texture will be choosen.
			///@sa useTextures()
			void useTextures(Textures &iTextures, int iTextureNum);
			///Disable texture mapping in outline fonts.
			///Call this overloaded function to disable texture mapping (which is default behaviour) and
			///go back to simple solid color texturing.
			void useTextures()	{useTextures_ = false;}
		private:

			///Structure containing information about one particular font and all its characters.
			struct OutlineFontInfo
				{
				///Sizes of all loaded characters.
				///Type GLYPHMETRICSFLOAT is a structure containing information about one character,
				///such as its width, height, and so on.
				///We create array with 256 fields, so we store information about all
				///characters in specified font.
				///Because AVAIL_CHAR_COUNT is not a constant in this context, we can't create
				///gmf[AVAIL_CHAR_COUNT]. But this redundance makes live easier since info about char(x)
				///is simply stored in gmf[x]
				///@sa width(unsigned char c)
				///@sa height(unsigned char c)
				GLYPHMETRICSFLOAT gmf[256];
				///Thickness of all characters in specified font.
				///Font can be given by load() method and is not stoed in GLYPHMETRICSFLOAT structure.
				///@sa depth(unsigned char c)
				float thickness;
				///Base pointer to a font.
				///Base is a pointer to a first loaded char (FIRST_AVAIL_CHAR). See @ref gmf to see,
				///how to computer the pointer to other chars.
				GLuint base;
				};

			///Contains information about all loaded fonts.
			///Each item in this vector stores information about one particular font.
			///@sa depth() and width()
			std::vector<OutlineFontInfo> fontsInfo;
			///MyOGL::Textures object used when texture mapping outline fonts.
			///@sa useTextures()
			Textures *textures;
			///Texture number in textures object to use.
			///@sa textures
			int textureNum;
			///Whether to use texture mapping.
			///This flag default to false, which means no texture mapping will be used.
			bool useTextures_;
		};

//----------------------------------------------------------------------------

	///Gives ability to draw GLU quadrics.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///Using this class you may very simply draw sphere, cylinders, and so on,
	///just by calling GLU-wrapped methods.
	///@par Example:
	///This example show how to draw spinning sphere using this object. Change the rot
	///shift value to control spinning speed. We assume that quadrics are enabled and some texture
	///is loaded and enabled (it will look better).
	///@code
	/// void draw(MyOGL::Window &win)
	/// {
	/// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/// glLoadIdentity();
	/// glTranslatef(0.0, 0.0, -5.0);
	/// static double rot;
	/// glRotatef(20.0, 1.0, -1.0, 0.0);
	/// glRotatef(rot += 1.0, 0.0, 0.0, 1.0);
	/// win.extensions().quadrics().sphere(1.0);
	/// win.refresh();
	/// }
	///@endcode
	///Play around with detail() method to make sphere more smooth-looking.
	///@sa sphere()
	///@sa cylinder()
	///@sa disk()
	///@sa partialDisk()
	class Quadrics
		{
		private:
			///Pointer to an object used during drawing.
			///It is created by gluNewQuadric() in constructor and released by gluDeleteQuadric()
			///in destructor.
			///@sa Quadrics()
			GLUquadricObj *qObj;
			///Number of details to display.
			///The bigger this number will be, the more smooth will the quadrics look.
			///Default value 16 may not be sufficient for some quadrics. You may change it
			///using details() method.
			///@sa details(int iDetails)
			int qDetails;
		public:
			///Constructor.
			///Sets up qObj object.
			///@param enableTextures Pass here true to make GLU create textures coordinates for quadrics.
			///This parameter is equal to Extensions::enabled(TEXTURES), which means that texture coordinates
			///are produced only when textures are enabled in Extensions.
			///@sa qObj
			Quadrics(bool enableTextures);
			///Destructor.
			///Releases qObj object.
			///@sa qObj
			~Quadrics()	{gluDeleteQuadric(qObj);}
			///Sets quadrics draw details.
			///Using this method you may change the value of qDetails.
			///If you want to make your shapes look more smooth, increase the details;
			///but if you want to speed-up your code, decrease the number of details.
			///@param iDetails Positive number of details which you want to show.
			///@sa qDetails
			void details(int iDetails)	{qDetails = iDetails;}
			///Draws a sphere on the scene.
			///@param radius Radius of a sphere
			///@sa cylinder()
			///@sa disk()
			///@sa partialDisk()
			void sphere(double radius)	{gluSphere(qObj, radius, qDetails, qDetails);}
			///Draws a cylinder on the scene.
			///@param baseRadius Radius at the base (bottom) of a cylinder
			///@param topRadius Radius at the top of cylinder
			///@param height Height of a cylinder
			///@note If one of the radiuses is set to 0.0, you will draw a cone.
			///@sa sphere()
			///@sa disk()
			///@sa partialDisk()
			void cylinder(double baseRadius, double topRadius, double height)
				{gluCylinder(qObj, baseRadius, topRadius, height, qDetails, qDetails);}
			///Draws a disc on the scene.
			///@param innerRadius Inner radius of a disc (make it 0 to have a circle without "hole" in the middle).
			///@param outerRadius Outer radius of a disc.
			///@sa sphere()
			///@sa cylinder()
			///@sa partialDisk()
			void disk(double innerRadius, double outerRadius)
				{gluDisk(qObj, innerRadius, outerRadius, qDetails, qDetails);}
			///Draw a partial disc on the scene.
			///Similar to disk(), but draws only a part of disc (in some angle-range).
			///@param innerRadius Inner radius of a disc (make it 0 to have a circle slice  without "hole" in the middle).
			///@param outerRadius Outer radius of a disc.
			///@param startAngle Starting angle of a slice
			///@param sweepAngle How big the slice will be (in degrees)
			///@sa disk()
			///@sa sphere()
			///@sa cylinder()
			void partialDisk(double innerRadius, double outerRadius, double startAngle, double sweepAngle)
				{gluPartialDisk(qObj, innerRadius, outerRadius, qDetails, qDetails, startAngle, sweepAngle);}
		};

//----------------------------------------------------------------------------

	}	//namespace MyOGL

//----------------------------------------------------------------------------

#endif

//----------------------------------------------------------------------------
