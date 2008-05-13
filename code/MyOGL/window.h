//---------------------------------------------------------------------------

///@file
///Declaration of a simple OpenGL Window class.
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
///@par
///Class is based on NeHe OpenGL Tutorial (available at http://nehe.gamedev.net),
///and it is an OOP wrapper for WinAPI code

//---------------------------------------------------------------------------

#ifndef MYOGL_WINDOW_H
#define MYOGL_WINDOW_H

//---------------------------------------------------------------------------

#include "hsv2rgb.h"
#include "extensions.h"

//---------------------------------------------------------------------------

///MyOGL - My OpenGL utility, simple OpenGL Windows framework
namespace MyOGL
	{

//---------------------------------------------------------------------------

	///Changes OpenGL color using HSV instead of RGB color model.
	///@param hue Color hue; this is an angle in radians and should be in range <0;2PI);
	///Pleas check remarks at hsv2rgb(float hue, float saturation, float value, float rgb[])
	///@param saturation Color saturation. Should be in range <0.0;1.0>, otherwise the result are undefined.
	///@param value Brithness of a color. Should be in range <0.0;1.0>, otherwise the result are undefined.
	///@par Example:
	///Switches into solid bright blue (hue = 4/3 PI = 240 degrees)
	///@code
	///glColorHSV(4 * M_PI / 3, 1.0, 1.0);
	///@endcode
	///@sa hsv2rgb(float hue, float saturation, float value)
	///@sa hsv2rgb(float hue, float saturation, float value, float rgb[])
	///@sa glColorHSV()
	void glColorHSV(float hue, float saturation, float value);

	///Changes OpenGL color using HSV instead of RGB color model including alpha channel
	///@param hue Color hue; this is an angle in radians and should be in range <0;2PI);
	///Pleas check remarks at hsv2rgb(float hue, float saturation, float value, float rgb[])
	///@param saturation Color saturation. Should be in range <0.0;1.0>, otherwise the result are undefined.
	///@param value Brithness of a color. Should be in range <0.0;1.0>, otherwise the result are undefined.
	///@param alpha OpenGL alpha value. Should be in range <0.0;1.0>, otherwise the result are undefined.
	///@sa hsv2rgb(float hue, float saturation, float value)
	///@sa hsv2rgb(float hue, float saturation, float value, float rgb[])
	///@sa glColorHSV()
	void glColorHSV(float hue, float saturation, float value, float alpha);

//---------------------------------------------------------------------------

	///Used to create fullscreen window by Window::Window()
	extern const bool FULLSCREEN;
	///Used to create ordinary window by Window::Window()
	extern const bool WINDOWED;

//---------------------------------------------------------------------------

	///Creates window using WinAPI adapted to OpenGL programs.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par Example:
	///@code
	///
	/// #include "MyOGL/window.h"
	///
	/// void draw(MyOGL::Window &win)
	/// 	{
	/// 	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/// 	glLoadIdentity();
	/// 	win.refresh();
	/// 	}
	///
	/// WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	/// 	{
	/// 	MSG msg;
	/// 	try
	/// 		{
	/// 		bool done = false;
	/// 		MyOGL::Window win("My OpenGL Window", 800, 600, MyOGL::WINDOWED);
	/// 		do
	/// 			{
	/// 			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	/// 				if(msg.message != WM_QUIT)
	/// 					{
	/// 					TranslateMessage(&msg);
	/// 					DispatchMessage(&msg);
	/// 					}
	/// 				else
	/// 					done = true;
	/// 			else
	/// 				if(win.active())
	/// 					if(win.keyPressed(VK_ESCAPE))
	/// 						done = true;
	/// 					else
	/// 						draw(win);
	/// 			if(win.keyPressed(VK_F3))
	/// 				win.toggleFullscreen();
	/// 			}
	/// 		while(!done);
	/// 		}
	/// 	catch(MyOGL::Exception &e)
	/// 		{
	/// 		MessageBox(NULL, e.what(), "OpenGL Window error", MB_OK | MB_ICONEXCLAMATION);
	/// 		}
	/// 	return msg.wParam;
	/// 	}
	///@endcode
	///This is the simpliest program using MyOGL::Window class. However it shows how to create window,
	///draw and refresh OpenGL scene and cooperate with keyboard. Besides it shows how to handle exceptions.
	class Window
		{
		private:

			///Exception class for Window
			///This class is thrown when some critical errors are found during creating or destroying Window
			class WinEx: public Exception
				{
				public:
					///Constructor.
					///Sends specified message to its base class Exception
					///@param s message which will be available in exception from what() method
					WinEx(const std::string &s): Exception(s)	{}
				};		//class WinEx: public Exception

			///Holds essential information about mouse cursor.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///Reference to such a structure is returned by Window::mouse() method.
			struct MouseData
				{
				///Position of a mouse cursor in window area.
				POINTS pos;
				///Wheel position.
				///When the mouse wheel is rotated, this value is incremented or decremented. So if you want to
				///check whether the wheel was rotated, simply compare the old value with the current one,
				///and then save the current value as old.
				int wheel;
				///Boolean flag corresponding to left mouse button press state.
				bool lButton;
				///Boolean flag corresponding to right mouse button press state.
				bool rButton;
				///Constructor.
				///Initiates some structure objects.
				MouseData();
				};		//struct MouseData

			///Stores information about one key status.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			struct KeyData
				{
				///Indicates whether the key is pressed at the moment.
				bool pressed;
				///Indicates whether the key was pressed and then its status was read.
				bool read;
				///Constructor.
				KeyData(): pressed(false), read(false)	{}
				};		//struct KeyData

			///Holds the flags of the enabled extensions.
			///This bit field stores the information which extensions (i.e. fonts, textures)
			///were emabled in the constructor.
			const int extensionsFlags;
			///Table of bools which corresponds to keys.
			///If some key (for example 'A') is pressed at the moment, than key['A'] would be set to true.
			///@sa keyDown(int keyCode)
			static KeyData keys[256];
			///True if program is currently active (needs to be refreshed)
			static bool wActive;
			///Prevents creating multiple windows (undefined behaviour).
			///@sa Window(const Window &)
			static bool created;
			///Width of a created window.
			static int width_;
			///Height of a created window.
			static int height_;
			///Windows procedure, needed by WinAPI.
			///This static function handles Windows BlockAnalyzerMsg and sets wide variety of flags, for example keys, wActive, etc.
			static LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
			///Private GDI Device Context
			HDC hDC;
			///Permanent Rendering Context
			HGLRC hRC;
			///Holds Our Window Handle
			HWND hWnd;
			///Holds The Instance Of The Application
			HINSTANCE hInstance;
			///Window title.
			///This title is shown on a window bar at the top of the window and in the taskbar.
			///It is useless in fullscreen mode.
			const std::string title;
			///Pointer to extensions container class.
			///This object is created every time window is created (in initGL()) and destroyed. Access to it
			///can be gained using extensions() method.
			///in kill().
			///@sa initGL()
			///@sa kill()
			///@sa extensions()
			Extensions *extensions_;
			///Internal flag indicates whether we are in fullscreen or window mode.
			///@sa toggleFullscreen()
			bool fullscreen;
			///Copy constructor.
			///Copy construcotr is private to prevent creating another window by copy-construction.
			///@sa created
			Window(const Window &);
			///Stores current mouse position.
			///Mouse position is read from WM_MOUSEMOVE.
			///@sa mouse()
			static MouseData mouseData_;
		protected:
			///Setting up various data and performing many WinAPI calls.
			///This method is called whenever window is created (from constructor or during toggling fullscreen <-> window).
			///Function rewriten completely from NeHe. Most of the code is probably absolutely necessery.
			///After needed setup, function calls initGL()
			///@sa initGL()
			///@sa kill()
			///@sa Window()
			///@sa toggleFullscreen()
			///@throws Window::WinEx
			virtual void init();
			///Shuts down window.
			///Like init(), rewriten from NeHe.
			///See init() what's the purpose of making this method protected
			///@sa init()
			///@throws Window::WinEx
			virtual void kill();
			///Sets up every OpenGL settings you wish.
			///This is the place where you should load textures, fonts, etc.
			///@par Your own initGL()
			///Consider the fact that some of actions which are taken place in initGL() needs to be done every time you create a window
			///(so even when you run toggleFullscreen()). It means that every time you run toggleFullscreen() in your program,
			///you would have to load textures, etc. explicitly.
			///@par
			///You may avoid it by using inheritance: you inherit from class Window and writing your own initGL() method
			///(that's why it is protected). All you have to remember is to run Window::initGL() explicitly in you own initGL().
			///@par
			///This is an example of such a trick:
			///@code
			/// class MyWindow: public MyOGL::Window
			/// 	{
			/// 	private:
			/// 		void initGL();
			/// 	public:
			/// 		MyWindow(const string &iTitle, int iWidth, int iHeight, bool iFullscreen, int optionFlags = 0):
			/// 			MyOGL::Window(iTitle, iWidth, iHeight, iFullscreen, optionFlags)	{initGL();}
			/// 	};
			///
			/// void MyWindow::initGL()
			/// 	{
			/// 	MyOGL::Window::initGL();
			/// 	extensions().bitmapFonts().load("Courier New", 24);
			/// 	extensions().textures().load("texture.bmp", GL_LINEAR, GL_LINEAR);
			/// 	}
			///@endcode
			///Now MyWindow::initGL() will be called automatically - you don't have to remember about loading textures and bitmaps.
			///@sa toggleFullscreen()
			virtual void initGL();
		public:
			///Constructor.
			///Initializes window with given parameters.
			///Window is optimized for OpenGL.
			///@param iTitle Title of a window
			///@param iWidth Width of a window
			///@param iHeight Height of a window
			///@param iFullscreen True if you want to run fullscreen mode, otherwise false
			///@param iExtensionsFlags Which features you want to enable in your window (teztures, bitmap fonts, etc.)
			///@sa init()
			///@sa title
			///@sa height
			///@sa width
			///@sa fullscreen
			///@sa texturesEnabled
			///@sa bitmapFontsEnabled
			///@sa TEXTURES
			///@sa BITMAP_FONTS
			///@throws Window::WinEx
			Window(const std::string &iTitle, int iWidth, int iHeight, bool iFullscreen, int iExtensionsFlags = 0);
			///Destructor.
			///Destroys the window and cleans up OpenGL features.
			///@sa kill()
			virtual ~Window();
			///Refreshes the window.
			///@note This method automatically calls frame counter frame() method, so there is no need to call it explicitly.
			///So if you have FPS counter enabled, simply read its state whenever you want, you don't have to bother about it.
			virtual void refresh();
			///Returns information whether the key is pressed at the moment.
			///@param keyCode
			///@return True if specified key is hold down at the moment.
			///@sa keyPressed() and check the difference.
			///@sa keys
			///@sa wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
			virtual bool keyDown(int keyCode)	{return keys[keyCode].pressed;}
			///Returns information whether the key was pressed.
			///The difference between this and keyDown() method is the behaviour when the key is hold down
			///for a while. keyDown() returns true all the time the key is hold down. keyPressed() returns
			///true only once when the key state is checked for the first time. If the key is still hold down
			///down and keyPressed() is called again, it will return false (keyDown() will be still returning
			///true). keyPressed() will return true again only when you release the key and presse it again.
			///@sa keyDown()
			///@sa keys
			virtual bool keyPressed(int keyCode);
			///Toggles from fullscreen to windowed mode and vice versa.
			///Future mode depends on current window mode.
			///Toggle means first destroying the current window (kill()) and then creating a new one (init()).
			///You may toggle modes absolutely independent on the rest of your code, all settings will be saved
			///(see example in initGL() for more information).
			///@sa initGL()
			///@sa init()
			///@sa kill()
			virtual void toggleFullscreen();
			///Check if window is active.
			///This information may be usefull, for example does the window needs to be refreshed.
			///@return True if window is active, otherwise false
			///@sa wActive
			///@sa wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
			virtual bool active()	{return wActive;}
			///Gives access to OpenGL extensions.
			///@sa extensions_
			virtual MyOGL::Extensions& extensions()	{return *extensions_;}
			///Returns the current mouse position.
			///@return POINTS structure containg short int X and Y coordinates of mouse cursor.
			///@sa mouseData_
			///@sa wndProc()
			///@sa MouseData
			static const MouseData &mouse()	{return mouseData_;}
			///Returns window width.
			///@return Current window width in pixels.
			///@sa width_
			///@sa viewport(int left, int right, int top, int bottom, bool ortho)
			static int width()	{return width_;}
			///Returns window height.
			///@return Current window height in pixels.
			///@sa height_
			///@sa viewport(int left, int right, int top, int bottom, bool ortho)
			static int height()	{return height_;}
			///Setting up window viewport.
			///Using this function you may choose on which part of your screen you would like
			///put your OpenGL scene. This is usefull when you want to create multiple viewports,
			///for example to look at the same model from different sides. It can also create
			///an ordinary 3D model view or flat ortho view.
			///@warning This method does not perform any range checking so be carefull when
			///specifying parameters below.
			///@param left X corrdinate of the left egde of the viewport.
			///@param right X corrdinate of the right egde of the viewport.
			///@param top Y corrdinate of the top egde of the viewport.
			///@param bottom Y corrdinate of the bottom egde of the viewport.
			///@param ortho Create an ortho view viewport, defaults to false which will create ordinary
			///3D view.
			///@sa viewport()
			static void viewport(int left, int right, int top, int bottom, bool ortho = false);
			///Sets up default viewport.
			///Default viewport fills up the whole window; it is set up to model (3D) view. Call this
			///function when you want to reset all the 
			///@sa initGL()
			///@sa viewport(int left, int right, int top, int bottom, bool ortho)
			static void viewport()	{viewport(0, width(), height(), 0);}
		};		//class Window

//----------------------------------------------------------------------------

	}	//namespace MyOGL

//----------------------------------------------------------------------------

#endif

//----------------------------------------------------------------------------
