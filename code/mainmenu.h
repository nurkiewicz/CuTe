//----------------------------------------------------------------------------

///@file
///CuTe main menu scene classes.
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
///This file contains declarations of main menu scene classes.
///@todo Add possibility to create HTML high scores file with as many statistics as possible :-)

//----------------------------------------------------------------------------

#ifndef CUTE_H
#define CUTE_H

//----------------------------------------------------------------------------

#include "optionsmenu.h"

//----------------------------------------------------------------------------

///Namespace containing core and interface classes for CuTe.
namespace CuTe
	{

//----------------------------------------------------------------------------

	///Game menu scene.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///This class handles all the data and actions connected to game menu. It not only
	///reads and saves the options (including high scores) but starts new games as well.
	class MainMenu: public MenuScene
		{
		public:
			///Mostly creates menu hierarchy.
			///Sets up some essential variables and calls buildMenu() to create
			///the menu structure. It also loads all the fonts used by the game. This process
			///was delayed to this moment (fonts are not loaded when the window is created)
			///to speed-up the loading of intro animation.
			///@param win Parent menu window.
			///@param iOptions XML key containing hte game options.
			///@sa buildMenu()
			MainMenu(CuTeWindow& win, MyXML::Key& iOptions);
		private:

			///Container class for all high scores (for all difficulty levels)
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///This class stores container of HighScores objects. Every HighScores object stores high
			///scores for one particular difficulty level and this objects collects high scores for all
			///high scores in the game.<br>
			///The object automatically loads highscores from crypted data file, decrypts it
			///and crypt back when saving.
			class AllHighScores
				{
				public:
					///Loads all the high scores data into highScores container.
					///Goes through all the "difficulty" keys nested inside "highscores" and calls
					///loadDifficultyScore() for all of them
					///@param iDifficulty AllHighScores objects need to track the current difficulty level
					///to give access to proper map container elements (see curDifficulty and current())
					///@sa loadDifficultyScore()
					AllHighScores(DifficultyData& iDifficulty);
					///Saves all the high scores data into highScores container.
					///Goes through all the "difficulty" keys nested inside "highscores" and calls
					///loadDifficultyScore() for all of them
					///@sa loadDifficultyScore()
					~AllHighScores();
					///Returns all high scores for a current difficulty level.
					///Although the AllHighScores object stores the information about all high scores
					///(high scores for all difficulty levels) it gives external access only to the high
					///scores for the current difficulty level. This way the user can't access the rest of
					///high scores.
					///@return Reference to the HighScores object containing all high scores for the current
					///difficulty level.
					HighScores& operator()()	{return highScores[curDifficulty];}
				private:

					///Crypts/decrypts a file using XOR cipher.
					///@param inputFile Path and name of an input file name
					///(before crypting/encrypting)
					///@param outputFile Path and name of an output file name
					///(after crypting/encrypting)
					///@param xor XOR value used in cipher algorithm. Pleas note that for xor == 0
					///output file is just the same as input (for every x: x^0 == x)
					///@return True if input file exists and the crypt/decrypt process has been
					///done properly.
					static bool xorFile(const std::string& inputFile, const std::string& outputFile,
						unsigned char xor);
					///Stores the file name of a high scores data file (.dat).
					static const std::string fileNameCrypted;
					///Stores the file name after decryption (.xml).
					static const std::string fileNameDecrypted;
					///XOR value used when saving.
					///XML file storing high scores is crypted using very simple XOR cipher.
					static const unsigned char XOR_VALUE = 0xCC;
					///All high scores container.
					///First element (key) is a difficulty level for which the high scores are collected.
					///The second one (value) are the high scores itself. To gain information about all
					///high scores in one particular difficulty level, pass the difficulty level info
					///to this container.
					///@par Input and output
					///All the data stored in this container are read in constructor from a given XML key
					///and they are saved again (probably after some modification - new high scores) in
					///destructor.
					///@sa loadDifficultyScores()
					///@sa current()
					std::map<const DifficultyData, HighScores> highScores;
					///Reference to current difficulty object.
					///This object is used to determinate which high scores saved in a container highScores
					///to return in current() function. Thanks to this reference the object always knows the
					///current difficulty level and despite it stores all the high scores, it gives access
					///only to the one which are currently needed.
					///@sa current()
					const DifficultyData& curDifficulty;
					///XML key representing the "highscores" key user XML file.
					///This key is filled in the constructor by opening and reading name called
					///fileName (const).<br>
					///@sa loadDifficultyScores()
					///@sa fileName
					MyXML::Key highScoresKey;
					///Loads all the high scores data from XML key.
					///This method is called in constructor to load high scores for all saved difficulty
					///levels.
					///@par
					///The method loads the high scores data for one particular difficulty level (it creates
					///one new key in highScores container).
					///@param data XML key representing one "difficulty" key inside "highscores" key.
					///@sa AllHighScores()
					///@sa highScores
					void loadDifficultyScores(const MyXML::Key& data);
				};

			///New game menu item.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///This class also displayes the high scores information for current difficulty level.
			///@sa AllHighScores class
			class NewGameItem: public MenuTextItem
				{
				private:
					///High scores panel X axis maximum shift.
					///This value sotres the information how far to move the high scores panel right to
					///make it completle disapper beyond screen edge. This is used while animating the
					///high scores panel after making the New Game item current.
					///@sa HS_PANEL_X_SHIFT
					///@sa xShift
					static const float HS_PANEL_X_SHIFT;
					///High scores panel animation speed.
					///Tells how fast does the high scores panel move during its animation.
					///@sa HS_PANEL_X_SHIFT
					///@sa update()
					static const float HS_PANEL_X_SPEED;
					///High scores animation timer.
					///Is used for animating the high scores panel in update() method
					///@sa update()
					MyOGL::Timer timer;
					///Current High scores panel X position shift.
					///When this value is set to 0.0 it might mean that: when this menu item has focus,
					///the panel is on its solid position; when the New Game item isn't current, the panel
					///is invisible (somewhere beyond the screen edge).<br>
					///But when the value is nonzero, it means that the panel is moving left to right or
					///vice-versa.
					///@sa update()
					float xShift;
					///Updates the high scores panel position.
					///This method (except calling the base class version) sets up new xShift position - so
					///it is repsonsible for creating the high scores panel animation.
					///@sa xShift
					void update(bool isCurrent);
					///Reference to all high scores data container.
					///This object stores the data of all high scores. Of course having access to it is
					///necessery to display the high scores panel.
					///@sa draw()
					AllHighScores& highScores;
				public:
					///Constructor.
					///@param extensions MyOGL::Extensions object needed to be passed to base class.
					///@param iHighScores Reference to all high scroes data container.
					///@sa AllHighScores class
					NewGameItem(MyOGL::Extensions& extensions, AllHighScores& iHighScores);
					///Draws the menu item and high scores panel.
					///@sa highScores
					void draw(bool isCurrent);
				};		//class NewGameItem: public MenuTextItem

			///Display help menu.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///Display sub menu with help about rotating, moving, using mouse and camera control.
			class HelpItem: public MenuTextItem
				{
				private:
					///How fast is the example model block rotating.
					///This value is also used to calculate the speed of example block move, because the
					///moving procedure uses this angle as an argument of sine function.
					///@sa rotateAnimation()
					static const float ROTATION_SPEED;
					///Amplitude of example block move.
					///@sa moveAnimation()
					static const float MOVE_AMPLITUDE;
					///Timer used to calculate the model block angle and position.
					///@sa update()
					///@sa ROTATION_SPEED
					MyOGL::Timer timer;
					///Current model block angle.
					///Angle is in degrees. It is also used in block moving (see ROTATION_SPEED).
					///@sa update()
					float rot;
					///Updates model blocks position.
					///Updates the rot value position so that the model blocks are rotating or moving.
					///@sa rot
					void update(bool isCurrent);
					///Creates blocks rotation animation.
					///Draws three blocks, each of which are rotating clockwise around X, Y or Z axis.
					///@sa moveAnimation()
					///@sa draw()
					void rotateAnimation();
					///Creates blocks move animation.
					///Draws three blocks, each of which are moving along X, Y or Z axis.
					///@sa rotateAnimation()
					///@sa draw()
					void moveAnimation();
					///OpenGL list index name where the help model block is saved.
					///The help model block is built in buildModel() method. The block is then used in
					///moveAnimation() and rotateAnimation()
					///@sa buildModel()
					const GLuint modelList;
					///Puts a centered string to a specified position.
					///Moves the current bitmap fonts position to (x, y) and prints the message.
					///The message is centered, so that x is the position of the middle of the message.
					///@param info String containing the message
					///@param x X coordinate of the middle of the message
					///@param y Y coordinate of the middle of the message
					void putString(const std::string& info, float x, float y)
						{bitmapFonts.pos(x - info.length() * 0.0215 / 2.0, y) << info;}
					///Currently used keyboard control codes.
					///When help screen displays keys which are responsible for particular actions, it always
					///displays the keys which are currently used, not the default ones. In order to do this,
					///the help object must always have access to the controls key codes container class
					///Controls
					const Game::Controls& controls;
				public:
					///Constructs base class and builds block model.
					///@param extensions Contains references to bitmap and outline fonts
					///@param helpItemModel XML key containing coded OpenGL commands about model block.
					///@param iControls See controls for details about the purpose of this argument.
					///@sa buildModel()
					HelpItem(MyOGL::Extensions& extensions, const MyXML::Key& helpItemModel,
						const Game::Controls& iControls);
					///Draws the sub menu and help panels.
					///If the item is current, displays the desired (on the base of sub menu current item)
					///help panel.
					void draw(bool isCurrent);
				};		//class HelpItem: public MenuTextItem

			///Displays About info.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///About info includes author, e-mail and WWW addresses and some extract from license.
			class AboutItem: public MenuTextItem
				{
				private:
					///Next message character delay.
					///About message does not appear suddenly but instead it is shown one character by another.
					///This makes a nice impression of "typewriter" (especially that the blinking cursor is
					///visible). This value tells how much time (in ms) it has to elapse to print the next
					///character in the message.
					///@sa update()
					///@sa aboutMsg
					///@sa msgTimer
					static const int CHAR_DELAY = 15;
					///About message content.
					///This string contains the whole about message including new line characters and some
					///special tags which represents abstract data. Those tags are:
					/// <table><tr><td><b>Tag</b></td><td><b>Meaning</b></td></tr>
					/// <tr><td>$a</td><td>Author</td></tr>
					/// <tr><td>$d</td><td>Date (year only)</td></tr>
					/// <tr><td>$m</td><td>E-mail address</td></tr>
					/// <tr><td>$t</td><td>Switch to tiny font</td></tr>
					/// <tr><td>$v</td><td>Program version</td></tr>
					/// <tr><td>$w</td><td>WWW address</td></tr></table>
					///@warning
					///The aboutMsg <b>must not</b> end with tag or new line.
					///@sa draw()
					///@sa msgTimer
					const std::string aboutMsg;
					///Last character to print.
					///The whole about message is not printed onto the screen from the beginning
					///(see CHAR_DELAY for details) - this iterators points to the character <b>after</b>
					///after the last which should be printed.
					///@sa update() where this iterator is updated.
					///@sa draw()
					std::string::const_iterator last;
					///About message "typewriter" timer.
					///Used to control the @ref last iterator in update() method.
					///@sa update()
					///@sa last
					MyOGL::Timer msgTimer;
					///Updates the last about message character to be shown.
					///@sa CHAR_DELAY
					///@sa last
					void update(bool isCurrent);
				public:
					///Constructor.
					///@param extensions Extensions to be passed futher to the base class.
					AboutItem(MyOGL::Extensions& extensions);
					///Draws the menu item.
					///If the item is current, it shows the about message as well.
					void draw(bool isCurrent);
				};		//class AboutItem: public MenuTextItem

			///New game main menu item index.
			static const int NEW_GAME = 0;
			///Difficulty main menu item index.
			static const int DIFFICULTY = 1;
			///Demo mode main menu item index.
			static const int DEMO = 2;
			///Help main menu item index.
			static const int HELP = 3;
			///About main menu item index.
			static const int ABOUT = 4;
			///Options main menu item index.
			static const int OPTIONS = 5;

			///Current game difficulty.
			///Stores the information about game size, depth and blocks set. This data is used when
			///starting new game and displaying high scores panel.
			///@sa startNewGame()
			Difficulty difficulty;
			///Checks keyboard input.
			///Checks for arrows, [Enter] and [Esc] pressed.
			///@sa refresh()
			void checkInput();
			///Starts new game.
			///Creates a Game object, starts the game and optionally adds a high score into
			///highScores.
			///@sa startDemo()
			///@sa difficulty
			///@sa highScores
			void startNewGame();
			///Starts demo mode game.
			///Creates a Demo object and starts the demo mode game.
			///@sa startNewGame()
			///@sa difficulty
			void startDemo();
			///Displays the options menu.
			///Creates the Options object and starts the options menu scene. If the options menu indicates
			///restart, calls restart() method to restart the whole game.
			void optionsMenu();
			///Creates game menu hierarchy.
			///Creates all the game menu item, including main and sub items.
			///@sa menu
			void buildMenu();
			///Container for all high scores got in the game.
			///This object is used to fill the high score panel in NewGameItem.
			///@sa NewGameItem and AllHighScores class.
			AllHighScores highScores;
			///XML key containing game options.
			///This key is read from a options file and given to the class via constructor.
			///@sa MainMenu()
			MyXML::Key& options;
			///XML key containing some models used in the game.
			///It stores some OpenGL objects which will be built into display list in the future.
			///@sa HelpItem class
			const MyXML::Key models;
			///Keyboard input controls container.
			///This class stores the virtual key codes for almost all actions during the game. Reference
			///to this object is passed to the game object when new game is started.
			Game::Controls controls;
		};		//class Menu: public MyOGL::Scene

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif		//#define CUTE_H

//----------------------------------------------------------------------------
