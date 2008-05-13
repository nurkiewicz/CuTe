//----------------------------------------------------------------------------

///@file
///CuTe main game sidebar view classes.
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
///This file contains some classes which are used to view the sidebar
///on the right side of the game.

//----------------------------------------------------------------------------

#ifndef SIDEBAR_H
#define SIDEBAR_H

//----------------------------------------------------------------------------

#include <string>
#include "MyOGL/window.h"
#include "common.h"
#include "difficulty.h"

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///Takes cares about the right game side bar.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Aug 2005
	///@par
	///The sidebar shows all the information connected to the game, like points counter, game time
	///some fancy progress bars and additionally game logo.
	///@note Although the next block preview lies on a sidebar, it is not displayed using this class.
	///For preview check special class GLEngine::NextBlockPreview.
	class SideBar
		{
		private:


			///Graphic progress bar in OpenGL
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///This class gives whole interface to view nice looking 2D progress bars in OpenGL
			///environment.
			class FloatingBar
				{
				private:
					///Width of a progress bar.
					static const float WIDTH;
					///Height of a progressbar.
					static const float HEIGHT;
					///Maximum position on  aprogress bar.
					///The progressbar can graphically show the values in range <0.0; MAX_POS>
					///@sa set()
					static const float MAX_POS;
					///Speed of progress bar value changes.
					///When you change the value in progress bar, it won't change rapidly, but will be
					///changing smoothly with this speed. This makes a nice ipression of a floating values.
					const float speed;
					///Hue of a bar.
					///You can specify the main color scheme for a bar. This value (in radians) tells which
					///color to use in bar (0.0 - red, 2/3 pi - blue, etc.)
					const float hue;
					///X coordinate of a progress bar position.
					///This is the coordinate of the middle of the bar.
					///@sa barPosY
					///@sa barAngleZ
					const float barPosX;
					///X coordinate of a progress bar position.
					///This is the coordinate of the middle of the bar.
					///@sa barPosX
					///@sa barAngleZ
					const float barPosY;
					///Angle around the Z axis.
					///The progressbar will be rotated with this angle around Z axis counter clockwise.
					///@note If the angle is 0.0, the bar will be vertical and its 0 position will laid at
					///the bottom.
					///The angle is in degrees.
					const float barAngleZ;
					///Position on the progress bar.
					///Can be in range <0.0; MAX_POS>. Because set() method checks the this range, it should never
					///be exceeded.
					///@sa set()
					float pos;
					///Bar position shift.
					///This value is used to perform smoothly changing of a progress bar position.
					///@sa speed
					float shift;
					///Floating bar timer.
					///Is used in update() method to correctly (independently to how often update() is called)
					///count the position shifts.
					///@sa update()
					MyOGL::Timer timer;
					///Calculates the current position shift.
					///Thanks to this method progress bar positions are changing smoothly
					///@sa shift
					void update()	{CuTe::decAbs(shift, timer.restart() / 1000.0f * speed);}
				public:
					///Constructor.
					///Sets some important values and constants.
					FloatingBar(float iBarPosX, float iBarPosY, float iBarAngleZ, float iHue, float iSpeed);
					///Draws the bar.
					///The bar position and rotation are saved in the object and set just before drawing.
					///Before drawing the bar the shift value is updated using update() private method.
					///@sa update()
					///@sa draw(float newPos)
					void draw();
					///Overloaded draw() version.
					///This version of draw() method changes the bar position before drawing, which might
					///save you some time while writing the program.
					///@sa draw()
					void draw(float newPos)	{set(newPos); draw();}
					///Sets the progress bar position.
					///@param newPos New progress bar position. Should be in range <0.0; MAX_POS>
					///which actually is <0.0; 1.0>. If your newPos will be lower than 0.0, position 0.0
					///will be set. If you exceed 1.0, 1.0 will be set as well.
					///@sa MAX_POS
					void set(float newPos);
				};

			///Points counter display.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Aug 2005
			///@par
			///This class shows the point count using outline fonts. For the future where maybe the
			///display will be more sophisticated I've decided to keep it its own class.
			class ScoreDisplay
				{
				private:
					///Acceleration at which the value on display changes.
					///When the amount of points changes, the value displayed on the screen doesn't
					///switch rapidly from one to another. It changes smoothly through the values between
					///old and new points count.
					///@par
					///This time I've thought that using constant acceleration would give much prettier
					///effect than constant speed. Thanks to that value on the display first moves slowly but
					///then its change speed increases.
					///@sa update()
					static const float DISPLAY_CHANGE_ACCELERATION;
					///Updates currently displayed value.
					///This method is needed to create this smooth value changing.
					///@sa DISPLAY_CHANGE_ACCELERATION
					void update(int points);
					///OutlineFonts object for drawing.
					///draw() method prints the current score using outline fonts, so this object must have
					///access to MyOGL::OutlineFonts object.
					///@sa MyOGL::OutlineFonts
					MyOGL::OutlineFonts &outlineFonts;
					///Previous points count.
					///When the current score is different from the one stored here, it means that the user
					///got some new points. This ables the class to start smooth display changing.
					///@sa update()
					int oldPoints;
					///Points value shift.
					///To create smooth points value changing, this variable changes a bit every time
					///update() is called.
					///@note Although score is integral value, to correctly and precisly count the shift
					///it is internallt floating point (and cut to int at the end).
					float pointsShift;
					///Object timer.
					MyOGL::Timer timer;
				public:
					///Constructor.
					ScoreDisplay(MyOGL::OutlineFonts &iOutlineFonts);
					///Draws the points counter.
					///Position and angles should be set before calling this method.
					void draw(int points);
				};

			///Floating bar for distance.
			///Red bar showing the distance between current block and cuboid.
			FloatingBar distBar;
			///Floating bar for forward auto move time.
			///Green bar showing how much time left to automatic block forward move.
			FloatingBar forwardMoveBar;
			///Floating bar for speed level change time.
			///Blue bar showing how much time left to change to the bigger speed.
			FloatingBar speedTimeBar;
			///Draws the game logo at the top of the sidebar.
			void drawBackground();
			///Window extensions.
			///Reference to MyOGL::Extensions gives the side bar access to many MyOGL extensions
			///such as textures, bitmap and outline fonts, etc.
			MyOGL::Extensions& extensions;
			///ScoreDisplay object.
			///Score display is a part of a side bar so it is nested in the object.
			///SideBar only needs to call ScoreDisplay::draw() method.
			ScoreDisplay scoreDisplay;
			///Shows various less important information.
			///This information is game speed level, time, FPS counter, etc.
			///@param speed Game's current speed level
			///@param gameTime How long is the game running (in 1/10 sec.)
			void showOtherData(int speed, int gameTime);
			///Cuboid size and depth info string.
			///This string contains information about cuboid size and depth in format: SIZExSIZExDEPTH.<br>
			///This string is shown on a side bar.
			///@sa SideBar()
			const std::string sizeStr;
		public:

			///Contains all the game info which should be shown on the sidebar.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Aug 2005
			///@par
			///Reference to this structured is passed to the SideBar object's draw() method.
			///I found this solution better than giving the SideBar object some GLEngine or Game
			///reference and make it a friend so it could get this information by itself.
			struct GameInfo
				{
				///Current score in the game.
				int points;
				///How much is the game currently running (in 1/10 seconds).
				int gameTime;
				///Game speed level (in range <0; 9>
				int speed;
				///Relative distance between current block and the cuboid.
				///This value can be in range 1.0 (furthest) and 0.0 (closest).
				float dist;
				///Relative time to next speed level change.
				///This value can be in range 1.0 (the change has just been done) and 0.0 (will be done in
				///a second).
				float speedChangeTime;
				///Relative time to next automatic block moving forward.
				///This value can be in range 1.0 (the move has just been done) and 0.0 (will be done in
				///a second).
				float forwardMoveTime;
				};

			///Constructor.
			///@param difficulty Contains information about cuboid's size and depth, which is needed
			///to set up @ref sizeStr string.
			///@param iExtensions MyOGL::Extensions object needed mostly for fonts access in side bar.
			///@sa sizeStr
			SideBar(const Difficulty& difficulty, MyOGL::Extensions& iExtensions);
			///Draws the sidebar.
			///@param info All game information which should be placed on the sidebar.
			void draw(const GameInfo &info);
		};

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif

//----------------------------------------------------------------------------
