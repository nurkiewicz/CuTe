//----------------------------------------------------------------------------

///@file
///CuTe main game scene classes.
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
///It does not include game main engine.

//----------------------------------------------------------------------------

#ifndef GAME_H
#define GAME_H

//----------------------------------------------------------------------------

#include <complex>
#include <utility>
#include <boost/array.hpp>
#include "scene.h"
#include "glengine.h"
#include "sidebar.h"
#include "blockanalyzer.h"

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///OpenGL wrapper for CuTe game EngineExt object.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///This object takes care of all the drawings, including cuboid, blocks, menus, camera, etc.
	///It mostly uses the extended game information from EngineExt object.
	///@sa EngineExt
	///@todo Speed-up rendering by omitting overlapped cube walls
	class Game: public CuTeScene
		{
		private:

			///Camera control class.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///This class extends the abilities of the base class by giving three different camera modes
			///and 10 camera positions load/save slots, which might be used by the user.
			///@bug Camera is moving even in pause mode
			class Camera: public GLEngine::Camera
				{
				private:
					///The speed of camera normal rotation.
					///When you rotate camera, its angle changes with this speed.
					///@sa AUTO_SPEED
					///@sa set()
					static const float ROTATION_SPEED;
					///The speed of camera normal zoom.
					///When you zoom (in or out) camera, its position changes with this speed.
					///@sa AUTO_SPEED
					///@sa set()
					static const float ZOOM_SPEED;
					///Camera auto-move speed multiplier.
					///When camera is moved automatically by program (when you switch view or when it goes back
					///to permanent position), its rotation and zoom speed is multiplied by this value (so that
					///this move could be faster)
					static const float AUTO_SPEED;
					///Solid bottom camera mode.
					///In this mode camera rotates around the point located in the middle of a cuboid far
					///the user (bottom of cuboid).
					static const int MODE_SOLID_BOTTOM = 0;
					///Solid top camera mode.
					///In this mode camera rotates around the point located in the middle of a cuboid near
					///the user (top of cuboid).
					static const int MODE_SOLID_TOP = 1;
					///Interactive camera mode.
					///In this mode camera rotates around the point located in the middle of a current
					///block, so it interactively moves with the block.
					static const int MODE_BLOCK = 2;
					///Camera timer.
					///@sa update()
					MyOGL::Timer timer;
					///Camera position save slots.
					///Camera object gives the player ability to save up to 9 camera positions and
					///restore them in the future. 0 slot is reserved for the default camera position
					///and can't be changed, but the user can switch to it to set standard position.
					Point<float, 3> slots[10];
					///Current camera mode.
					///Game camera can work in several different modes. The mode actually informs where the
					///OpenGL viewport should be placed.
					///@sa MODE_SOLID_BOTTOM, MODE_SOLID_TOP, MODE_BLOCK
					int mode;
					///Camera normal view state.
					///The camera is on this state when the Ctrl and Shift keys are released and camera is not
					///being edited.
					///@sa prevState
					///@sa update()
					static const int VIEW = 0;
					///Camera temporary edit state.
					///The camera is being edited and the Ctrl key is pressed. When the Ctrl will be realsed
					///the camera will go back to its last solid position (the position change was temporary).
					///@sa prevState
					///@sa update()
					///@sa lastSolid
					static const int EDIT_TEMP = 1;
					///Camera permanent edit state.
					///The camera is on this state when the Shift key is pressed. When the Shift will be realsed
					///the camera will simply remain on its place.
					///@sa prevState
					///@sa update()
					static const int EDIT_PERM = 2;
					///Camera state in the previous update() call.
					///The Camera object saves its last state information to compare it with the current one.
					///If those states differs, some actions should be taken. For example when the camera goes
					///to the VIEW state from EDIT_TEMP, the camera position should go back to the lastSolid
					///position as the change was only temporary.
					///@sa update()
					///@sa VIEW, EDIT_TEMP, EDIT_PERM
					int prevState;
					///Parent game scene object.
					///The object must have access to two elements of the Game object: the keyboard
					///input routines and keyboard control codes. The first part is obvious, the second one
					///is used to determinate which key codes are responsible for which actions.
					///@sa place()
					const Game& parent;
					///Solid camera position just before entering the camera edit mode.
					///When the user enters the camera edit mode the current solid camera position is saved
					///here. Then when the user goes back to normal mode and if the last pressed key was Ctrl
					///(temporary edit mode) the camera is set to back to this position.
					///@sa update()
					Point<float, 3> lastSolid;
					///Length of the one cube edge in OpenGL units.
					///This value is computed by dividing the constant 4.0 by the cuboid size. It is used
					///to properly position the camera in block camera mode.
					///@sa place()
					const float border;
					///Checks for the keyboard input.
					///Reads the camera controlling keys and sets the appriopriate values. At the end calls
					///the base class version.
					///@sa win
					void update();
					///Helpping place() method.
					///Tis method is used in place() for the second and third camera modes.
					///@sa place()
					void placeBase();
				public:
					///Only computes and sets up some important values.
					///@param iParent Parent game scene, needed to properly position the camera (e.g. includes
					///game sizes)
					Camera(const Game& iParent);
					///Positions the camera.
					///The base class version of place() can only be used in the first camera mode (solid
					///bottom). In the two remaining ones the placing code is completely rewritten.
					///@sa placeBase()
					void place();
				};		//class Camera: public Camera

			///Mouse and keyboard controlling class.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///This class uses information from Window class to gain control over game engine using mouse
			///and keyboard. It can rotate and move current block by watching the mouse position
			///and keys state.
			///@par Using
			///All you have to do is to call the check() method every frame so that the class
			///could check the current mouse and keyboard events events and respond for them.
			class Input
				{
				private:
					///Mouse dead zone circle radius.
					///Game creates a special place around the cursor called "dead zone". This circle shaped
					///area is used to ignore very tiny mouse moves (probably accidental) - only when cursor
					///is moved beyond the circle, an action is performed.
					///@par
					///The bigger dead zone, the less sensitive will the mouse be. In the future maybe I'll add
					///ability to customize this item by the user.
					///@sa ZONE_MOVE_SPEED
					///@sa mouseCheck()
					static const float DEAD_ZONE_RADIUS;
					///Dead zone move speed.
					///When the user moves the cursor only a bit (see @ref DEAD_ZONE_RADIUS), it is not located
					///in the middle of a dead zone circle. So to move it back there, cursor is moved with
					///this speed in the direction of a circle's middle.
					///@note To perform an action using mouse you must move the cursor faster than this speed -
					/// otherwise cursor will be returning faster than the user trying to get him beyond the circle.
					static const float ZONE_MOVE_SPEED;
					///Parent Game scene.
					///This reference is used in numerous places - especially when reading data from keyboard and
					///mouse (access to Window object's needed) and when affecting game engine (moving, rotating,
					///etc.)
					///@sa check()
					Game &parent;
					///Mouse dead zone position.
					///I use complex numbers to simplify many 2D maths (for example checking the distance
					///between two numbers). Only remember that real() part is X coordinate and imag() is
					///Y.
					std::complex<float> deadZonePos;
					///Current mouse position.
					///@sa deadZonePos for more details about complex numbers.
					std::complex<float> pos;
					///Mouse wheel position.
					///This is the absolute position of mouse wheel in last update. If this value is
					///different than the one read from Window object, it means that the user moved the wheel
					///(the sign of the difference tells in which direction the wheel was moved).
					int wheel;
					///Rotate block after mouse was moved.
					///When mouse is moved horizontaly, the block should rotate around Y axis, when's
					///moving vertivally, should rotate around X axis.
					///@param direction Direction in which the mouse was moved (left, right, up, down).
					///See @ref angleToDirection() for more details about how this direction is computed.
					///@sa angleToDirection()
					///@sa mouseCheck()
					void rotateBlockXY(int direction);
					///Rotate block after wheel was moved.
					///When the mouse wheel is moved the block should rotate around Z axis.
					///@param newWheel New mouse wheel position - if differs from the last one, block
					///shoud be rotated around Z axis.
					///@sa angleToDirection()
					///@sa mouseCheck()
					///@sa rotateBlockXY()
					void rotateBlockZ(int newWheel);
					///Move block after mouse was moved.
					///When the right mouse button is pressed, the blocks are moving instead of rotating
					///when the mouse is moved.
					///@param direction Direction in which the mouse was moved (left, right, up, down).
					///See @ref angleToDirection() for more details about how this direction is computed.
					///@sa angleToDirection()
					///@sa mouseCheck()
					void moveBlock(int direction);
					///Converts angle in radians to integral direction values.
					///This table covers which angles are converted to what numbers and what directions are
					///they corresponding.
					/// <table><tr><td><b>Angle range</b></td><td><b>Direction</b></td><td><b>Number</b></td></tr>
					/// <tr><td> <-PI/2 ; PI/2> </td><td>right</td><td>0</td></tr>
					/// <tr><td> <5/3 PI/2 ; 7/3 PI/2> </td><td>up</td><td>1</td></tr>
					/// <tr><td> <3/2 PI/2 ; 5/3 PI> </td><td>left</td><td>2</td></tr>
					/// <tr><td> <PI/2 ; 3/2 PI> </td><td>down</td><td>3</td></tr></table>
					///@param angle Angle to convert. It must be in range <-PI ; PI) - Such a range is returned
					///by standard arg() function for complex numbers.
					static int angleToDirection(float angle);
					///Mouse position timer.
					///@sa updateCursor()
					MyOGL::Timer timer;
					///Changes mouse position externally.
					///Mouse cursor is limited to the area of a screen. But in the game cursor is hidden
					///so the player shouldn't realise that - he should be able to move the cursor as far as he
					///wants it. This illusion can be done by this method - when the cursor is on the one edge
					///of a screen, method moves it using mouse_event() WinAPI function right to the
					///opposite edge. Because the user can't see the mouse pointer, he don't realise this action.
					///But thanks to it, mouse pointer can be moved apparently as far as you can.
					///@sa mouseCheck()
					void updateCursor();
					///Check mouse position.
					///This method checks the mouse position and buttons pressed. If mouse cursor is positioned
					///beyond the dead zone, proper action is performed. Besides checking the mouse, it saves
					///many essential information for next mouseCheck() call.
					void mouseCheck();
					///Checks keyboard status.
					///Reads from Window's object information about keys pressed and performs some
					///actions if they are connected to the keys pressed. Maybe I'll add ability to customize
					///the keys, by now the action key codes are stored in static constants in a class.
					void keyboardCheck();
					///Is the mouse control enabled?
					///Mouse control should be enabled only in fullscreen mode. In windowed mode it's really
					///uncomofortable and crashes sometimes.
					///@sa mouseCheck()
					bool enableMouse;
				public:
					///Constructor.
					///No special activities - just save parent's reference and intialize some data.
					///@param iParent Parent scene for some input actions
					///@param iEnableMouse Mouse should be enabled only in fullscreen mode
					Input(Game &iParent, bool iEnableMouse);
					///Checks all the game engine input.
					///Calls the mouse and keyboard check methods. This method should be called every
					///frame to respond as fast as possible to incoming input data (and do not loose any).
					///Besides it informs whether the user wants to finish the game.
					///@return True if user wants to finish the game.
					bool check();
				};		//class Input

			///Reference to game engine.
			///Obviously OpenGL scene needs to have access to the game engine, cause this is the place
			///were all game data is stored. This is the extended version, because it was created
			///especially for this class.
			GLEngine engine_;
			///Camera object.
			///@sa Camera for more details how this object is used.
			Camera camera;
			///SideBar class object used to draw and update the side bar.
			SideBar sideBar;
			///OpenGL display list ID's.
			///Curently there is only one display list available, but this code is still under development.
			///@sa buildDisplayLists()
			GLuint cubeDisplayList;
			///Draws the main game panel.
			///Sets up main game viewport, places the camera and draws the game engine (cuboid, blocks,
			///etc.)
			///@sa refresh()
			void drawMainGame();
			///Draws side bar on the right side of the screen.
			///@sa SideBar class
			void drawSideBar();
			///Draws next block preview at the bottom of the side bar.
			///@sa NextBlockPreview class.
			void drawNextBlock();
			///Game finished flag.
			///This flag is set to true when user pressed [Enter] in paused mode. This might when happen when
			///user wants to abort the game or the game is overed (which causes pause mode run automatically).
			///@sa done()
			bool done_;
			///Mouse and keyboard controlling object.
			///@sa Game::Input class.
			Input input;
			///Block analyzer object used to help user when he is cheating.
			///This object was designed to use in demo mode but it also can be used to help user
			///in finding the best place for his current block.
			///@sa cheat() for more details.
			BlockAnalyzer cheater;
			///Help user placing his block.
			///When user presses [F1] it initiates cheater object to find the best place for the block.
			///Calling this function also causes taking half of the user points.
			///@sa cheater
			void cheat();
		public:

			///Stores the virtual key codes for all common game actions.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///This class gives ability to read and change the keyboard virtual key codes. Those are the
			///Windows VK_* codes which are assigned to most commang game actions like rotating, moving,
			///etc.
			class Controls
				{
				private:

					///Holds some essential data about one game action.
					///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
					///@date Jul 2005-Mar 2006
					struct ActionData
						{
						///Short game action name.
						///This name is used in XML files for input/output; it is suppose to be short but
						///give the basic feeling of the action, like "rotateXCW" or "cheat".
						std::string actionName;
						///Default key code assigned to the action.
						///When the user won't assign his own key or after first launching CuTe, the default
						///action key will be used.
						unsigned char defaultKey;
						};		//struct ControlData

					///Number of all customizable game action.
					///This is the number of all game actions (like "moveLeft") which keys can be customized.
					///@sa allActions()
					///@sa checkActionNum()
					static const int ALL_ACTIONS = 20;
					///Stores some essential data about all the customizable game actions.
					///Holds the data described in ActionData structure for each customizable game action.
					static const ActionData actionsData[ALL_ACTIONS];
					///Virtual key codes for all game actions.
					///This array of chars stores the Windows virtual key codes for all customizable game
					///actions. This is the core part of the class since its base purpose is to store
					///this codes.
					///@sa operator() to see how to obtain the key codes for particular actions.
					boost::array<unsigned char, ALL_ACTIONS> actions;
					///XML key containing the key codes data.
					///The class might first load the key codes from this key and then save back the data
					///to the key (for examle if it was changed).
					///@sa Controls(MyXML::Key& iControlsKey) and Controls(const Controls& src)
					///@sa ~Controls()
					MyXML::Key* controlsKey;
					///Checks whether the given action number is valid.
					///The action number must be non-negative and less than ALL_ACTIONS constant. If it isn't
					///exception is thrown.
					///@throws CuTeEx
					///@sa ALL_ACTIONS
					static void checkActionNum(int action);
				public:
					///Rotate XCW action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int ROTATE_XCW = 0;
					///Rotate XCCW action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int ROTATE_XCCW = 1;
					///Rotate YCW action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int ROTATE_YCW = 2;
					///Rotate YCCW action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int ROTATE_YCCW = 3;
					///Rotate ZCW action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int ROTATE_ZCW = 4;
					///Rotate ZCCW action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int ROTATE_ZCCW = 5;
					///Move left action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int MOVE_LEFT = 6;
					///Move right action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int MOVE_RIGHT = 7;
					///Move up action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int MOVE_UP = 8;
					///Move down action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int MOVE_DOWN = 9;
					///Move forward action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int MOVE_FORWARD = 10;
					///Camera set temporary action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int CAMERA_SET_TEMPORARY = 11;
					///Camera set permanent action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int CAMERA_SET_PERMANENT = 12;
					///Camera zoom in action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int CAMERA_ZOOM_IN = 13;
					///Camera zoom out action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int CAMERA_ZOOM_OUT = 14;
					///Camera default position action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int CAMERA_DEFAULT_POS = 15;
					///Camera mode bottom action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int CAMERA_MODE_0 = 16;
					///Camera mode top action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int CAMERA_MODE_1 = 17;
					///Camera mode block action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int CAMERA_MODE_2 = 18;
					///Cheat action code.
					///@sa operator()()
					///@sa actionStrs()
					static const int CHEAT = 19;
					///Loads the keyboard control codes from the given XML key.
					///This constructor loads the key virtual codes from the given XML key. Use this constructor
					///when you want to automatically save back the key codes to the XML key in the destructor.
					///It will be very usefull for example if the key codes were changed during the game
					///was running.
					///@param iControlsKey XML key containing the key codes. It should have a number of
					///ALL_ACTIONS nested keys with numeric values of the key codes.
					///@sa ~Controls()
					///@sa controlsKey
					Controls(MyXML::Key& iControlsKey);
					///Copy constructor.
					///This constructor copies the key codes from the source class. It differs from the
					///basic version of the constructor that it doesn't save the key codes to the XML
					///key - in fact, it doesn't even have any available, since it is not provided as
					///a parameter.
					///@param src Source Controls class object, which will be used in copy-construction.
					Controls(const Controls& src);
					///Saves back the key codes to the provided XML key.
					///Saves only when the key was actually provided, i.e. when the
					///Controls(MyXML::Key&) constructor was used.
					///@sa Controls(MyXML::Key&)
					~Controls();
					///Returns the number of all customizable actions in the game.
					///Use this number when you want to iterate through all the available action: their numbers
					///are from o to allActions() - 1.
					///@return ALL_ACTIONS constant;
					///@sa ALL_ACTIONS
					static int allActions()	{return ALL_ACTIONS;}
					///Returns the text representation of the virtual key code.
					///This function is used in some output routines in the game is not strongly connected
					///with the Controls class.
					///@param keyCode Windows virtual key code to be converted.
					///@return String representing the key code, e.g.: VK_SHIFT -> "[Shift]", VK_F7 -> "[F7]".
					static const std::string keyCodeToStr(unsigned char keyCode);
					///Gives access to the key code assigned to the particular action.
					///The overloaded operator() allows to treat the Controls object as a function returning
					///the key code.
					///@param action Action code, should be one of the constants defined in this class.
					///@return Virtual key code assigned to the provided action. Because it is a reference,
					///this method allows to change the code as well.
					///@throws CuTeEx when the action parameter is not valid.
					unsigned char& operator()(int action);
					///Gives access to the key code assigned to the particular action.
					///@param action Action code, should be one of the constants defined in this class.
					///@return Virtual key code assigned to the provided action.
					///@throws CuTeEx when the action parameter is not valid.
					///@sa non-const operator()()
					unsigned char operator()(int action) const;
					///Returns two strings describing the particular action.
					///@param action Action code, should be one of the constants defined in this class.
					///@return Pair of string; the first one is the short description which can be used as
					///a menu item label; the second one is the longer description, full explaining the
					///action behaviour.
					///@throws CuTeEx when the action parameter is not valid.
					static std::pair<const std::string, const std::string> actionStrs(int action);
					///To which action is the specified key assigned?
					///This method can be used to determinate whether the specified key isn't already
					///assigned to some action (and to which one).
					///@param keyCode Virtual key code to be checked.
					///@return -1 if the specified key isn't assigned to any customizable action. Otherwise
					///returns this action code.
					///@throws CuTeEx when the action parameter is not valid.
					int controlAction(unsigned char keyCode);
					///Overloaded assignment operator.
					///Pleas note that this operator copies only the key codes. It does not change the
					///controlsKey pointer value.
					///@sa controlsKey.
					Controls& operator=(const Controls& src)	{actions = src.actions; return *this;}
				};		//class Controls

			///Constructor.
			///@param parentWindow Parent MyOGL::Window object, which is passed further to the base
			///object's constructor.
			///@param iDifficulty Game difficulty to use. This is used to gain information about
			///cuboid sizes and desired blocks set. This referenced is passed further to Engine object.
			///@param iControls Keyboard controls which will be used during the game
			Game(CuTeWindow &parentWindow, const Difficulty& iDifficulty, const Controls& iControls);
			///Sets the viewport back after new game has been done.
			///Game scene changes the viewport splitting it into many independent parts. But the menu
			///uses only one viewport. This method sets the viewport back after the game finished.
			~Game()	{win.viewport();}
			///Main scene method.
			///All OpenGL code goes here. In this method the OpenGL scene is drawn onto the buffer and
			///the screen is refreshed.
			void refresh();
			///Returns game engine associated with object.
			///Some environment routines must call EngineExt methods explicitly.
			///@return Reference to an EngineExt object.
			///@sa engine() const
			///@sa EngineExt
			///@sa engine_
			GLEngine& engine() {return engine_;}
			///Returns const game engine associated with object.
			///Use this routine when you have access only to const Game object.
			///@return Const reference to an EngineExt object.
			///@sa engine()
			///@sa engine_
			const GLEngine& engine() const	{return engine_;}
			///Keyboard input controls codes object.
			///This object stores the virtual key codes for majority of game actions (moving, rotating, etc.)
			///I've decided to leave at as public insted of giving access to it via public method
			///because the object is const itself so the external envirnoment can't damage the class
			///information any how and it is simplier to read the codes.
			const Controls& controls;
		};		//class Game: public CuTeScene

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif		//#define GAME_H

//----------------------------------------------------------------------------
