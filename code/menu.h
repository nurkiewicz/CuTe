//----------------------------------------------------------------------------

///@file
///CuTe menu classes.
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
///This file contains declarations of CuTe game menu classes. One class stands for one
///menu item and the second one collects all the items in to one menu.

//----------------------------------------------------------------------------

#ifndef MENU_H
#define MENU_H

//----------------------------------------------------------------------------

#include <algorithm>
#include <vector>
#include "scene.h"
#include "common.h"
#include "atom.h"
#include "difficulty.h"

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	class MenuItem;

	///OpenGL menu for CuTe.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	class Menu
		{
		private:
			///Time between switching two tiled items.
			///When you change the current mennu position, this amount of time (in ms)
			///will elapse till next current item will be set.
			///@sa next()
			///@sa previous()
			///@sa update()
			static const int ITEMS_PERIOD = 300;
			///Color value of an item which isn't current.
			///Value in HSV model.
			///@sa itemColorValue()
			///@sa COLOR_VALUE_MAX
			static const float COLOR_VALUE_MIN;
			///Color value of an item which is current.
			///Value in HSV model.
			///@sa itemColorValue()
			///@sa COLOR_VALUE_MIN
			static const float COLOR_VALUE_MAX;
			///Text items in menu.
			///Vector containing pointers to MenuItem objects. Each object corresponds to one item in menu.
			///@sa addItem()
			///@sa current_
			///@sa 
			std::vector<MenuItem*> items;
			///Current menu item.
			///This variable stores the number of the current menu position.
			///@sa current()
			int current_;
			///Previous menu item.
			///This variable stores the number of the menu position which was current just before the
			///current one.<br>
			///It is used in itemColorValue() because the color of item which was current a moment ago
			///is different than the color of any other non-current items.
			///@sa itemColorValue()
			int previous_;
			///Angle between two items in a circle.
			///The menu is drawn using the circle shape. Two adjacent items are creating this angle;
			///it is calculated by simply dividing 360 degrees by the count of all items in menu.
			///@note This variable has capital letters name to suggest that it is constant - but actually
			///it isn't. This happens because the value depends on the number of items on menu class
			///but the items can be added any time.
			///@sa drawVertically()
			///@sa drawHorizontally()
			float ITEMS_ANGLE;
			///Menu circle angle shift.
			///This variable is used to create an illusion of smooth rotating menu - when user changes the
			///current items, thanks to this variable the menu doesn't move rapidly.
			///@sa previous()
			///@sa next()
			///@sa update()
			float angleShift;
			///Object timer.
			///Is used in update() method when smoothly rotating the menu circle.
			///@sa update()
			MyOGL::Timer timer;
			///Updates the menu circle position.
			///This method changes the @ref angleShift value depending on timer's reading. Thanks to that
			///it creates a nice illusion of rotating menu.
			///@sa angleShift
			void update();
			///Returns a color value of a specified menu item.
			///When the situation is stabilized (much time elapsed since last changing the current item)
			///the job is simple - current item's color value is set to COLOR_VALUE_MAX and others to
			///COLOR_VALUE_MIN. But when the current item is changed, we don't want the color value to
			///change rapidly. This is the function's job - it checks the specified item state (current,
			///previous, none of above) and depending on @ref angleShift it calculates the color value:
			///something between <COLOR_VALUE_MIN; COLOR_VALUE_MAX>.
			///@param pos Position in the menu to calculate the color value.
			///@return Color value in range <COLOR_VALUE_MIN; COLOR_VALUE_MAX>
			///@sa angleShift
			///@sa current_
			///@sa previous_
			float itemColorValue(int pos);
			///Draws menu in horizontal orientation.
			///This type of drawing is reserved to sub menus.
			///@param scale Gives ability to scale the sub menu. Should be in range <0.0; 1.0>
			///@sa drawVertically()
			void drawHorizontally(float scale);
		public:
			friend class MenuItem;

			///Constructor.
			Menu();
			///Destructor.
			///It deletes (using operator delete()) all the items in the menu.
			///@sa addItem()
			virtual ~Menu()	{for_each(items.begin(), items.end(), deleter<MenuItem>);}
			///Draws menu vertically.
			///This method is reserved for drawing the main menu. It takes care about drawing the
			///the sub menus.
			///@sa drawHorizontally()
			void drawVertically();
			///Tries to switch to the next menu item.
			///@sa previous()
			void next();
			///Tries to switch to the previous menu item.
			///@sa next()
			void previous();
			///Returns current menu position index.
			///@return Index of the current menu position, starting from 0.
			///@sa current()
			///@sa currentIndex(int newCurrent)
			int currentIndex() const	{return current_;}
			///Sets new current menu item index.
			///@param newCurrent New current menu item index. Function does not check ranges
			///so take care about its proper value.
			///@note
			///This method only switches the current menu item index, without setting up any shift
			///variables. That's why you should always run this method only for sub menus which aren't
			///visible at the moment.
			///@sa currentIndex()
			void currentIndex(int newCurrent)	{current_ = newCurrent;}
			///Returns current menu position pointer.
			///@return Reference to the current menu position MenuItem class.
			///@sa currentIndex()
			MenuItem& current() const	{return *items[current_];}
			///Gives access to any item in the menu.
			///@param index Index of an item (starting from 0) in menu. Should be in range <0; N-1>, where
			///N is the number of item in the menu. But the function does not check the range so you
			///must take care of it by your self.
			///@return Reference to wanted menu item.
			MenuItem& operator[](int index)	{return *items[index];}
			///Add new item to the menu.
			///@param item Pointer to a <b>dynamically created</b> (using operator new) MenuItem object.
			///@return Pointer to a newly added item (same as item parameter)
			///@warning
			///item pointer must point to a dynamically created MenuItem object because all pointers
			///passed by this method are released using operator delete in the destructor.
			///@sa ~Menu()
			MenuItem* addItem(MenuItem* item);
		};		//class Menu

//----------------------------------------------------------------------------

	///One text item in Menu class.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///This is one independent item in Menu class's menu.
	///@sa Menu class
	class MenuItem
		{
		private:
			///Text which will be displayed with this item.
			///@sa draw()
			///@sa MenuItem()
			const std::string label;
			///Length of a label width in specified font.
			///It is used to center the text while displaying - current position is moved left of about
			///half of this width, so the @ref label text is centered.
			///@sa label
			///@sa fonts
			///@sa draw()
			const float labelWidth;
			///Items chaotic movement speed.
			///Every item moves independently along X, Y and Z axis using sine function - it creates
			///nice "chaotic" movement expression. This variable stores the randomly choosen speeds of
			///this move.
			///@sa chaosTimer
			Point<float, 3> shiftSpeed;
			///Timer used for chaotic move calculations.
			///This timer is never reseted - it is used to calculate the position shift with sine function.
			///@sa shiftSpeed
			MyOGL::Timer chaosTimer;
			///Pointer to a sub menu connected to the menu item.
			///Some menu items has their own sub menus, which are drawn only when the menu item is current.
			///This sub menus are saved at this pointer. If the item hasn't got any sub menu, this
			///pointer is NULL.
			///@sa addSubItem()
			///@sa subMenu()
			///@sa hasSubMenu()
			Menu* subMenu_;
			///Sub menu scale change speed.
			///Tells how fast does the sub menu scale changes.
			///@sa scale for more details how does it work.
			static const float SCALE_CHANGE_SPEED;
			///Sub menu scale timer.
			///Needed to animate the sub menu scale.
			///@sa scale for more details how does it work.
			MyOGL::Timer scaleTimer;
			///Sub menu scale shift.
			///When the sub menu is just being shown, we don't want to show it rapidly, but instead its
			///size increases smoothly. And vice versa - when the sub menu is being hidden, its size
			///decreases smoothly.<br>
			///This variable controls this size (scale).
			///@sa update() which changes the scale value smoothly
			float scale;
		protected:
			///Font which will be used to display the item's @ref label in main menu.
			///Because it's protected, it can be used to print additional information by derived classes.
			///@sa draw()
			///@sa MenuItem()
			MyOGL::OutlineFonts& outlineFonts;
			///Updates the @ref scale variable to smoothly animate sub menu.
			///@param isCurrent True if the item is current at the moment.<br>
			///by checking this parameter (and the value of wasCurrent which is set here too) function
			///may take right action: start zooming in or out the sub menu.
			///@sa wasCurrent
			virtual void update(bool isCurrent);
			///Indicates whether the item was current in the last update() call.
			///This variable is used in update() to determinate which actions to take.
			///@sa update()
			bool wasCurrent;
		public:
			///Constructor.
			///@param iOutlineFonts Outline fonts to use when displaying item's label.
			///@param iLabel Item's label.
			///@sa fonts
			///@sa label
			MenuItem(MyOGL::OutlineFonts& iOutlineFonts, const std::string& iLabel);
			///Destructor.
			///Deletes sub menu if it was created.
			virtual ~MenuItem()	{if(subMenu_ != NULL) delete subMenu_;}
			///Draws the item.
			///Besides just drawing the @ref label using @ref outlineFonts, it might draw sub menu
			///depending on isCurrent parameter, wasCurrent, scale, etc.
			///@param isCurrent Indicates whether the currently drawn item is current in parent menu.
			///@sa label
			///@sa subMenu_
			virtual void draw(bool isCurrent);
			///Add item into sub menu.
			///If the sub menu wasn't created yet, the function takes care about that.
			///@param item MenuItem object to be added.
			///@sa subMenu()
			void addSubItem(MenuItem* item);
			///Return a Sub Menu class.
			///@return Pointer to an item's sub menu object, or NULL if the item hasn't got sub item.
			///@sa hasSubMenu()
			Menu* subMenu() const	{return subMenu_;}
			///Checks whether the item has sub menu.
			///@return True if there is a sub menu connected to an item.
			///@sa subMenu()
			bool hasSubMenu() const	{return subMenu_ != NULL;}
		};		//class MenuItem

//----------------------------------------------------------------------------

	///Base class for menu items which use bitmap fonts (high scores, infos, etc.)
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///The class also introduces simple cursor blinking routines if you want to display blinking
	///cursor in your item.
	class MenuTextItem: public MenuItem
		{
		private:
			///Cursor blinking rate.
			///After this time (in ms) the cursor would become invisible (if it was visible) or
			///vice versa.
			///@sa cursorTimer
			///@sa cursorVisible_
			static const int CURSOR_BLINK_RATE = 200;
			///Cursor timer.
			///If the time on this timer exceeds CURSOR_BLINK_RATE, the cursor state will be
			///reversed.
			///@sa cursorVisible_
			MyOGL::Timer cursorTimer;
			///Cursor state.
			///If this is true it means that the cursor is visible, otherwise false. The value of this
			///variable can be read using cursorVisible() method.
			///@sa cursorVisible()
			bool cursorVisible_;
		protected:
			///Updates cursor state.
			///Check the cursorTimer time and changes cursor state if necessery (see cursorTime).
			///@sa cursorVisible_
			void update(bool isCurrent);
			///Returns current cursor state.
			///@return True if cursor should be visible, otherwise false.
			///@sa cursorVisible_
			bool cursorVisible() const	{return cursorVisible_;}
			///MyOGL::BitmapFonts object reference to bitmap fonts.
			///This element is protected so you can use it to print out the text in bitmap fonts
			///in any derived class.
			///@sa MenuItem::outlineFonts
			MyOGL::BitmapFonts& bitmapFonts;
		public:
			///Constructor.
			///@param extensions Used to extract the reference to both bitmap and outline fonts
			///objects. OutlineFonts object is passed to the base class MenuItem and BitmapFonts
			///reference is saved in @ref bitmapFonts variable.
			///@param label Menu item text to be displayed in the menu - it is passed to base
			///class constructor.
			MenuTextItem(MyOGL::Extensions& extensions, const std::string& label);
		};		//class MenuTextItem: public MenuItem

//----------------------------------------------------------------------------

	///Base class for all game menus.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///Inherit from this class when you want to make some menu in the game. It is used both to create
	///the main menu (MainMenu class) and the options menu (Options class). This class has all the
	///logic and routines needed to fully draw and handle the game menus.
	class MenuScene: public CuTeScene
		{
		private:

			///Game menu scene background.
			///This class handles the background animation of texture, making a nice impression of
			///a wind blowing through the game logo.
			///@param DETAILS Number of details to use in rendering. The bigger this value, the smoother
			///(and slower) the animation will be.
			///@param WIDTH Width of the menu logo texture. It is used when scaling the logo polygon.
			template<int DETAILS, int WIDTH>
			class Background
				{
				public:
					///Sets up some essential variables and constants in the class.
					///@param iBrightness Brightness of a game background; this shouldn't be too bright because
					///it will interrupt the user too much.
					///@param iZDist Z distance from the GL viewport to the game logo polygon position.
					Background(float iBrightness, float iZDist);
					///Draws the menu background.
					///@param textures Textures object to use when drawing. This reference can't be given in
					///the constructor because the object is created statically at the very beginning (when even
					///the Window object might not exist!)
					///@sa update()
					void draw(MyOGL::Textures& textures);
				private:
					///Number of details per one OpenGL unit.
					///This constant is usefull when drawing the background, because it simplyfies and speeds
					///up many things in draw()
					///@sa draw()
					static const float UNIT;
					///Menu background brightness.
					///This value is specified in the constructor parameters list.
					///@sa Background()
					const float brightness;
					///Timer object used in animation.
					///This timer is used when computing the Lissajous shape movement in update().
					///@sa update()
					MyOGL::Timer timer;
					///2D array containing the heights of each point in the background.
					///Animation in Background class is performed by continously changing the heights (Z pos)
					///if DETAILS + 1 x DETAILS + 1 polygon coordinates.
					///@sa update()
					boost::array<boost::array<float, DETAILS + 1>, DETAILS + 1> heights;
					///Frequency of the X axis vibration.
					///@sa omegaY
					///@sa update()
					const float omegaX;
					///Frequency of the Y axis vibration.
					///@sa omegaX
					///@sa update()
					const float omegaY;
					///Z axis distance from the viewport to the background.
					///This value is specified in the constructor parameters list.
					///@sa Background()
					const float zDist;
					///X and Y point amplitudes computed using omegaX and omegaY values.
					///@sa update() for the computations details.
					Point<float, 2> ampl;
					///Updates the bakcground positions array.
					///This method is called from the draw() method. It updates the heights array satisfying
					///the equation of the Lissajous curve. The whole background is animated using the Lissajous
					///curve equation which creates a nice impression of wave or wind animation.
					///@sa omegaX, omegaY
					void update();
				};		//class Background

		protected:
			///Menu object.
			///Collects all menu items. Use  drawVertically() to draw the vertical menu wheel on
			///the left of the screen.
			///@sa buildMenu()
			Menu menu;
			///Atom visualisation object.
			///This object controls the "atom" visualization in the upper right corner of the menu.
			///The object is drawn in menu's refresh(). It is static so the same object is used in all
			///menus.
			///@sa refresh()
			static Atom atom;
			///Player name (nick).
			///This value can be changed in Player Name menu.
			///@sa PlayerNameItem class
			static std::string playerName;
			///Background object for performing the background animation.
			static Background<8, 20> background;
			///Displays some game information on the screen.
			///Prints current game sizes, difficulty and player name od the bottom of a screen.
			///@sa playerName
			///@sa difficulty
			///@sa refresh()
			void drawInfo();
			///Current game difficulty choosen by the user.
			///Information stored in this class is used by drawInfo() method to draw the game info
			///at the bottom of the screen.
			///@sa drawInfo()
			Difficulty& difficulty;
			///Checks the keyboard input.
			///This method checks tke arrow keys and the [Esc]. [Up] and [Down] keys are changing the
			///current item in the vertical main menu and the [Left] and [Right] keys are switching the
			///current item in sub menu (if it exist). [Esc] key causes the done() method to be called
			virtual void checkInput();
			///Refreshes the menu scene.
			///This method does all the job connected to drawing the menu. It first checks the user input
			///and then draws the vertical main menu and optionally the horizontal sub menu.
			virtual void refresh();
			///Pure virtual method used to build the menu hierarchy.
			///This method needs to have a definition in all derived class. You should put all the
			///menu building routins (like addItem()) here.
			virtual void buildMenu() = 0;
		public:
			///Constructor.
			///Only passes some arguments to the base class or saves other in internall variables.
			///@param iWin Parent menu window.
			///@param iDifficulty Current game difficulty, used to draw the game info using drawInfo()
			MenuScene(CuTeWindow& iWin, Difficulty& iDifficulty);
		};

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif		//#define MENU_H

//----------------------------------------------------------------------------
