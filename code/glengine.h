//----------------------------------------------------------------------------

///@file
///CuTe game main engine OpenGL wrapper classes.
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
///Those classes store additional information needed to fully animate
///all parts of the game.

//----------------------------------------------------------------------------

#ifndef GLENGINE_H
#define GLENGINE_H

//----------------------------------------------------------------------------

#include <list>
#include "MyOGL/window.h"
#include "engine.h"

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///Definition of Line type.
	///This type is used when storing displaying grid. It stores two 3D points.
	///@sa Game::grid
	typedef std::pair<Point<double, 3>, Point<double, 3> > Line;

//----------------------------------------------------------------------------

	///Floating point wrap for main game engine.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///This class performs some additional tasks and stores information which are needed
	///to properly animate and draw Engine object data. It takes care about all the floating point
	///position (in Engine object all coordinates are fixed to integer) and does all this
	///additional stuff to make everything look nice.
	class EngineExt: public Engine
		{
		private:
			///Speed of block when it is moved (excluding forward move)
			///Measured in units per second, where one unit is the length of a cube border.
			///@sa update()
			///@sa ROTATION_SPEED
			///@sa MOVE_FORWARD_SPEED;
			static const float MOVE_SPEED;
			///Speed of block when it is moved forward.
			///Block moves forward much faster than along X and Y axis.
			///Measured in units per second, where one unit is the length of a cube border.
			///@sa update()
			///@sa MOVE_SPEED
			static const float MOVE_FORWARD_SPEED;
			///Rotation speed of block when it is rotated.
			///Speed is measured in degrees per second.
			///@sa update()
			///@sa MOVE_SPEED
			static const float ROTATION_SPEED;
			///Current block minimal alpha.
			///Current block changes its alpha value from MINIMAL_ALPHA to MAXIMAL_ALPHA. When it is
			///close to the user, cube walls are almost completly transparent (MINIMAL_ALPHA) and the
			///grid borders are visible. When the block reaches the end of a cuboid, this proportions
			///are reverted
			///@sa drawBlock()
			static const float MINIMAL_ALPHA;
			///Current block maximal alpha.
			///@sa MINIMAL_ALPHA
			static const float MAXIMAL_ALPHA;
			///Specifies how fast does the block blend value change.
			///When the distance between block cubes and cuboid changes, simultanously its alpha value
			///should change. This change like everything is suppose to go smoothly with this speed.
			///@sa blockAlphaShift
			///@sa blockAlpha
			static const float BLOCK_BLEND_SPEED;
			///Speed at which the planes vanishing goes on.
			///On the first phase of removing filled planes (see removeFilledPlanes() for more details)
			///whole planes are fanishing (becoming more translucent). This process comes up with
			///this speed.
			///@sa removeFilledPlanes()
			static const float PLANES_BLEND_SPEED;
			///Speed level changing average period.
			///When your playing time is greater than this time, the speed level is increased and the
			///counter is reseted.
			///@note This time is average period. Actual time to next speed level change is
			///randomized for every level and can be greater or lower up to SPEED_CHANGE_VARIATION
			///from this value.
			///@sa randomSpeedChangePeriod()
			///@sa SPEED_CHANGE_VARIATION.
			///@sa increaseSpeed() for information what is speed level
			///@sa updateTimes()
			static const int SPEED_CHANGE_PERIOD = 4 * 60 * 1000;
			///Speed change time variation.
			///SPEED_CHANGE_PERIOD is just the average value. The actual value can be in range:
			///<SPEED_CHANGE_PERIOD - SPEED_CHANGE_VARIATION; SPEED_CHANGE_PERIOD + SPEED_CHANGE_VARIATION>
			///and is calculated randomly for every speed level.
			///@sa randomSpeedChangePeriod()
			///@sa SPEED_CHANGE_PERIOD
			///@sa increaseSpeed()
			static const int SPEED_CHANGE_VARIATION = static_cast<int>(SPEED_CHANGE_PERIOD * 0.2);
			///Time to next speed change for the current speed level.
			///This value is generated randomly for every level.
			///@sa randomSpeedChangePeriod()
			///@sa increaseSpeed()
			///@sa SPEED_CHANGE_PERIOD
			///@sa SPEED_CHANGE_VARIATION.
			int speedChangePeriod;
			///@sa speedChangePeriod
			///@sa SPEED_CHANGE_PERIOD
			///@sa SPEED_CHANGE_VARIATION.
			static int randomSpeedChangePeriod()
				{return static_cast<int>(SPEED_CHANGE_PERIOD + SPEED_CHANGE_VARIATION * ((rand() % 1024) / 512.0 - 1));}
			///Period of time between two automatic block forward moves at speed 0.
			///If the user doesn't push the forward button for a while, block is pushed forward automatically.
			///How often ot happens, in depednds on this and @ref MOVE_FORWARD_PERIOD_MIN values.
			///@par
			///At the speed level of 0 the time interval between those automatic forward moves is equal to
			///SPEED_CHANGE_PERIOD. At level 9 it is decreased to MOVE_FORWARD_PERIOD_MIN. At the rest
			///of speed levels, this interval is calculated using exponential function e ^ -Ax
			///@sa MOVE_FORWARD_PERIOD_MIN
			///@sa increaseSpeed()
			///@sa updateTimes()
			static const int MOVE_FORWARD_PERIOD_MAX = 60 * 1000;
			///Period of time between two automatic block forward moves at speed 9.
			///@sa MOVE_FORWARD_PERIOD_MAX for comprehensive information.
			///@sa increaseSpeed()
			///@sa updateTimes()
			static const int MOVE_FORWARD_PERIOD_MIN = 100;
			///Alpha value coefficient.
			///Alpha value of a current block is calculated using exponential function e^ax, where 'a'
			///is the ALPHA_COEFF. This coefficient is calculated depending on values of MINIMAL_ALPHA
			///and MAXIMAL_ALPHA and is used in blockAlpha.
			///@sa blockAlpha
			const float ALPHA_COEFF;
			///Current block alpha value.
			///This alpha value is calcuated in base of the distance between current block and cuboid.
			///@sa blockAlphaShift
			///@sa blockAlpha()
			float blockAlpha_;
			///Current block alpha value correction.
			///If the alpha value of a current block was based only on the distance to the cuboid
			///it might sometimes occur that the block alpha would change very rapidly creating nasty
			///effect. Thanks to this correction alpha value always changes smoothly.
			///@sa update()
			///@sa blockAlpha()
			float blockAlphaShift;
			///Alpha value of currently removing planes.
			///When currently removing planes are translucent, this variable stores its alpha value.
			///@sa planesAlpha()
			///@sa update()
			float planesAlpha_;
			///Position floating point shifts.
			///Although block position is stored in base class Engine (in Block to be precise), it is
			///integer and that is not enough to create smooth animated movement in OpenGL. This floating
			///point shifts solve the problem - they are much more precise and when they're added to
			///ordinary integer positions, we can create really nice looking animation.
			///@sa drawBlock()
			///@sa angleShift
			///@sa MOVE_SPEED
			Point<float, 3> posShift;
			///Angle floating point shifts.
			///In Engine base class angles aren't even mentioned: because whole game is cubic, all the angles
			///are multipliity of 90 degrees. But when we are animating block rotation, we don't wan't
			///the block simply to jump from one angle to another (90 degrees). That's why the
			///angle shifts are introduced - thanks to them block rotating smoothly.
			///@sa drawBlock()
			///@sa posShift
			///@sa ROTATION_SPEED
			Point<float, 3> angleShift;
			///Z planes individual Z axis shifts.
			///When planes are removed (second phase), they are smoothly moved down thru Z axis.
			///This table stores their Z shift position (individually for every plane) so that the move
			///is goes smooth.
			std::vector<double> cuboidPlanesShift;
			///Game timer.
			///It is used when calculating shifts of movements and rotations.
			///@sa update()
			MyOGL::Timer timer;
			///Speed changing timer.
			///This timer stores the time which have elapsed since last speed level change.
			///It is reseted every time it exceed SPEED_CHANGE_PERIOD
			///@sa SPEED_CHANGE_PERIOD
			///@sa updateTimes()
			MyOGL::Timer speedChangeTimer;
			///Auto moving forward timer.
			///This timer stores the time which have elapsed since last moving the block
			///forward was performed (no matter if it was done by the player or automatically).
			///If this time exceed moveForwardPeriod, block is moved forward automatically (which
			///causes reseting this timer too).
			MyOGL::Timer moveForwardTimer;
			///Whole game time.
			///This timer stores the time how long the current game is running.
			///@sa gameTime();
			MyOGL::Timer gameTimer;
			///The current block is drawn in two phases: grid borders and textured walls. The first phase
			///requires to create a set of line which corresponds to every block edge. This job is done
			///by generateBlockGrid() and those lines coordinates are stored in this list.
			///@sa generateBlockGrid()
			///@sa Line
			///@sa grid()
			std::list<Line> grid_;
			///Smart grid generation.
			///This function generates a set of border grid coordinates on a base of current block cubes.
			///@sa grid_
			void generateBlockGrid();
			///Performs some additional tasks if block was moved succesfully during rotation.
			///See @ref Engine::tryMove() for more details about how tryMove() works. This overriden
			///version apart from calling its base class version performs some additional calculations
			///to animate the block properly. This jobs needs to be done when the block was move by
			///Engine::tryMove() and the posShift needs to be set in order to animate this additional
			///move.
			///@param block Block that was rotated before.
			///@sa Engine::tryMove()
			bool tryMove(Block &block);
			///Sets up additional variables to make vanishing of planes smooth.
			///Removed blocks are vanishing in two steps: first they become more and more translucent
			///and when they finally become transparent, planes which were not removed and are located
			///over them are smoothly moved down.
			///@sa removingPlanes
			void removeFilledPlanes();
			///Indicates that removed planes are smoothly become transparent.
			///This flag is set only when the first phase of removing planes occurs (see removeFilledPlanes()
			///for more details). It is a signal to the EngineExt object
			///not to move the planes (second phase) and to ignore all user actions (they are active in
			///a second phase)
			///@sa removeFilledPlanes();
			bool removingPlanes;
			///Current game speed.
			///This can be a value between 0 and 9, where 0 is the slowest and 9 is the fastest.
			///@sa increaseSpeed() for more details about game speed
			int speed_;
			///Time after which the block moves forward automatically.
			///When the time since last moving forward the block exceed this value, block will be moved
			///forward automatically.
			///@par
			///This period of time is stricly connected to speed_ value - the bigger speed, the lower this
			///time. And the lower this value - the tougher the game.
			int moveForwardPeriod;
			///Updates game times.
			///This method checks the speedChangeTimer and moveForwardTimer and if they exceed their
			///maximum values (adequately SPEED_CHANGE_PERIOD and moveForwardPeriod), they are reseted and
			///additional tasks are done (see those timers detailed information).
			///@sa update()
			///@sa moveForwardPeriod
			///@sa speedChangePeriod
			void updateTimes();
			///Increases speed level.
			///When speedChangePeriod exceeds SPEED_CHANGE_PERIOD and we are not in the last (9) speed level
			///the level is increased. This means that the time interval moveForwardPeriod will be
			///exponentially decreased, which will cause the game to be more difficult (as it will be
			///running faster).
			///@sa update()
			///@sa speedChangeTimer
			///@sa moveForwardPeriod
			void increaseSpeed();
			///Informs whether the block is currently rotating.
			///When the block is rotating around around any axis, it shouldn't be rotated around any other.
			///There is no need for such a feature because the blocks are rotating fast enough, besides
			///rotating around two different axises at a time sometimes produces strange animation
			///behaviour.
			///@return True if the block is rotating around any axis at a time.
			///@sa All rotate*() functions
			bool rotating() const	{return (angleShift.x() != 0.0) || (angleShift.y() != 0.0) || (angleShift.z() != 0.0);}
		protected:
			///Mostly generates new grid after blocks were switched.
			void switchBlocks();
			///Mostly updates data in EngineExt object.
			///Sets up new shifts for currentBlock so that in next frame it will move
			///and rotate a bit (of course only if it was during rotation or movement).
			///Besides checks the game speed timers using updateTimes().
			///@sa updateTimes()
			virtual void update();
			///Returns current block grid.
			///@return Reference to a list of line coordinates - needed by the environment to
			///draw the block grid.
			///@sa grid
			///@sa generateBlockGrid()
			///@sa Game::drawBlockGrid()
			const std::list<Line> &grid() const	{return grid_;}
			///Overriden Engine::pause() method.
			///When the game is overed, all block animations should be freezed. This is done by calling
			///pause() method in this function.
			void gameOver()	{pause(true);}
		public:
			///Constructor.
			///Creates Engine object with specified size and depth.
			///@param difficulty Stores information about the game difficulty, which are the game cuboid
			///size, depth and the desired blocksSet. All this information is used when creating game
			///cuboid in Engine object.
			///@sa Engine::Engine()
			EngineExt(const Difficulty& difficulty);
			///Moves block right if it is possible.
			///@return Always true. Pleas note that the return value is left only because of compatibility
			///with base class Engine::moveRight() version. In derived class version this boolean return
			///value is useless.
			///@sa Engine::moveRight() for more details.
			bool moveRight();
			///Moves block left if it is possible.
			///@return Always true. Pleas note that the return value is left only because of compatibility
			///with base class Engine::moveLeft() version. In derived class version this boolean return
			///value is useless.
			///@sa Engine::moveLeft() for more details.
			bool moveLeft();
			///Moves block up if it is possible.
			///@return Always true. Pleas note that the return value is left only because of compatibility
			///with base class Engine(MOVE_UP)() version. In derived class version this boolean return
			///value is useless.
			///@sa Engine(MOVE_UP)() for more details.
			bool moveUp();
			///Moves block down if it is possible.
			///@return Always true. Pleas note that the return value is left only because of compatibility
			///with base class Engine::moveLeft() version. In derived class version this boolean return
			///value is useless.
			///@sa Engine::moveDown() for more details.
			bool moveDown();
			///Moves block forward if it is possible.
			///@return Always true. Pleas note that the return value is left only because of compatibility
			///with base class Engine::moveForward() version. In derived class version this boolean return
			///value is useless.
			///@sa Engine::moveForward() for more details.
			bool moveForward();
			///Rotates block around X axis clockwise if it is possible.
			///@return Always true. Pleas note that the return value is left only because of compatibility
			///with base class Engine::rotateXCW() version. In derived class version this boolean return
			///value is useless.
			///@sa Engine::rotateXCW() for more details.
			bool rotateXCW();
			///Rotates block around X axis counterclockwise if it is possible.
			///@return Always true. Pleas note that the return value is left only because of compatibility
			///with base class Engine::rotateXCCW() version. In derived class version this boolean return
			///value is useless.
			///@sa Engine::rotateXCCW() for more details.
			bool rotateXCCW();
			///Rotates block around Y axis clockwise if it is possible.
			///@return Always true. Pleas note that the return value is left only because of compatibility
			///with base class Engine::rotateYCW() version. In derived class version this boolean return
			///value is useless.
			///@sa Engine::rotateYCW() for more details.
			bool rotateYCW();
			///Rotates block around Y axis counterclockwise if it is possible.
			///@return Always true. Pleas note that the return value is left only because of compatibility
			///with base class Engine::rotateYCCW() version. In derived class version this boolean return
			///value is useless.
			///@sa Engine::rotateYCCW() for more details.
			bool rotateYCCW();
			///Rotates block around Z axis clockwise if it is possible.
			///@return Always true. Pleas note that the return value is left only because of compatibility
			///with base class Engine::rotateZCW() version. In derived class version this boolean return
			///value is useless.
			///@sa Engine::rotateZCW() for more details.
			bool rotateZCW();
			///Rotates block around Z axis counterclockwise if it is possible.
			///@return Always true. Pleas note that the return value is left only because of compatibility
			///with base class Engine::rotateZCCW() version. In derived class version this boolean return
			///value is useless.
			///@sa Engine::rotateZCCW() for more details.
			bool rotateZCCW();
			///Returns current block position.
			///@return Floating point position of a current block. It is the position of a middle
			///point in a block
			///@sa blockAngles()
			const Point<float, 3> blockPos() const;
			///Returns current block angles.
			///@return X, Y and Z floating point angle shifts of a current block.
			///@sa blockPos()
			const Point<float, 3> &blockAngles() const {return angleShift;}
			///Returns floating-point position of a specified Z plane.
			///Z planes are moving only when they are removed (see @ref removeFilledPlanes()).
			///During this process to correcly draw the cuboid in GLEngine, environment must now
			///the exact position of all planes (though the zth plane position is always integral z).
			///@param z Number of planes which position we want to gain.
			///@return Position of Zth plane.
			double ZPlanePos(int z)	const {return z + cuboidPlanesShift[z];}
			///Returns the alpha value for the current block.
			///The alpha is counted in base of current distance() value. It is used by
			///Game::drawBlock()
			///@return Alpha value in range of <0.0, 1.0>
			///@sa blockAlpha_
			float blockAlpha();
			///Returns the alpha value of translucent removing planes.
			///When planes are removed from cuboid, their alpha value changes from completly opaque
			///to completly transparent. You can check this alpha from this function.
			///@return Alpha value in range of <0.0, 1.0>
			///@sa planesAlpha_
			float planesAlpha() const;
			///Returns how much time left to next speed level change.
			///@return The time scaled lineary to the range <0.0; 1.0>
			float speedChangeTime() const;
			///Returns how much time left to next automatic block forward move.
			///@return The time scaled lineary to the range <0.0; 1.0>
			float moveForwardTime() const;
			///Returns current game speed level.
			///@return Speed of a game in range <0; 9>
			int speed() const	{return speed_;}
			///Returns whole game time.
			///@return How much time did the current time is running in 1/10 second (100 ms)
			int gameTime() const	{return gameTimer / 100;}
			///Returns the distance between the current block and cuboid.
			///This value is represented on a progress bar in side bar, so it must be available public.
			///@sa Engine::distance()
			int distance()	{return Engine::distance();}
			///Pauses the engine object.
			///Pauses or resumes all the timers collected in this object. This causes freezing or resuming
			///all blocks animations.
			///@param pauseState True if you want to pause the engine, false to resume
			virtual void pause(bool pauseState);
		};		//class EngineExt: public Engine

//----------------------------------------------------------------------------

	///OpenGL main game engine wrapper class.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///This class does all the job connected with drawing the main game panel onto the screen.
	class GLEngine: public EngineExt
		{
		private:

			///Cuboid walls drawing object.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///This object is suitable for creating and drawing all the cuboid walls efects.
			///By now walls look really uncool, because I couldn't find any simple and nice-looking
			///3D effect for them. I'll try to write some more effective graphics in the future.
			class Walls
				{
				private:
					///How fast does the points color change.
					///The color of wall points changes with this speed.
					static const float COLOR_CHANGE_SPEED;
					///Size of a cuboid.
					///It is width and height of a game cuboid, delivered by EngineExt object.
					///@sa DEPTH
					const int SIZE;
					///Depth of a cuboid.
					///It is depth of a game cuboid, delivered by EngineExt object.
					///@sa SIZE
					const int DEPTH;
					///Border of a single cube in a cuboid.
					///This is the length of a border of a single cube (distance between two points).
					const float BORDER;
					///Current points color.
					float color;
					///Used to animate sine color waving
					float phi;
					///Walls timer.
					///Used to calculate the next color.
					///@sa update()
					MyOGL::Timer timer;
					///Updates all walls data (currently only color).
					///@sa timer.
					void update();
				public:
					///Constructor.
					///Saves cuboid dimensions.
					Walls(int size, int depth, double border);
					///Draws the walls.
					///All you have to do from parent class is to call this method. Everything rest is done
					///automatically (including call to update()).
					///@sa update()
					void draw();
				};

			///Manages preview of a next block.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///Although next block preview is a part of the sidebar, I've decided to keep it in
			///independent class.
			class NextBlockPreview
				{
				private:
					///How fast are the translucent cubes changing their color.
					///@sa alphaShift for description.
					static const float BLEND_SPEED;
					///Length of a single beat-function period.
					///@sa beat to see what is beat-function.
					static const float BEAT_PERIOD;
					///Time between two beat-function peaks.
					///@sa beat to see what is beat-function.
					static const float BEAT_PEAK_INTERVAL;
					///Minimal value of a beat function.
					///This is the minimal border size of a single cube in a preview.
					///@sa beat
					///@sa draw()
					static const float BEAT_MIN;
					///Minimal value of a beat function.
					///This is the maximum border size of a single cube in a preview (in the peak).
					///@sa beat
					///@sa draw()
					static const float BEAT_MAX;
					///Parent GLEngine object.
					///This object is needed for cubec drawing routines.
					GLEngine &parent;
					///Next block preview angle.
					///Next block is previewed in small square box on a sidebar. To make this preview nice
					///looking, the block is rotating. This variable stores its current angle.
					///@sa draw()
					///@sa color
					float angle;
					///Next block preview color.
					///Color of a next block in a preview pane. Should be an angle in range <0; 2PI).
					///@sa draw()
					///@sa angle
					float color;
					///Translucent cubes alpha value.
					///When some cubes are removed because they were part of previous block but they're
					///not part of current (or cubes are part of block now, but they weren't), they are
					///drawn with this alpha value.
					///@sa draw()
					float alphaShift;
					///Object timer.
					///Needed to properly update alpha and angle values.
					MyOGL::Timer timer;
					///Updates preview data.
					///Mostly changes the angle to make preview rotating and changes alpha value a bit.
					void update();
					///Prevoius block data.
					///Although it might look this object is redundant because it stores the same data
					///as current block, it is not true. To be precise - it is until the user rotates current
					///block - if he does while alphaShift isn't 0, animation will crash.
					///I've decided to use ready-made Block object, even if most of its method are not needed
					///(actually only cubes data is).
					Block previousBlock;
					///Beat-function's argument.
					///@par Beat-function idea
					///I've got an idea to change next block preview's size so that it looks like a beating heart.
					///This is done using two added Gaussian distributes with different phases. The effect is
					///pretty realistic.
					///@par
					///Variable beat holds an argument of this function in range <0; BEAT_PERIOD>
					///@sa BEAT_PERIOD
					///@sa update()
					///@sa draw()
					float beat;
				public:
					///Constructor.
					NextBlockPreview(GLEngine &iParent);
					///Updates preview data and shows next block.
					///@note It does not change any viewport settings, this method only draws the next block.
					///All the GL perspective and viewport jobs has to be done earilier.
					void draw();
					///Next block was changed.
					///This method should be done every time the blocks are switched 'cause it must
					///perform some additional tasks after that.
					void switchBlocks();
				};

			///Object controlling walls of game cuboid.
			///To see what features has this object, check its detailed description.
			Walls walls;
			///Length of a border in a single cube.
			///This length depends on how big the game size is: the bigger size, the smaller cubes.
			const double border;
			///Pointer to a cube display list.
			///OpenGL style pointer to a display list containing textured cube. Used to draw blocks,
			///cuboid, etc.
			///@sa drawBlock()
			///@sa drawCuboid()
			const GLuint cubeDisplayList;
			///Current block alpha value.
			///@sa EngineExt::blockAlpha
			float alpha;
			///Draws the cubes which were already saved on the cuboid.
			///This method draws only the cubes which are saved in game engine (not those, which are
			///building the current block).
			///@sa drawGrid()
			void drawCuboid() const;
			///Draws current block.
			///It calls directly a set of help methods to fully draw the current block. Before drawing all
			///parts of a current block, sets up position and angle shifts.
			///@sa drawBlock()
			///@sa drawBlockCubes()
			///@sa drawBlockGrid()
			void drawBlock();
			///Draws all the cubes which the current block is built from.
			///Every cube has its color corresponding to Z plane where it is located.
			///@sa drawBlock()
			void drawBlockCubes();
			///Draws current block grid.
			///Draws all the lines (using @ref drawBlockGridLine()) of a cuboid grid. Uses the list of them
			///created in EngineExt object and returned by its method EngineExt::grid().
			///@sa drawBlockGridLine()
			///@sa EngineExt::grid()
			void drawBlockGrid();
			///Draws one line from given Line object.
			///Puts two points in OpenGL environment, which coordinates are stored in Line object.
			///For performance reasons this function does not call glBegin() and glEnd() - you must take
			///care of it by your self (it is done in drawBlockGrid(), which internally uses this method).
			///@param line Line coordinates to be drawn
			///@sa Line
			///@sa drawBlockGrid()
			static void drawBlockGridLine(const Line &line)	{glVertex3d(line.first.x(), line.first.y(), line.first.z()); glVertex3d(line.second.x(), line.second.y(), line.second.z());}
			///Draw a cuboid of any size in any place.
			///This method is more complex then its overloaded version drawCube(int x, int y, int z)
			///because it gives the ability to put the cube on every place (not limited to integer
			///coordinates) and any size.
			///@param x X coordinate of the middle of a cube
			///@param y Y coordinate of the middle of a cube
			///@param z Z coordinate of the middle of a cube
			///@param border Size of a cube (length of its border)
			void drawCube(double x, double y, double z, double border) const;
			///Creates OpenGL display lists.
			///Creates OpenGL textured cube display list
			///@return OpenGL pointer to a display list.
			///@sa cubeDisplayList;
			GLuint buildDisplayLists();
			///Object responsible for previewing next block.
			///This object handles all the job of preview the next block.
			///@sa NextBlockPreview for more details.
			NextBlockPreview nextBlockPreview;
		protected:
			///Reference to MyOGL::Extensions.
			///GLEngine needs to have access to such an object to properly
			///texturize the cubes which it draws (on blocks and cuboid), print BlockAnalyzerMsg, etc.
			///@sa MyOGL::Extensions
			MyOGL::Extensions& extensions;
			///Some additional routines when the blocks are switched.
			///Except calling base class method calls NextBlockPreview::switchBlocks() which
			///must be called every time the blocks are switched.
			///@sa NextBlockPreview::switchBlocks()
			void switchBlocks();
			///Overriden Engine::gameOver()'s method run automatically by Engine.
			///When the game is overed, Engine object call its pure virtual function gameOver().
			///This causes to execute this method which starts game overed pause mode.
			///@sa PauseInfo class.
			void gameOver();
		public:

			///Main game panel when the game is paused or overed.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///When the game is paused or overed, this screen is shown instead of the cuboid and current block.
			class PauseInfo
				{
				private:
					///Big info message rotates around Z axis with this speed.
					///@sa angle
					///@sa update()
					static const float ROTATION_SPEED;
					///Amplitude of a info message oscillation.
					///@sa angle
					static const float MSG_AMPL;
					///String containing "PAUSE" message.
					///Put in a constant to be able to quickly replace the message in the future.
					///@sa GAME_OVER_MSG
					///@sa drawMsg()
					const std::string PAUSE_MSG;
					///String containing "GAME OVER" message.
					///Put in a constant to be able to quickly replace the message in the future.
					///@sa PAUSE_MSG
					///@sa drawMsg()
					const std::string GAME_OVER_MSG;
					///MyOGL::Window extensions.
					///PauseInfo class uses bitmap and outline fonts, so it must have access to such
					///an object.
					MyOGL::Extensions& extensions;
					///Updates info message position.
					///This BlockAnalyzerMsg oscillates around Z axis and its position must be updated
					///every frame.
					///@par
					///draw() is called automatically be draw() method.
					///@sa draw().
					void update();
					///Object timer.
					///Used by update().
					///@sa update()
					MyOGL::Timer timer;
					///Current info message angle around Z axis.
					float angle;
					///Draws info message.
					///@sa PAUSE_MSG
					void drawMsg();
					///Draws simple help in pause mode.
					///Show the information how to resume and exit the game at the bottom of the screen.
					void drawHelp();
					///Pause flag.
					///If it is set, it means that the game is currently paused, if not - it is normally running.
					///This flag is read in Game::draw() to determine what object to use to draw.
					///@sa Game::draw()
					///@sa RUNNING;
					///@sa PAUSED;
					///@sa GAME_OVER;
					int mode_;
				public:
					///Game running pause mode.
					///This mode (default) is set when the game is normally running
					///@sa mode_
					static const int RUNNING = 0;
					///Game paused by the user pause mode.
					///This mode is set when the user pauses the game using [Esc] key.
					///@sa mode_
					static const int PAUSED = 1;
					///Game over pause mode.
					///This mode is set when the game is overed (automatic pause on).
					///@sa mode_
					static const int GAME_OVER = 2;
					///Constructor.
					///@param iExtensions Reference to an MyOGL::Extensions object. See @ref extensions
					///to see what is the purpose of this object.
					///@sa extensions
					PauseInfo(MyOGL::Extensions& iExtensions);
					///Draws the pause screen.
					///First calls update(), then show the info message and simple help.
					///@sa update()
					///@sa drawMsg()
					///@sa drawHelp()
					void draw();
					///Changes the current pause mode.
					///@param pauseMode Desired new pause mode
					///@sa mode_ for modes list
					void mode(int pauseMode)	{mode_ = pauseMode;}
					///Returns current pause mode.
					///@return Current pause mode.
					///@sa mode_ for a list of pause modes.
					int mode()	{return mode_;}
				};

			///Handles all the camera positioning and animation routines.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///This class takes care about all the camera position and animation features. Especially
			///it provides interface to set up new camera position and to move to that position smoothly
			///with specified speed.
			class Camera
				{
				private:
					///Trims angle in degrees to <-180;180) range.
					///If specified angle isn't in such a range (less or greater), it is shifted 360 degrees to
					///fit range.
					///@param angle Angle to be trimmed (in degrees)
					///@sa GLEngine::Camera::update()
					template<typename T> static void trimAngle(T &angle);
					///Temporary camera position set by the user.
					///This variable is a proxy when user changes the camera position. He first changes
					///the pos_ variable and in update() method the class calculates the new shift position
					///values.
					///@sa pos() to see how to change this variable
					Point<float, 3> pos_;
					///Current camera speed.
					///Stores the information how fast the camera should be animated.
					///@sa speed()
					float speed_;
					///Camera timer.
					///This timer is used to correctly animate the camera animation when it changes the
					///position.
					///@sa update()
					MyOGL::Timer timer;
					///Max of 3 values.
					///Finds the biggest value from three given and returns it.
					///Uses standard algorithm max() which takes two arguments.
					///@param val1 First value to check
					///@param val2 Second value to check
					///@param val3 Third value to check
					///@return Biggest value from val1, val2, val3
					///@sa update()
					template<typename T> static const T &max3(const T &val1, const T &val2, const T &val3)
						{return max(max(val1, val2), val3);}
				protected:
					///Updates the camera position.
					///When the user changes the camera position using pos() function it calculates the
					///camera shift values. Then it decreases the absolute values of shift depending on
					///the time elapsed since last update() call to perform camera animation.
					///@sa pos() and pos_, solid, shift variables
					virtual void update();
					///Game cuboid depth.
					///This depth (in OpenGL units) informs what is the distance between the top and the
					///bottom of the cuboid. It is used when positioning camera.
					///@sa place()
					const float depth;
					///Solid camera position.
					///This is the camera's destination position. After a while camera should stand on this
					///position, but right after the change using pos() the shift values aren't zeroes and
					///the camera is moving smoothly in the solid position direction.<br>
					///X coordinate is the angle (in degrees) around X axis
					///Y coordinate is the angle (in degrees) around Y axis
					///Z coordinate is the zoom value (0.0 is the default zoom)
					///@sa pos()
					///@sa shift
					Point<float, 3> solid;
					///Camera shift position.
					///The camera shift position is used to animate the camera position when it is moving from
					///one solid position to another. The absolute values in shift structure are decreased
					///every time update() is called so the camera makes its way to the destination position.
					///@sa update()
					///@sa pos() and solid
					Point<float, 3> shift;
				public:
					///Receives some necessery information for camera working.
					///@param iSize Game cuboid size
					///@param iDepth Game cuboid depth
					///@param iSpeed Initial camera speed
					///@sa speed()
					Camera(int iSize, int iDepth, float iSpeed);
					///Used to change the camera position.
					///By accessing and changing the returned reference's methods you can change the
					///solid camera position. Camera class mechanisms will perform proper calculations
					///to make the change from one solid position into another smooth.
					///@return Reference to the camera solid position, which can be changed
					Point<float, 3>& pos()	{return pos_;}
					///Places the camera.
					///Puts the OpenGL axises in the lower left bottom corner of a game cuboid. It resets
					///the model view matrix, so you don't have to other about it.
					virtual void place();
					///Gives access to the camera speed.
					///@return Reference to current camera speed, allowing to read and, especially, change the
					///speed.
					float& speed()	{return speed_;}
				};

			///Constructor.
			///@param difficulty Stores information about the game difficulty, which are the game cuboid
			///size (width and height), depth and the desired blocksSet.
			///All this information is used when creating game
			///@param iExtensions Although the object doesn't need the whole Game or even
			///MyOGL::Window reference, because it draws the scene, it must have access to loaded extensions.
			GLEngine(const Difficulty& difficulty, MyOGL::Extensions& iExtensions);
			///Draws the whole main game panel.
			///Very important method - it draws the whole client window including cuboid, walls, block, etc.
			///@sa drawCuboid()
			///@sa drawBlock()
			void draw();
			///Draws next blokck preview.
			///Because the next block preview is viewed in a different viewport, this task can't be joint
			///into draw(). Between drawing main panel and next block preview the viewport must be
			///changed - and this is not the GLEngine job.
			void drawNextBlock()	{nextBlockPreview.draw();}
			///PauseInfo object used to draw nice pause information.
			///@sa PauseInfo class for more details.
			PauseInfo pauseInfo;
			///Pauses the GL engine.
			///Call base class pause() method to freeze the timers and ssets the PauseInfo mode for
			///proper pause mode displaying.
			void pause(bool pauseState);
		};		//class GLEngine: public EngineExt

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif

//----------------------------------------------------------------------------
