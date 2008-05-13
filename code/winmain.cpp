//----------------------------------------------------------------------------

///@file
///CuTe main implementation file.
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
///This file only includes main() function. All essential parts of the game
///are stored in other header and implementation files.
///@todo Create CuTe screen saver
///@todo More high scores (maybe some scroll in menu?)

//----------------------------------------------------------------------------

#include <exception>
#include <fstream>
#include "language.h"
#include "mainmenu.h"
#include "intro.h"
#include "sounds.h"
using namespace CuTe;

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///Definition of a langData variable.
	///This variable is declared in the language.h file. It stores all the messages displayed in the
	///game allowing to introduce multiple languages to the game.
	MyXML::Key langData;

	///Definition of a langInfo variable.
	///This variable is declared in the language.h file. It stores the language additional information
	///like language pack author, fonts, etc.
	MyXML::Key langInfo;

//----------------------------------------------------------------------------

	///Game options information key.
	///This XML key inherited class not only stores the game options information
	///but loads this data from a file in constructor and saves it back (probably after some
	///modifications) in destructor. It is designed to create an empty options file when it does not
	///exist rather than crashing or interrupting.
	///@par Language loading
	///This class also loads the the language data from a file. The name of this file is stored
	///in the user key, so I've decided to put this code here.
	class FileOptions: public MyXML::Key
		{
		public:
			///Loads user data from XML file if it exists.
			///@sa userFileName
			FileOptions();
			///Saves the XML key with user data into user data file.
			///If the file wasn't existing, it will be created here.
			///@sa userFileName
			~FileOptions()	{saveToFile(userFileName);}
			///Loads the language data from XML file.
			///The XML fiel containg the choosen language data is saved in the user key.
			///This function first loads the whole key from the specified file and then copies the
			///file content to the two XML language keys: langData (messages) and langInfo (additonal
			///language information).
			///@sa langData and langInfo global variables
			void loadLanguage();
		private:
			///Name of the user data file.
			///This file name is used both in constructor when opening user data file and in destructor
			///when saving it.
			static const std::string userFileName;
		};		//class FileOptions: public MyXML::Key

	const std::string FileOptions::userFileName = "options.xml";

	FileOptions::FileOptions()
		{
		if(boost::filesystem::exists(userFileName))
			loadFromFile(userFileName);
		else
			value() = "options";
		sounds.enable(MyXML::readKeyDef((*this)["sounds"], std::string("1")) == "1");
		loadLanguage();
		}

	void FileOptions::loadLanguage()
		{
		MyXML::Key langFile("lang/" +
			MyXML::readKeyDef((*this)["language"], std::string("polish.xml")));
		langData = langFile["msg"];
		langInfo = langFile["info"];
		}

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

///WinAPI main() function.
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
	{
	srand(time(NULL));		//initialize random generator
	try
		{
		FileOptions FileOptions;		//object for storing the game options
		bool restart;
		do
			{		//main game objects
			FileOptions.loadLanguage();
			CuTeWindow win(MyXML::readKeyDef(FileOptions["video"], CuTeWindow::F_800x600));
			Intro intro(win);
			intro.start();
			MainMenu menu(win, FileOptions);
			menu.start();
			restart = menu.restart();
			}
		while(restart);
		}
	catch(const std::exception &e)
		{
		MessageBox(NULL, e.what(), "CuTe exception", MB_OK | MB_ICONEXCLAMATION);
		}
	return 0;
	}		//int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)

//----------------------------------------------------------------------------
