//----------------------------------------------------------------------------

///@file
///Definitions of classes declared in cute.h
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

#define _USE_MATH_DEFINES		///<for MS VC++ compatibility (M_* are not part of the standard)
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include "language.h"
#include "mainmenu.h"
#include "demo.h"
using namespace CuTe;
using std::list;
using std::string;
using MyOGL::glColorHSV;
using boost::lexical_cast;

//----------------------------------------------------------------------------

const float MainMenu::NewGameItem::HS_PANEL_X_SHIFT = 0.85;
const float MainMenu::NewGameItem::HS_PANEL_X_SPEED = 3.5;

MainMenu::NewGameItem::NewGameItem(MyOGL::Extensions& extensions, AllHighScores& iHighScores):
	MenuTextItem(extensions, langData["mainMenu"]["newGame"]), highScores(iHighScores), xShift(0.0)
	{
	}

void MainMenu::NewGameItem::draw(bool isCurrent)
	{
	if(isCurrent != wasCurrent)
		xShift = HS_PANEL_X_SHIFT;		//set up X axis shift if New Game item got or lost focus.
	MenuItem::draw(isCurrent);		//draw menu item label
	if(isCurrent || (xShift != 0.0))
		{		//draw panel only if new game has focus or during animation
		float x = 0.25 + (isCurrent? xShift : (HS_PANEL_X_SHIFT - xShift));
		float y = 0.05;
		list<const HighScore>::const_iterator score = highScores().first();
		for(int p = 1; p <= HighScores::MAX_COUNT; ++p, y -= 0.11, x += 0.016, ++score)
			{
			const float COLOR_COEFF = 1.0 - (p - 1.0) / HighScores::MAX_COUNT;
			glColorHSV(2 * M_PI / 3, 1.0, 0.3 + 0.6 * COLOR_COEFF);		//draw score position
			bitmapFonts.pos(x, y - 0.03) << p << '.';
			glColorHSV(M_PI / 3, 1.0, 0.2 + 0.7 * COLOR_COEFF);		//draw player and points info
			bitmapFonts.pos(x + 0.07, y) << score->mainInfo();
			bitmapFonts.select(1);		//tiny time info
			glColorHSV(4 * M_PI / 3, 0.1, 0.2 + 0.7 * COLOR_COEFF);
			bitmapFonts.pos(x + 0.12, y - 0.05) << score->timeInfo();
			bitmapFonts.select(0);
			}
		}
	}

void MainMenu::NewGameItem::update(bool isCurrent)
	{
	MenuTextItem::update(isCurrent);
	decAbs<float>(xShift, timer.restart() / 1000.0 * HS_PANEL_X_SPEED);
	}

//----------------------------------------------------------------------------

const float MainMenu::HelpItem::ROTATION_SPEED = 200.0;
const float MainMenu::HelpItem::MOVE_AMPLITUDE = 0.3;

MainMenu::HelpItem::HelpItem(MyOGL::Extensions& extensions, const MyXML::Key& helpItemModel,
	const Game::Controls& iControls):
	MenuTextItem(extensions, langData["mainMenu"]["help"]), rot(0.0),
		modelList(buildDisplayList(helpItemModel)), controls(iControls)
	{
	MyXML::KeysRange items = langData["mainMenu"].keys("helpSubItem");
	for(MyXML::KeyIterator i = items.first; i != items.second; ++i)
		addSubItem(new MenuItem(extensions.outlineFonts(), i->second));
	}

void MainMenu::HelpItem::draw(bool isCurrent)
	{
	MenuItem::draw(isCurrent);
	const MyXML::Key& helpStrings = langData["help"];
	string tempStr;		//will be used in some transformations
	if(isCurrent)
		switch(subMenu()->currentIndex())
			{
			case 0:		//rotation help
				rotateAnimation();		//animation
				glColorHSV(0.0, 1.0, 0.8);		//and help text
				putString(Game::Controls::keyCodeToStr(controls(controls.ROTATE_XCW)), 0.18, -0.25);
				putString(Game::Controls::keyCodeToStr(controls(controls.ROTATE_XCCW)), 0.18, -0.35);
				glColorHSV(2 * M_PI / 3, 1.0, 0.8);
				putString(Game::Controls::keyCodeToStr(controls(controls.ROTATE_YCW)), 0.48, -0.25);
				putString(Game::Controls::keyCodeToStr(controls(controls.ROTATE_YCCW)), 0.48, -0.35);
				glColorHSV(4 * M_PI / 3, 1.0, 0.8);
				putString(Game::Controls::keyCodeToStr(controls(controls.ROTATE_ZCW)), 0.78, -0.25);
				putString(Game::Controls::keyCodeToStr(controls(controls.ROTATE_ZCCW)), 0.78, -0.35);
				break;
			case 1:		//move help
				moveAnimation();
				glColorHSV(0.0, 1.0, 0.8);		//and help text
				putString(Game::Controls::keyCodeToStr(controls(controls.MOVE_LEFT)), 0.18, -0.25);
				putString(Game::Controls::keyCodeToStr(controls(controls.MOVE_RIGHT)), 0.18, -0.35);
				glColorHSV(2 * M_PI / 3, 1.0, 0.8);
				putString(Game::Controls::keyCodeToStr(controls(controls.MOVE_UP)), 0.48, -0.25);
				putString(Game::Controls::keyCodeToStr(controls(controls.MOVE_DOWN)), 0.48, -0.35);
				glColorHSV(4 * M_PI / 3, 1.0, 0.8);
				putString(Game::Controls::keyCodeToStr(controls(controls.MOVE_FORWARD)), 0.78, -0.3);
				break;
			case 2:		//camera help
				glColorHSV(0.0, 0.0, 0.9);
				bitmapFonts.pos(0.1, 0.1)
					<< Game::Controls::keyCodeToStr(controls(controls.CAMERA_SET_TEMPORARY)) << "/"
					<< Game::Controls::keyCodeToStr(controls(controls.CAMERA_SET_PERMANENT)) << " + :";
				glColorHSV(0.0, 0.0, 0.6);
				bitmapFonts.pos(0.2, 0.0) << helpStrings["cameraArrows"].value();
				bitmapFonts.pos(0.2, -0.05) << Game::Controls::keyCodeToStr(controls(controls.CAMERA_ZOOM_IN)) << '/'
					<< Game::Controls::keyCodeToStr(controls(controls.CAMERA_ZOOM_OUT)) << helpStrings["cameraZoomInOut"].value();
				bitmapFonts.pos(0.2, -0.1) << helpStrings["cameraRestorePositions"].value();
				bitmapFonts.pos(0.2, -0.15) << helpStrings["cameraSavePositions"].value();
				bitmapFonts.pos(0.2, -0.2) << Game::Controls::keyCodeToStr(controls(controls.CAMERA_DEFAULT_POS))
					<< helpStrings["cameraBack"].value();
				glColorHSV(0.0, 0.0, 0.4);
				bitmapFonts.pos(0.2, -0.3);
				tempStr = helpStrings["cameraPermChange"];
				bitmapFonts << tempStr.substr(0, tempStr.find('|')) <<
					Game::Controls::keyCodeToStr(controls(controls.CAMERA_SET_PERMANENT)) << tempStr.substr(tempStr.find('|') + 1);
				bitmapFonts.pos(0.2, -0.35);
				tempStr = helpStrings["cameraTempChange"];
				bitmapFonts  << tempStr.substr(0, tempStr.find('|')) <<
					Game::Controls::keyCodeToStr(controls(controls.CAMERA_SET_TEMPORARY)) << tempStr.substr(tempStr.find('|') + 1);
				glColorHSV(0.0, 0.0, 0.8);
				bitmapFonts.pos(0.1, -0.45) << Game::Controls::keyCodeToStr(controls(controls.CAMERA_MODE_0))
					<< helpStrings["cameraBottomMode"].value();
				bitmapFonts.pos(0.1, -0.5) << Game::Controls::keyCodeToStr(controls(controls.CAMERA_MODE_1))
					<< helpStrings["cameraTopMode"].value();
				bitmapFonts.pos(0.1, -0.55) << Game::Controls::keyCodeToStr(controls(controls.CAMERA_MODE_2))
					<< helpStrings["cameraBlockMode"].value();
				break;
			case 3:		//mouse help
				rotateAnimation();
				glColorHSV(0.0, 1.0, 0.8);		//and help text
				putString(langData["keyNames"]["up"], 0.18, -0.25);
				putString(langData["keyNames"]["down"], 0.18, -0.35);
				glColorHSV(2 * M_PI / 3, 1.0, 0.8);
				putString(langData["keyNames"]["left"], 0.48, -0.25);
				putString(langData["keyNames"]["right"], 0.48, -0.35);
				glColorHSV(4 * M_PI / 3, 1.0, 0.8);
				putString(helpStrings["mouseWheel"], 0.78, -0.3);
				glColorHSV(0.0, 0.0, 0.8);
				putString(helpStrings["mouseRight"], 0.5, -0.45);
				putString(helpStrings["mouseLeft"], 0.5, -0.55);
				break;
			}
	}

void MainMenu::HelpItem::update(bool isCurrent)
	{
	MenuTextItem::update(isCurrent);
	if(isCurrent)
		{
		rot += timer.restart() / 1000.0 * ROTATION_SPEED;
		if(rot >= 360.0)
			rot -= 360.0;
		}
	}

void MainMenu::HelpItem::rotateAnimation()
	{
	glLoadIdentity();
	glTranslatef(0.5, 0.0, -6.0);
	glRotatef(rot, -1.0, 0.0, 0.0);
	glCallList(modelList);
	glLoadIdentity();
	glTranslatef(1.4, 0.0, -6.0);
	glRotatef(rot, 0.0, -1.0, 0.0);
	glCallList(modelList);
	glLoadIdentity();
	glTranslatef(2.3, 0.0, -6.0);
	glRotatef(rot, 0.0, 0.0, -1.0);
	glCallList(modelList);
	}

void MainMenu::HelpItem::moveAnimation()
	{
	glLoadIdentity();
	glTranslatef(0.5, 0.0, -6.0);
	glTranslatef(MOVE_AMPLITUDE * sin(rot * M_PI / 180), 0.0, 0.0);
	glCallList(modelList);
	glLoadIdentity();
	glTranslatef(1.4, 0.0, -6.0);
	glTranslatef(0.0, MOVE_AMPLITUDE * sin(rot * M_PI / 180), 0.0);
	glCallList(modelList);
	glLoadIdentity();
	glTranslatef(2.3, 0.0, -6.0);
	glTranslatef(0.0, 0.0, MOVE_AMPLITUDE * sin(rot * M_PI / 180));
	glCallList(modelList);
	}

//----------------------------------------------------------------------------

MainMenu::AboutItem::AboutItem(MyOGL::Extensions& extensions):
	MenuTextItem(extensions, langData["mainMenu"]["about"])
	{
	string& about = const_cast<string&>(aboutMsg);
	about = langData["misc"]["about"];
	about.replace(aboutMsg.find("$v"), 2, "0.0");
	about.replace(aboutMsg.find("$d"), 2, "2005");
	about.replace(aboutMsg.find("$a"), 2, "Tomasz Nurkiewicz");
	about.replace(aboutMsg.find("$m"), 2, "T.Nurkiewicz@stud.elka.pw.edu.pl");
	about.replace(aboutMsg.find("$w"), 2, "http://www.cute.xt.pl");
	about += "\n\n" + langInfo["about"]["translationL"].value() + langInfo["about"]["author"].value()
		+ " (" + langInfo["about"]["version"].value() + ", " + langInfo["about"]["date"].value() + ')';
	}

void MainMenu::AboutItem::draw(bool isCurrent)
	{
	MenuItem::draw(isCurrent);
	if(isCurrent)
		{
		float linePos = 0.1;
		glColorHSV(2 * M_PI / 3, 0.2, 1.0);		//big text color
		bitmapFonts.pos(0.1, linePos);
		bool tinyInfo = false;
		if(isCurrent)
			for(string::const_iterator c = aboutMsg.begin(); c < last; ++c)
				{
				bool done = false;
				do		//process all special characters
					switch(*c)
						{
						case '\n':		//new line, switch linePos to next line
							if(tinyInfo)
								linePos -= 0.05;
							else
								linePos -= 0.07;
							bitmapFonts.pos(0.1, linePos);
							++c;		//go to next character
							break;
						case '$':		//special option, read option tag
							++c;		//skip '$'
							if(*c == 't')		//switch to tiny info mode
								{
								tinyInfo = true;
								glColorHSV(M_PI / 3, 0.2, 1.0);		//tiny text color
								bitmapFonts.select(1);
								}
							++c;		//skip option tag
							break;
						default: done = true;
						}
				while(!done);
				bitmapFonts << *c;
				}
			if(cursorVisible())
				bitmapFonts << '_';		//blinking cursor
			bitmapFonts.select(0);
		}
	}

void MainMenu::AboutItem::update(bool isCurrent)
	{
	if(isCurrent)
		if(!wasCurrent)
			{		//item became current, reset the msg animation
			last = aboutMsg.begin();
			msgTimer.restart();
			}
		else
			while((msgTimer > CHAR_DELAY) && (last != aboutMsg.end()))
				{		//strecht out the visible part of about message
				msgTimer.shift(-CHAR_DELAY);		//don't reset but substract the delay time
				++last;
				}
	MenuTextItem::update(isCurrent);
	}

//----------------------------------------------------------------------------

const string MainMenu::AllHighScores::fileNameCrypted = "hscores.dat";
const string MainMenu::AllHighScores::fileNameDecrypted = "hscores.xml";

MainMenu::AllHighScores::AllHighScores(DifficultyData& iDifficulty):
	curDifficulty(iDifficulty)
	{
	if(xorFile(fileNameCrypted, fileNameDecrypted, XOR_VALUE))
		{
		highScoresKey.loadFromFile(fileNameDecrypted);
		boost::filesystem::remove(fileNameDecrypted);
		}
	MyXML::KeysConstRange range = highScoresKey.keys("difficulty");
	//iterate through all difficulty levels
	for(MyXML::KeyConstIterator i = range.first; i != range.second; ++i)
		loadDifficultyScores(i->second);		//and load high scores for particular difficulty levels
	}

MainMenu::AllHighScores::~AllHighScores()
	{
	highScoresKey.clear();		//remove all the previous high scores items
	//they're all saved in highScoresKey in constructor
	highScoresKey = "highscores";
	for(std::map<const DifficultyData, HighScores>::const_iterator i = highScores.begin();
		i != highScores.end(); ++i)		//iterate through all difficulty levels
		if(i->second.count() > 0)		//save difficulties which have at least one high score
			highScoresKey.insert("difficulty") << i->first << i->second;
	highScoresKey.saveToFile(fileNameDecrypted);
	xorFile(fileNameDecrypted, fileNameCrypted, XOR_VALUE);
	boost::filesystem::remove(fileNameDecrypted);
	}

bool MainMenu::AllHighScores::xorFile(const std::string& inputFile,
		const std::string& outputFile, unsigned char xor)
	{
	std::ofstream ofs(outputFile.c_str(), std::ios::binary);
	std::ifstream ifs(inputFile.c_str(), std::ios::binary);
	if(!ifs)
		return false;
	char c;
	for(ifs.get(c); !ifs.eof(); ifs.get(c))
		ofs << static_cast<char>(c ^ xor);
	return true;
	}

void MainMenu::AllHighScores::loadDifficultyScores(const MyXML::Key& data)
	{
	data >> highScores[DifficultyData(
		lexical_cast<int>(data.attribute("size")),
		lexical_cast<int>(data.attribute("depth")),
		lexical_cast<int>(data.attribute("blocksSet")))];
	}

//----------------------------------------------------------------------------

MainMenu::MainMenu(CuTeWindow& win, MyXML::Key& iOptions):
	MenuScene(win, difficulty), options(iOptions), difficulty(iOptions["difficulty"]),
		highScores(difficulty), controls(iOptions["controls"])
	{
	playerName = MyXML::readKeyDef(options["player"], langData["misc"]["defaultPlayerName"].value());
	const_cast<MyXML::Key&>(models).loadFromFile("data/models.xml");		//read models data from XML file

	//load fonts needed in menu and in the game itself
	win.extensions().bitmapFonts().load(langInfo["fonts"]["medium"],
		lexical_cast<int>(langInfo["fonts"]["medium"].attribute("size")) * win.width() / 1024);
	win.extensions().bitmapFonts().load(langInfo["fonts"]["small"],
		lexical_cast<int>(langInfo["fonts"]["small"].attribute("size")) * win.width() / 1024);
	win.extensions().outlineFonts().load(langInfo["fonts"]["large"],
		lexical_cast<int>(langInfo["fonts"]["large"].attribute("size")) * win.width() / 1024, 0.09);

	buildMenu();		//create all main and sub menu hierarchy
	atom.useModel(models["atomModel"]);
	menu[DIFFICULTY].subMenu()->currentIndex(difficulty.level());		//set up difficulty menu position
	}

void MainMenu::buildMenu()
	{
	MyOGL::OutlineFonts& fonts = win.extensions().outlineFonts();
	menu.addItem(new NewGameItem(win.extensions(), highScores));

	//pointer to the added item
	MenuItem* last = menu.addItem(new MenuItem(fonts, langData["mainMenu"]["difficulty"]));
	for(int d = Difficulty::EASY; d <= Difficulty::CUSTOM; ++d)
		last->addSubItem(new MenuItem(fonts, Difficulty::levelStr(d)));

	menu.addItem(new MenuItem(fonts, langData["mainMenu"]["demo"]));
	menu.addItem(new HelpItem(win.extensions(), models["helpModel"], controls));
	menu.addItem(new AboutItem(win.extensions()));
	menu.addItem(new MenuItem(fonts, langData["mainMenu"]["options"]));
	}

void MainMenu::checkInput()
	{
	MenuScene::checkInput();
	if(win.keyPressed(VK_RETURN))		//Enter pressed
		switch(menu.currentIndex())
			{
			case NEW_GAME: startNewGame(); break;
			case DEMO: startDemo(); break;
			case OPTIONS: optionsMenu(); break;
			}
	difficulty.level(menu[DIFFICULTY].subMenu()->currentIndex());
	}

void MainMenu::startNewGame()
	{
	Game game(win, difficulty, controls);
	game.start();
	if(highScores().add(playerName, game.engine().points(), game.engine().gameTime()))
		menu.currentIndex(NEW_GAME);		//go back to New Game only when player got high score
	}

void MainMenu::startDemo()
	{
	bool restart;
	do
		{
		Demo demo(win, difficulty);
		demo.start();
		restart = demo.restart();
		}
	while(restart);
	}

void MainMenu::optionsMenu()
	{
	Options options(win, options, difficulty, controls);
	options.start();
	if(options.restart())
		restart();		//if the options inform to restart, it means to restart the whole game!
	else
		//difficulty level might have changed after leaving the options menu
		menu[DIFFICULTY].subMenu()->currentIndex(difficulty.level());
	}

//----------------------------------------------------------------------------
