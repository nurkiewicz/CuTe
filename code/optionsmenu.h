//----------------------------------------------------------------------------

///@file
///Options menu classes declarations.
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

//----------------------------------------------------------------------------

#ifndef OPTIONS_MENU_H
#define OPTIONS_MENU_H

//----------------------------------------------------------------------------

#include <boost/filesystem/operations.hpp>
#include "highscores.h"
#include "game.h"
#include "menu.h"
namespace filesys = boost::filesystem;

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///Options menu GL scene.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///This class is responsible for displaying and handling the options menu. This menu gives access
	///to various settings connected with the game, display, etc.
	class Options: public MenuScene
		{
		private:

			///Change keyboard controls menu.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///This OpenGL scene draws the menu with all customizable game actions, such as rotating,
			///moving, controlling camera, etc. Menu allows the user to view and change the controls.
			class ControlsChange: public MenuScene
				{
				private:
					///Maximum length of the line with description.
					///Every game action has some description stored in language XML file. This description can
					///be very long so it is needed to split it into several lines. This constant stores the
					///maximum number of characters in one line. If the description is longer than this
					///it is split out into lines shorter than DESC_LINE_MAX_LENGTH.
					static const int DESC_LINE_MAX_LENGTH = 36;
					///Original controls reference.
					///This reference stores a pointer to the original controls, i.e. the controls before
					///accessing the ControlsChange menu. This object is used to check whether the key for some
					///particular action was changed in the menu (this is done by comparing with newControls).
					///The origControls object is replaced by newControls in destructor (every time user leaves
					///thr menu).
					///@sa newControls
					Game::Controls& origControls;
					///New controls choosen by the user.
					///This class stores the copy of origControls at the beginning. Later, as far as user
					///changes the controls, is stores the new key codes.
					///@sa origControls
					Game::Controls newControls;
					///Builds the ControlsChange menu hierarchy.
					///This menu hierarchy is actually very simple - every item is one customizable action key
					///in the game. User can change the key by entering the selected item (action).
					void buildMenu();
					///Waiting for the new key at the moment flag.
					///This flag is set when user enters some menu item indicating his willingness to change
					///the key. This turns off the base class checkInput() (so the user can press arrows,
					///[Enter], etc. without manipulating menu) and the checkInput() checks whether any key
					///was pressed.
					///@sa checkInput()
					bool isChanging;
					///Checks the keyboard input in the menu.
					///When isChanging flag isn't set (normally) this method simply calls the base class version
					///giving the user ability to move across the menu. But when he enters some item, the
					///isChanging flag is set and behaviour is much different. It checks all the keyboard keys
					///and if any was pressed (see notes) sets this key to the selected action.
					///@note Actually not all keys are valid. First of pressing [Esc] will simply leave the
					///changing mode (isChanging will be unset). Secondly when user presses the key already
					///assigned to some different game action, the warning message would appear and he
					///would have to press so different key again.
					///@sa isChanging
					///@sa lastConflictAction and lastPressedKey
					void checkInput();
					///Displays the key controls info.
					///The base class version does all the job of rendering the menu itself. But this method
					///input the additional messages connected to the selected item (action) like current key
					///assigned to the action as well as the previous one if they've changed.
					///@sa showDesc()
					void refresh();
					///Shows the description of the specified action.
					///Description is the long text message explaining the meaning of the choosen action.
					///This distinction was needed because the short action string (used as a label in a menu)
					///is often unsufficient to full exaplain the meaning.
					///@param action Action code, should be one of the constants defined in Controls class.
					void showDesc(int action);
					///Action code which has the last pressed key assigned to it.
					///When the user chooses the key which is already assigned to some action, this variable
					///stores the number of this conflicting action.
					///@sa lastPressedKey
					int lastConflictAction;
					///Last key which was pressed while changing the action key.
					///This value is used only when some key conflict occurs. The conflict key can be drawn
					///even if it's not pressed now.
					///@sa lastConflictAction
					unsigned char lastPressedKey;
					///Cursor blinking rate.
					///Cursor is shown when the user is asked to enter a new key for some action. It is rendered
					///in refresh() method.
					///@sa cursorTimer
					///@sa refresh()
					static const int CURSOR_BLINK_RATE = 200;
					///Timer used to create the cursor blinking.
					///@sa cursorVisible
					///@sa CURSOR_BLINK_RATE
					MyOGL::Timer cursorTimer;
					///Indicates whether the cursor is visible.
					///When this flag is true, the cursor ('_' character) should be put on screen. The flag is
					///changed every time the cursorTimer exceeds CURSOR_BLINK_RATE.
					///@sa CURSOR_BLINK_RATE
					///@sa refresh()
					bool cursorVisible;
				public:
					///Sets up some essential variables and build menu.
					///@param win Parent window, used to get the keyboard state and OGL extensions.
					///@param controls Most important parameter, contains the original game controls.
					///This class is copierd into newControls and overriden by newControls in destructor.
					///@param difficulty Game difficulty is given as a parameter to the base class.
					///It is needed because the difficulty info is shown in every CuTe menu.
					ControlsChange(CuTeWindow& win, Game::Controls& controls, Difficulty& difficulty);
					///Saves back the controls.
					///Overrides the origControls data with newControls. This causes to set the game controlers
					///to the new ones changed by the user.
					~ControlsChange()	{origControls = newControls;}
				};		//class Options: public MenuScene

			///Player name edit item.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///It gives user ability to edit its name (nick). This name is then saved with high scores.
			class PlayerNameItem: public MenuTextItem
				{
				private:
					///Name string alpha value changing speed.
					///When this menu item gots or losts focus the outline fonted player name
					///becomes adequately less or more translucent. The speed of this change is stored
					///in this constant.
					///@sa alpha variable
					///@sa update()
					static const float NAME_EDIT_ALPHA_SPEED;
					///Reference to player name string.
					///The menu item does not hold the string itself, it only has a reference to it.
					///The variable is stored in menu scene object.
					///@sa addChar()
					std::string& playerName;
					///Checks for keyboard input and changes alpha value.
					///This method calls checkInput() to gain information about which keys were pressed
					///and updates the alpha value depending on alphaTimer time.
					///@sa checkInput()
					///@sa alphaTimer
					void update(bool isCurrent);
					///Window object reference.
					///This object is needed in order to gain information about keys pressed in inputCheck()
					///method.
					///@sa inputCheck()
					///@sa MyOGL::Window class
					MyOGL::Window& win;
					///Adds one character to the end of player name string.
					///Keeps track of player name string length and don't add next character if its
					///length exceed HighScore::PLAYER_NAME_MAX_LENGTH.
					///@sa HighScore::PLAYER_NAME_MAX_LENGTH
					///@sa inputCheck()
					void addChar(char c);
					///Checks for keyboard input.
					///Reads letters, digits, [Shift] and [Backspace] keys. Then adds valid characters
					///to player name string using addChar() method.
					///@sa addChar()
					void checkInput();
					///Alpha value timer.
					///This timer is used when updating the player name string alpha value in update().
					///@sa update()
					///@sa alpha
					MyOGL::Timer alphaTimer;
					///Current player name string alpha shift.
					///Thanks to this variable object creates nice impression of appering and disappering
					///of player name string.<br>
					///This value is updated depending on alphaTimer time in update()
					///@sa update()
					///@sa alphaTimer
					float alpha;
				public:
					///Constructor.
					///@param iWin Window object is used to gain information about which keys were pressed.
					///@param iPlayerName Reference to external player name string; the string itself will
					///be modified in this object.
					///@sa win
					///@sa playerName
					PlayerNameItem(MyOGL::Window& iWin, std::string& iPlayerName);
					///Draws the menu item.
					///Calls the base class MenuItem::draw() to draw the menu item label and then, if desired
					///draws the player name string with proper alpha value.
					///@param isCurrent Tells whether the menu item is current in main menu.
					///@sa alpha
					void draw(bool isCurrent);
				};		//class PlayerNameItem: public MenuTextItem

			///Video mode menu item class.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///This class is used to choose the desired video mode used in CuTe.
			class VideoItem: public MenuTextItem
				{
				private:
					///Current video mode used in CuTe.
					///This should be one of the CuTe video mode flags declared in CuTeWindow class.
					///@sa CuTeWindow class
					int curMode;
				public:
					///Constructor.
					///Passes all necessery argument to the base class and saves the
					///current video mode index. The constructor creates all the sub menu items so
					///the MenuScene::buildMenu() method does not have to do this.
					///@param parent Used window object. Needed to gain information about current video mode
					///and window Extensions class.
					///@sa curMode
					VideoItem(CuTeWindow& parent);
					///Draws the sub menu + additional info.
					///Calls the base class version to draw the sub menu and prints additional video
					///information (screen resolutions and the info to press [Enter] to restart).
					void draw(bool isCurrent);
				};		//class VideoItem: public MenuTextItem

			///Game language options menu item class.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///This object is responsible not only for displaying the menu including all available
			///languages, but also it searches the "lang" subdirectory and accumulates the information
			///about all the available alnguages.
			class LanguageItem: public MenuTextItem
				{
				private:

					///Stores the information about one particular language.
					///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
					///@date Jul 2005-Mar 2006
					struct LangFileInfo
						{
						///Path to the language XML file.
						///Path includes the "lang/" directory path.
						filesys::path filePath;
						///Name of the language in English.
						///For example "Polish".
						///@sa nameNative
						std::string nameEnglish;
						///Name of the language in English.
						///For example "Polski".
						///@sa nameEnglish
						std::string nameNative;
						///Name of the author of the language pack.
						///@sa email
						std::string author;
						///E-mail to the language pack's author.
						///@sa author
						std::string email;
						///Game version for which the language pack is dedicated.
						///To be sure that the language pack covers all the program messages, this version should
						///be same as the program version.
						std::string version;
						///Date when the language pack was released.
						std::string date;
						///Loads the language info from the specified XML file.
						///@param iFilePath File path to the XML file containing the language info.
						///@sa filePath
						LangFileInfo(const filesys::path& iFilePath);
						};		//struct LangFileInfo

					///Vector of all language packs info.
					///This vectors containes the LangFileInfo objects, and that store all the language
					///pack information, like name, author, etc. This information is used directly
					///while drawing the sub menu.
					///@sa draw()
					///@sa curLang
					std::vector<const LangFileInfo> langFiles;
					///XML key specifying the current language file.
					///This key corresponds to the user file options/language key. It is set in constructor
					///and updated (if user changed the current language) in the destructor.
					///@sa LanguageItem()
					MyXML::Key& langKey;
					///Currently choosen language.
					///This is actually the index in langFiles vector. This value is used to determinate
					///whether the currently choosen position in languges sub menu is the same as the
					///curretnly used language; if not - restart information message is displayed and
					///the needRestart flag is set.
					///@sa langFiles
					///@sa needRestart
					int curLang;
					///Need to restart after changing the language flag.
					///When the language choosen in languages sub menu differs from the one currently
					///used, this flag is set. It is actually a pointer to the same flag located in the
					///Options object.
					bool& needRestart;
					///Adds information about one particular language to langFiles list.
					///Opens the XML language file located at the given file path, extracts the language
					///info ("info" key in XML file) and puts it at the end of the langFiles list.<br>
					///Also checks whether the currently extracted language isn't the selected one - if it is,
					///sets the sub menu position to it.
					///@param filePath path to some XML file which should contain the language pack data.
					///@sa langFiles
					void addLangItem(const filesys::path& filePath);
				public:
					///Studies all the language files and saves information about them.
					///Iterates through all the files located in language subdirectory and calls addLangItem()
					///for each one.
					///@param extensions Extensions object needed by base classes.
					///@param iLangKey Initial value for langKey
					///@param iNeedRestart Initial value for needRestart
					///@sa addLangItem()
					///@sa langKey
					///@sa needRestart
					LanguageItem(MyOGL::Extensions& extensions, MyXML::Key& iLangKey, bool& iNeedRestart);
					///Saves back the selected language.
					///If the user changed the current language using sub menu, the new language file
					///is saved to the user XML key here.
					///@sa langKey
					~LanguageItem()	{langKey = langFiles[subMenu()->currentIndex()].filePath.leaf();}
					///Draws the languages sub menu.
					///Shows all the previously collected information about the currently choosen language.
					void draw(bool isCurrent);
				};		//class LanguageItem: public MenuTextItem

			///Player name options menu item index.
			static const int PLAYER_NAME = 0;
			///Cuboid size options menu item index.
			static const int CUBOID_SIZE = 1;
			///Cuboid depth options menu item index.
			static const int CUBOID_DEPTH = 2;
			///Blocks set options menu item index.
			static const int BLOCKS_SET = 3;
			///Video options menu item index.
			static const int VIDEO = 4;
			///Language options menu item index.
			static const int LANGUAGE = 5;
			///Keyboard options menu item index.
			static const int KEYBOARD = 6;
			///Sounds options menu item index.
			static const int SOUNDS = 7;

			///Build the whole menu hierarchy.
			///This method creates all the menu items and sub items in the options menu.
			///@sa MenuScene::buildMenu() base class declarations.
			void buildMenu();
			///Checks some input and updates menu positions.
			///This method mostly updates the sizes menus depending on the menus current items. Because the
			///base class version (which is called at the beginning) does all the job of reading the
			///keyabord arrows, all this method have to do is to read the [Enter] if it was pressed
			///in video menu.
			void checkInput();
			///User data XML key.
			///This key (corresponding to the user.xml's "options" key) stores all the game settings,
			///including difficulty, language, player name, etc. The class first reads this information
			///while initializing and then saves it back again (probably changed) in the destructor.
			///@sa ~Options()
			MyXML::Key& user;
			///Reference to the game keyboard controls.
			///This variable points to the Controls object in MainMenu. It is used in nested menu
			///ControlsChange to read and change the keyboard codes.
			Game::Controls& controls;
			///Restart whole game flag.
			///This flag is used by some nested objects (like LanguageItem) to indicate that something
			///very important has been done (like changing the current language) and the whole game
			///should be restarted.<br>
			///When this flag is set the Options object calls the restart() method.
			bool needRestart;
		public:
			///Builds menu and sets up some menu positions.
			///Constructor first calls the buildMenu() method to build the menu hierarchy and then
			///sets up the proper menu positions.
			///@param win Parent menu window object.
			///@param iUser XML key containing the game options.
			///@param difficulty Current game difficulty; this reference is needed when the base class
			///@param iControls Game keyboard control virtual key codes. This is used in Options menu.
			///draws the game info at the bottom of the screen.
			///@sa buildMenu
			///@sa user key
			Options(CuTeWindow& win, MyXML::Key& iUser, Difficulty& difficulty, Game::Controls& iControls);
			///Saves back the information which might have been changed.
			///Sets up new player name and video mode values in the user XML key. The key itself is
			///saved back to a user.xml file in the main.cpp file.
			~Options();
		};		//class Options: public MenuScene

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif		//#define OPTIONS_MENU_H

//----------------------------------------------------------------------------
