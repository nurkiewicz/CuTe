//----------------------------------------------------------------------------

///@file
///OpenGL scene with computer playing CuTe itself.
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
///This scene creates a new game but the whole control is done by the computer
///which plays the game using BlockAnalyzer class.

//----------------------------------------------------------------------------

#ifndef DEMO_H
#define DEMO_H

//----------------------------------------------------------------------------

#include <complex>
#include "scene.h"
#include "blockanalyzer.h"

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///Special engine class including class to analyze current block situation.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///This class inherits from GLEngine so it has all the routines to draw the game parts.
	class DemoEngine: public GLEngine
		{
		private:

			///Specialized BlockAnalyzer class for quasi-technical info messages.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///The class process the current block using base class, but it can also save and draw all
			///information connected to block processing.
			class BlockAnalyzerMsg: public BlockAnalyzer
				{
				public:
					///Constructor.
					///Only saves some essential information.
					///@param parent Parent engine class, needed by the base class BlockAnalyzer
					///@param iFonts Bitmap fonts used in message output
					///@sa draw()
					BlockAnalyzerMsg(GLEngine& parent, MyOGL::BitmapFonts& iFonts);
					///Draws the messages onto the screen.
					///Puts all the messages (maximum of ALL_MESSAGES_COUNT) onto the screen using the
					///bitmap fonts passed in the constructor.
					void draw();
				private:
					///How many messages can be stored at the moment.
					///When there are more than ALL_MESSAGES_COUNT messages saved in msgs variable, the oldest
					///message is deleted automatically.
					///@sa msgs
					///@sa insert()
					static const int ALL_MESSAGES_COUNT = 16;
					///After this time the oldest message is removed.
					///When MESSAGE_VALIDITY_TIME time elapsed since last message was added, the oldest message
					///is removed. Thanks to this the message don't last too long if no new message are added.
					///<br>Time is in ms.
					///@sa update()
					static const int MESSAGE_VALIDITY_TIME = 700;
					///Distance between two neighbouring analyzer messages.
					///This distance is used when drawing and animating the messages list.
					///@sa draw()
					static const float VERTICAL_DIST;
					///Type definition of list with analyzer messages.
					///The list contains std::pair object with both strings. The first string is the message
					///time and the second is the message itself.
					///@sa msgs
					typedef std::list<std::pair<const std::string, const std::string> > msgList;
					///Analyzer messages to be collected and drawn in the future.
					///This list stores a maximum of ALL_MESSAGES_COUNT messages.
					///@sa insert()
					///@sa draw()
					msgList msgs;
					///Messages list vertical position.
					///By updating this value every frame in update() we create the messages list animation.
					///@sa VERTICAL_DIST
					///@sa update()
					float vertPosShift;
					///Inserts new message at the end of the messages list.
					///@param msg Message string (second field of std::pair stored in msgs list); first field
					///(game time) is set to the current time.
					///@sa msgs
					void insert(const std::string& msg);
					///Converts the analyzer best block position into message string.
					///Reads the best analyzer block position found and converts it into a string with the
					///following pattern:
					///@verbatim
					///(x, y): [factor] rotation_codes
					///@endverbatim
					///@return String with best analyzer info
					std::string bestStr() const;
					///Updates message list.
					///This method is responsible for checking if whether the last message wasn't added
					///later than MESSAGE_VALIDITY_TIME. If it was, removes the oldest message. Also
					///checks whether the analyzer state didn't changed and if it did, puts a message informing
					///about it. If analyzer is processing at the moment, inserts a message about current
					///best block position found (but only if it changed since last update() call).
					///@sa timer
					///@sa bestStr()
					void update();
					///Timer to use in update()
					///This timer measures the time since last message was added. This information is used
					///in update()
					///@sa update()
					MyOGL::Timer timer;
					///Bitmap fonts to use when outputing messages.
					///Messages are drawn using very small bitmap font. This reference must be given
					///in constructor.
					MyOGL::BitmapFonts& fonts;
					///Changes the rotation code to its string representation.
					///Rotation code can be 'x', 'y' or 'z' both lower- and uppercase. For example the string
					///for 'X' is "XCCW".<br>
					///This function is used to when adding information about analyzer processes or currently
					///done rotations.
					///@sa rotateBlock()
					static const std::string codeToDirection(char axis);
					///Overriden BlockAnalyzer::rotateCurrentBlock() for message output.
					///This function does not perform any rotations, it only adds a message about rotation
					///done in base class.
					bool rotateCurrentBlock(char axis);
					///Overloaded state() method from BlockAnalyzer.
					///By overloading this method BlockAnalyzerMsg can gain the information about changing the
					///analyzer state.
					void state(int newState);
				};		//class BlockAnalyzerMsg: public BlockAnalyzer

			///Used to start processing the current block.
			///When this function is called by virtual function mechanism it means that the current
			///block was replaced with next. Right after that the analyzer object should start analysing
			///the current block position.
			///@sa analyzer
			void switchBlocks();
			///Takes care about analyzer processing and block moving.
			///If analyzer state is idle, it means that it finished processing and there's nothing left
			///to do than than just push it forward as far as possible (when it encounter cuboid cubes
			///switchBlocks() will be called automatically and the processing will start again); otherwise
			///the BlockAnalyzer::process() method will be called to continue analyzing or transforming
			///the block
			///@sa analyzer
			void update();
			///Block analyzer + message output.
			///This object is essential for whole demo. The object process the current block, transforms
			///its positon, rotates it and saves analyzer messages.
			///@sa update()
			BlockAnalyzerMsg analyzer;
			///Restart demo scene flag.
			///This flag can be set in two places: in update() when the game should be restarted.
			///This is caused by the GAMEOVER state of the analyzer. The second place is gameOver()
			///overriden function when the game engine sygnalizes game over.
			///@sa update()
			///@sa gameOver()
			bool restart_;
			///Overriden Engine::gameOver().
			///When game engine sygnalizes game over running this method, the demo game should be
			///restarted.
			///@sa restart_
			void gameOver()	{restart_ = true;}
		public:
			///Constructor, only saves some variables.
			///@param difficulty Information about how big the cuboid should be and what blocks set to use.
			///@param iExtensions Access to all OpenGL stuff needed in drawing.
			DemoEngine(const Difficulty& difficulty, MyOGL::Extensions& iExtensions);
			///Additional draw method for analyzer message output.
			///Analyzer is internal part of DemoEngine, but its derived class BlockAbalyzerMsg gives
			///access to messages drawing routine, which should be called from external scene class.
			///@sa analyzer
			void drawBlockAnalyzerMsg()	{analyzer.draw();}
			///Returns the restart flag.
			///@return restart_ flag to indicate the external environment that the scene should be restarted.
			bool restart()	{return restart_;}
		};		//class DemoEngine: public GLEngine

//----------------------------------------------------------------------------

	///OpenGL scene with demo game.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///This scene renders game similar to the normal "New game" panel. Some parts are not shown
	///(points, progress bars), the whole sidebar is on the left and there is additional info message
	///panel.
	class Demo: public CuTeScene
		{
		private:

			///Demo scene camera controller object.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///This object changes the camera viewport periodicly to random position.
			class Camera: public GLEngine::Camera
				{
				private:
					///Object timer.
					///This timer is used to count the time which have elapsed since last camera position
					///change.
					///@sa nextUpdateTime
					///@sa update()
					MyOGL::Timer timer;
					///Time (in ms) to the next camera position update.
					///If the time stored in timer object exceeds this time, the camera position is set
					///to some random new position.
					int nextUpdateTime;
					///Updates the camera position.
					///Calls the base class version which performs the camera position animation. If the
					///time stored in timer exceeds the nextUpdateTime, randomizes some new camera position.
					void update();
				public:
					///Constructor.
					///@param difficulty Stores the demo game size and depth; this value is used to properly
					///animate and position the camera.
					Camera(const Difficulty& difficulty);
				};

			///CuTe engine for handling internall game routines (including block analysis).
			///DemoEngine is a special class which also handles all the job connected to analysing
			///current block combinations.
			///@sa drawEngine()
			DemoEngine engine;
			///Scene refreshing method.
			///Refreshes the scene which is checking for input and drawing all the game parts
			///(cuboid, next block, messages, etc.)
			void refresh();
			///Draws the main game engine parts.
			///Sets the viewport and draws the game engine: cuboid with cubes and current block.
			void drawEngine();
			///Camera controller object.
			///This object is used to position the camera properly in demo scene.
			Camera camera;
			///Draws next block preview.
			///Sets the viewport to lower left corner and draws the next block preview using the
			///engine's method.
			///@sa engine
			void drawNextBlock();
			///Draws game info panel.
			///Info panel includes game logo (drawBackground() method), time and messages list.
			///@sa drawBackground()
			void drawInfo();
			///Draws the CuTe logo.
			///It is the same logo as in the normal game - to distinguish those scenes in normal game
			///logo with the whole panel is drawn on the right, here on the left.
			void drawBackground();
		public:
			///Only sets up some variables and pushes them to the base class.
			///@param parentWindow Parent Window object which is needed by base class Scene.
			///@param difficulty Information about demo game sizes and blocks set.
			Demo(CuTeWindow &parentWindow, const Difficulty& difficulty);
			///Sets back the default viewport.
			~Demo()	{win.viewport();}
		};

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif		//#define DEMO_H

//----------------------------------------------------------------------------
