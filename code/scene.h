//----------------------------------------------------------------------------

///@file
///CuTe window and scene classes.
///
///@par License:
///@verbatim
///CuTe - Cubic Tetris, OpenGL 3D Tetris game clone.
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
///This file contains CuTe window and scene classes, which are used commonly along the whole game.

//----------------------------------------------------------------------------

#ifndef CUTE_WINDOW_H
#define CUTE_WINDOW_H

//----------------------------------------------------------------------------

#include <boost/array.hpp>
#include "MyOGL/scene.h"
#include "point.h"

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///MyOGL::Window inherited window class.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///Specified for this game, loads all the necessery data (textures, fonts), etc. for the game logic.
	class CuTeWindow: public MyOGL::Window
		{
		public:
			///Window 800x600 video mode.
			///@sa CuTeWindow()
			static const int W_800x600 = 0;
			///Window 1024x768 video mode.
			///@sa CuTeWindow()
			static const int W_1024x768 = 1;
			///Fullscreen 800x600 video mode.
			///@sa CuTeWindow()
			static const int F_800x600 = 2;
			///Fullscreen 1024x768 video mode.
			///@sa CuTeWindow()
			static const int F_1024x768 = 3;
			///Constructor.
			///Passes all the necessery arguments to the base class Window.
			///@param iMode Chooses the desired video mode. This should be one of the flags: W_800x600,
			///W_1024x768, F_800x600, F_1024x768
			///@sa mode()
			CuTeWindow(int iMode);
			///Returns the window video mode.
			///The video mode is given to the class in the constructor.
			///@return Video mode flag (should be one of: W_800x600, W_1024x768, F_800x600, F_1024x768)
			///@sa mode_
			int mode() const	{return mode_;}
		protected:
			///Inits some additional OpenGL features.
			///This function must be called whenever the window is created (at the beginning and
			///when it is created and when the fullscreen mode is toggled). It loads textures
			///but ommits loading the fonts - they're loaded after the intro animation
			///is performed
			///@sa MainMenu::MainMenu() constructor.
			void initGL();
			///Window video mode.
			///This value is a parameter of the constructor. It can be gained using mode() method.
			///@sa mode()
			const int mode_;
		};		//class CuTeWindow: public MyOGL::Window

//----------------------------------------------------------------------------

	///OpenGL scene inherited from MyOGL class.
	///A seperate class is needed only for storing a copy of parent window reference.
	///This reference is usefull because it stores more specified type CuTeWindow rather than
	///simple MyOGL::Window.
	class CuTeScene: public MyOGL::Scene
		{
		public:
			///Creates a new scene.
			///This construcotr doesn't do anything particulary important - it only calls
			///the base class constructor and saves the win reference to store tha parent window.
			CuTeScene(CuTeWindow& parentWindow): MyOGL::Scene(parentWindow), win(parentWindow)	{}
			///Draws a cube wall orthogonal to the X axis.
			///@param x X coordinate of the center of the wall.
			///@param y Y coordinate of the center of the wall.
			///@param z Z coordinate of the center of the wall.
			///@param scale Length of a wall border.
			///@sa drawCube()
			static void drawCubeWallX(double x, double y, double z, double scale = 1.0);
			///Draws a cube wall orthogonal to the Y axis.
			///@param x X coordinate of the center of the wall.
			///@param y Y coordinate of the center of the wall.
			///@param z Z coordinate of the center of the wall.
			///@param scale Length of a wall border.
			///@sa drawCube()
			static void drawCubeWallY(double x, double y, double z, double scale = 1.0);
			///Draws a cube wall orthogonal to the Z axis.
			///@param x X coordinate of the center of the wall.
			///@param y Y coordinate of the center of the wall.
			///@param z Z coordinate of the center of the wall.
			///@param scale Length of a wall border.
			///@sa drawCube()
			static void drawCubeWallZ(double x, double y, double z, double scale = 1.0);
			///Draws a cube.
			///Draws all the 6 cube walls by calling the appriopriate functions.
			///@param x X coordinate of the center of a cube.
			///@param y Y coordinate of the center of a cube.
			///@param z Z coordinate of the center of a cube.
			///@param scale Length of a wall border.
			///@sa drawCubeWallX(), drawCubeWallY(), drawCubeWallZ()
			static void drawCube(double x, double y, double z, double scale = 1.0);
			///Draws a cube.
			///@param pos Position (in 3D) of the center of a cube.
			///@param scale Length of a wall border.
			///@sa drawCube(double x, double y, double z, double scale)
			static void drawCube(const Point<double, 3>& pos, double scale = 1.0)
				{drawCube(pos.x(), pos.y(), pos.z(), scale);}
		protected:
			///Parent CuTe window.
			///Base class has the variable win, but in the base class its type is MyOGL::Window. I wanted to
			///have access to some special methods available only in derived CuTeWindow class (like mode())
			///so I've decided to have additional reference, this type to the CuTeWindow type.
			CuTeWindow& win;
		};

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif		//#define CUTE_WINDOW_H

//----------------------------------------------------------------------------
