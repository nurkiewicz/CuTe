//----------------------------------------------------------------------------

///@file
///Game intro animation
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
///Declaration of CuTe game intro - a simple animation shown right after
///launching the game.

//----------------------------------------------------------------------------

#ifndef INTRO_H
#define INTRO_H

//----------------------------------------------------------------------------

#include <vector>
#include "scene.h"
#include "point.h"
#include "MyXML/myxml.h"

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///Intro animation OpenGL scene.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///This class handles all the job performed during the game intro is displayed. It loads
	///the needed data and renders the whole animation.
	class Intro: public CuTeScene
		{
		public:
			///Sets up some essential variables.
			///The constructor creates the first animation object, BumpingCube.
			Intro(CuTeWindow& win);
			///Draws the screen and refreshes the display.
			///This method, overriden from CuTeScene draws the animation onto the screen.
			///Because the animation is divided into several parts (implemented as classes
			///derived from Animation class), it also checks whether the current part of the
			///animation didn't finished and if it did, loads another part.
			///@sa BumpingCube, LogoBlending, LogoShaking, LogoExploding
			void refresh();
		private:

			///Base class for animation classes.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///Intro animation is divided intro several parts, each of which is implemented
			///as an independent class derived from this class.<br>
			///The class gives some common interface, like draw() and done() methods.
			class Animation
				{
				public:
					///Only sets up some variables in class.
					///@param iScene Animation parent scene. It is needed to gain access to some
					///important features like OpenGL extensions, window, etc.
					///@param animationLength Default animation length (in ms).
					///See done() for more details.
					Animation(const CuTeScene& iScene, int animationLength = 0):
						scene(iScene), ANIMATION_LENGTH(animationLength)	{}
					///Pure virtual destructor, needed for base class.
					virtual ~Animation() = 0;
					///Whether the part of animation has finished.
					///The default behaviour is that the animation finishes when the time
					///it has been performed reaches the time given in the constructor. However you
					///may override the method to introduce different behaviour.
					///@return True if the animation signalizes it has finshed.
					virtual bool done() const	{return timer >= ANIMATION_LENGTH;}
					///Draws the animation.
					///Pure virtual function, of course it must be implemented in derived classes
					///to perform the animation rendering.
					virtual void draw() = 0;
				protected:
					///Main animation timer.
					///This timer is not needed in this class, but is very commonly used in
					///derived class, because they're heavily dependant on time.
					MyOGL::Timer timer;
					///Parent animation scene.
					///Use this constant to get access to OpenGL extensions, window etc.
					const CuTeScene& scene;
					///Default animation length.
					///@sa done() for more details.
					const int ANIMATION_LENGTH;
				};		//class Animation

			///Bumping cube animation class.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///This animations shows a single cube bumping from any start position to an
			///end position
			class BumpingCube: public Animation
				{
				public:
					///Sets up cube start position, speed, etc.
					///@param startPos Starting cube position
					///@param endPos Final cube position
					///@param scene Parent CuTeScene object
					///@warning The endPos shoud lay beneath the startPos and the startPos (x, y)
					///should differ to endPos (x, y) coordinates
					BumpingCube(const Point<double, 3>& startPos, const Point<double, 3>& endPos, const CuTeScene& scene);
					///Draws the bumping cube.
					///First refreshes the cube position and speed and then, according to the
					///current time, shows the cube at the proper position.
					///@sa pos, speed.
					void draw();
				private:
					///Current cube position.
					///This value corrdesponds to the current cube position, it is updated
					///every time draw() is called according to the speed variable.
					///@sa speed
					Point<double, 3> pos;
					///Current cube speed.
					///speed.x() and speed.z() are constant at all time, but the speed.y() changes
					///to make an impression of gravitaion caused bumping.
					///@sa GRAVITY
					///@sa pos
					Point<double, 3> speed;
					///Axises around which the cube is spinning.
					///To make the cube movement more realistic, cube rotates around this axises
					///making a really nice impression of bumping.
					const Point<float, 2> rotAxis;
					///Animation length.
					///How much time (in ms) should it take the cube to move from startPos to endPos.
					static const int BUMPING_TIME = 8000;
					///Cube gravity (g) value.
					///This value determinates the amount of acceleration the cube experience.
					static const double GRAVITY;
					///Y coordinate of the endPos.
					///When the cube reaches this height, it should bump back and start to move
					///up with negative acceleration.
					const double BOTTOM;
					///Timer value saved last time the draw() was called.
					///(timer - lastTime) is the amount of time (in ms) which have elapsed since
					///the animation was last updated. It is very helpful in cube position
					///calculations.
					int lastTime;
				};		//class BumpingCube: public Animation

			///Single cube information in CuTe intro logo.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///Intro shows the CuTe game logo created using many cubes. This structure stores
			///the information about one cube (position and time the cube should be shown on
			///the screen.
			struct Cube
				{
				///Loads the cube information from XML key.
				///@param data XML key containing the information such as the cube position
				///and initial time. It only stores the (x, y) coordinates, the Z coordinate is
				///randomized.
				Cube(const MyXML::Key& data);
				///Equality operator used for sorting.
				///After all cubes are loaded from XML file, they are sorted according they
				///initial time.
				///@param left Left operator argument
				///@param right Right operator argument
				///@sa operator<()
				friend bool operator==(const Cube& left, const Cube& right)
					{return left.initTime == right.initTime;}
				///Less-than operator used for sorting.
				///@param left Left operator argument
				///@param right Right operator argument
				///@sa operator==()
				friend bool operator<(const Cube& left, const Cube& right)
					{return left.initTime < right.initTime;}
				///How fast should the alpha value change for the cube.
				///@sa initTime for more details
				static const double ALPHA_SPEED;
				///Cube position.
				///@sa Cube()
				Point<double, 3> pos;
				///Cube showing intial time.
				///When the initial time for the one particular cube is reached, it should be
				///shown on a screen. But to make a nicer impression, it is shown step by step
				///using alpha-blending with ALPHA_SPEED speed.
				int initTime;
				};	//struct Cube

			///Vector containing all the cubes in logo.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///The class works just like std::vector except it has a special constructor.
			///It loads the cubes from intro.xml file automatically.<br>
			///This class (created only once for all animations) is given to the animation
			///classes (derived from Logo class) in the contructor.
			class LogoCubes: public std::vector<Cube>
				{
				public:
					///Open a XML file and loads the cubes data.
					///Constructor open data/intro.xml file and pushes back all the cubes found
					///in that file.
					LogoCubes();
				};		//class LogoCubes: public std::vector<Cube>

			///Base class for animations related to CuTe logo.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///CuTe logo is a bunch of cubes grouped in a shape of a words "Cubic Tetris".
			///Several intro animations use this logo (blending, shaking and exploding).
			class Logo: public Animation
				{
				public:
					///Saves back reference to LogoCubes object.
					///@param scene Parent scene (passed to the base class)
					///@param animationTime Animation length (passed to the base class)
					///@param iCubes vector containing the data of all cubes
					///@sa cubes
					Logo(const CuTeScene& scene, int animationTime, LogoCubes& iCubes):
							Animation(scene, animationTime), cubes(iCubes)	{}
				protected:
					///Gives access to the data related to all logo cubes.
					///Use this protected variable to gain access to cube data when rendering
					///the game logo; you may even change the data stored here (but be careful with
					///that) e.g. in exploding animation.
					LogoCubes& cubes;
				};		//class Logo: public Animation

			///Animation for CuTe logo smoothly showing up.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///This class performs animation for smoothly showing the CuTe logo using
			///alpha blending.
			class LogoBlending: public Logo
				{
				public:
					///Constructor.
					///@param scene Parent CuTeScene object
					///@param cubes Logo cubes data object (passed to the constructor)
					LogoBlending(const CuTeScene& scene, LogoCubes& cubes):
							Logo(scene, LOGO_TIME, cubes)	{}
					///Renders the alpha-blended CuTe logo.
					void draw();
				private:
					///Animation length (in ms).
					static const int LOGO_TIME = 12566;
				};		//class LogoBlending: public Logo

			///Animation for CuTe logo shaking along the Y axis.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			class LogoShaking: public Logo
				{
				public:
					///Constructor.
					///@param scene Parent CuTeScene object
					///@param cubes Logo cubes data object (passed to the constructor)
					LogoShaking(const CuTeScene& scene, LogoCubes& cubes):
							Logo(scene, SHAKING_TIME, cubes)	{}
					///Renders the shaking logo.
					///The more time the animation is performed, the shake amplitude grows (but
					///the frequency is still the same).
					void draw();
				private:
					///Animation length (in ms).
					static const int SHAKING_TIME = 6283;
					///Shaking frequency (in Hz).
					///@sa draw()
					static const double SHAKING_FREQ;
				};

			///Animation for CuTe logo exploding.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			class LogoExploding: public Logo
				{
				public:
					///Constructor.
					///@param scene Parent CuTeScene object
					///@param cubes Logo cubes data object (passed to the constructor)
					LogoExploding(const CuTeScene& scene, LogoCubes& cubes):
							Logo(scene, EXPLODING_TIME, cubes), lastTimer(0)	{}
					///Renders the exploding logo.
					void draw();
				private:
					///Animation length (in ms).
					static const int EXPLODING_TIME = 3000;
					///Speed of explosion.
					///The speed of each cube satisfies the exponential function, this speed
					///is just a coefficient with some relative unit.
					static const double EXPLODING_SPEED;
					///Speed of the whole logo rotation (in degrees per second).
					///When the logo is exploding, the whole logo is rotated around some axis
					///at the same time to bolden the effect.
					static const double ROTATION_SPEED;
					///Timer value saved last time the draw() was called.
					///(timer - lastTime) is the amount of time (in ms) which have elapsed since
					///the animation was last updated. It is very helpful in cube position
					///calculations.
					int lastTimer;
				};

			///Whether any key was pressed during the intro animation.
			///If the user presses any key during the intro, the animation should stop
			///immediately and the menu should apper.
			///@return True if any key was pressed, otherwised false
			bool keyPressed();
			///Pointer to the current animation.
			///This pointer (thanks to polymorphysm) can point to any type of animation object
			///derived from Animation class. Then the refresh() method calls
			///animation->draw() and the virtual calls mechanism runs the proper draw()
			///method. When one method is done (animation->done()), refresh() method destroys
			///one animation and creates another one.
			///@sa refresh()
			Animation* animation;
			///All the cubes data collected in vector.
			LogoCubes cubes;
			///Current phase animation.
			///Each animation during the intro has its own uniqe number. When one animation is
			///done, refresh() reads this variable and decides, which is the next animation.<br>
			///Same behaviour could be obtained using dynamic casting the animation pointer
			///but this would be rather awkward.
			///@sa refresh()
			///@sa PHASE_BUMPING, PHASE_ALPHA, PHASE_SHAKING, PHASE_EXPLODING
			int phase;
			///Show the waiting loading screen.
			///When this variable is set (when users presses key or when whole intro is done)
			///the next time refresh() will be called, the waiting logo would appear and
			///the whole scene will finish.
			bool loadingScreen;
			///Single cube bumping animation is in progress.
			///@sa BumpingCube class
			static const int PHASE_BUMPING = 0;
			///CuTe logo is showing now using alpha-blending.
			///@sa LogoBlending class
			static const int PHASE_ALPHA = 1;
			///CuTe logo animation is in progress.
			///@sa LogoShaking class
			static const int PHASE_SHAKING = 2;
			///CuTe logo exploding animation is in progress.
			///@sa LogoExploding class
			static const int PHASE_EXPLODING = 3;
		};		//class Intro: public CuTeScene

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif		//#define INTRO_H

//----------------------------------------------------------------------------
