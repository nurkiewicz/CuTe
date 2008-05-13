//----------------------------------------------------------------------------

///@file
///Options menu classes definitions.
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
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include "language.h"
#include "scene.h"
#include "difficulty.h"
#include "optionsmenu.h"
#include "sounds.h"
using namespace CuTe;
using std::string;
using MyOGL::glColorHSV;
using boost::lexical_cast;
using boost::tokenizer;

//----------------------------------------------------------------------------

Options::ControlsChange::ControlsChange(CuTeWindow& win, Game::Controls& controls,
	Difficulty& difficulty):
		MenuScene(win, difficulty), origControls(controls), newControls(controls), isChanging(false),
			lastConflictAction(-1), cursorVisible(false)
	{
	buildMenu();
	}

void Options::ControlsChange::buildMenu()
	{
	for(int x = 0; x < Game::Controls::allActions(); ++x)
		menu.addItem(new MenuItem(win.extensions().outlineFonts(), Game::Controls::actionStrs(x).first));
	}

void Options::ControlsChange::refresh()
	{
	MenuScene::refresh();
	MyOGL::BitmapFonts& fonts = win.extensions().bitmapFonts();
	const MyXML::Key& keybMenu = langData["keyboardMenu"];
	glColorHSV(2.5, 1.0, 0.7);
	fonts.pos(0.3, -0.1) << keybMenu["current"].value();		//current key label
	if(isChanging)
		{
		if(cursorTimer >= CURSOR_BLINK_RATE)		//update the blinking cursor flag
			{
			cursorTimer.restart();
			cursorVisible = !cursorVisible;
			}
		if(cursorVisible)
			fonts << '_';
		glColorHSV(4.5, 0.8, 0.8);
		fonts.pos(0.3, 0.0) << keybMenu["typeNewKey"].value();
		if(lastConflictAction >= 0)
			{
			glColorHSV(0.0, 1.0, 0.8);
			fonts.pos(0.3, -0.3) << Game::Controls::keyCodeToStr(lastPressedKey) << ' ' << 
				keybMenu["conflict"].value();
			fonts.pos(0.38, -0.38) << '"' << Game::Controls::actionStrs(lastConflictAction).first << '"';
			}
		}
	else
		{
		fonts << Game::Controls::keyCodeToStr(newControls(menu.currentIndex()));
		showDesc(menu.currentIndex());		//currentIndex() is action code at the same time
		}
	//display only when original controls differs from the one choosen by the user or during changing
	if(isChanging || (newControls(menu.currentIndex()) != origControls(menu.currentIndex())))
		{
		glColorHSV(2.5, 0.4, 0.4);
		fonts.pos(0.3, -0.2) << keybMenu["previous"].value() <<
			Game::Controls::keyCodeToStr(origControls(menu.currentIndex()));
		}
	}

void Options::ControlsChange::showDesc(int action)
	{
	MyOGL::BitmapFonts& fonts = win.extensions().bitmapFonts();
	const string desc = Game::Controls::actionStrs(action).second;
	typedef tokenizer<boost::char_separator<char> > Tokens;
	glColorHSV(3.6, 0.5, 0.7);
	int len = 0;		//length of the currently output line
	float yPos = -0.3;
	fonts.pos(0.3, yPos);
	Tokens words(desc, boost::char_separator<char>(" "));
	for(Tokens::const_iterator word = words.begin(); word != words.end(); ++word)
		{
		if((len += word->length() + 1) > DESC_LINE_MAX_LENGTH)
			{		//current line is too long, go to the next one
			fonts.pos(0.3, yPos -= 0.08);
			len = word->length() + 1;
			}
		fonts << *word << ' ';
		}
	}

void Options::ControlsChange::checkInput()
	{
	if(isChanging)
		for(int control = VK_BACK; control <= VK_F24; ++control)
			if(win.keyPressed(control))		//check successively whther any key was pressed
				{
				lastPressedKey = control;
				switch(control)
					{
					case VK_ESCAPE:		//abort changing the key
						isChanging = false;
						lastConflictAction = -1;
						break;		//quit from changing
					default:
						const int oldAction = newControls.controlAction(control);
						if((oldAction < 0) || (oldAction == menu.currentIndex()))
							{		//no action is associated to pressed key or user pressed the same key as it was
							newControls(menu.currentIndex()) = control;		//set the new control
							isChanging = false;
							lastConflictAction = -1;
							}
						else
							lastConflictAction = oldAction;
					}
				break;
				}
			else;
	else
		{
		if(win.keyPressed(VK_RETURN))
			isChanging = true;
		MenuScene::checkInput();		//non-changing mode, simply check the menu input
		}
	}

//----------------------------------------------------------------------------

const float Options::PlayerNameItem::NAME_EDIT_ALPHA_SPEED = 1.5;

Options::PlayerNameItem::PlayerNameItem(MyOGL::Window& iWin, std::string& iPlayerName):
	win(iWin), MenuTextItem(iWin.extensions(), langData["optionsMenu"]["playerName"]),
		playerName(iPlayerName), alpha(0.0)
	{
	}

void Options::PlayerNameItem::draw(bool isCurrent)
	{
	MenuItem::draw(isCurrent);
	glDisable(GL_FOG);
	glEnable(GL_BLEND);
	if(isCurrent || (alpha > 0.0))
		{
		glColorHSV(4 * M_PI / 3, 0.8, 0.6, isCurrent? (1.0 - alpha) : alpha);
		glLoadIdentity();
		glTranslatef(3.2, -3.5, -16.0);
		glRotatef(-20.0, 0.0, 1.0, 0.0);
		glRotatef(35.0, 0.0, 0.0, 1.0);
		glTranslatef(-outlineFonts.width(playerName) / 2, 0.0, 0.0);
		outlineFonts << playerName;
		if(cursorVisible())
			outlineFonts << '_';
		}
	glDisable(GL_BLEND);
	glEnable(GL_FOG);
	}

void Options::PlayerNameItem::update(bool isCurrent)
	{
	if(isCurrent != wasCurrent)
		{
		alpha = 1.0;
		alphaTimer.restart();
		if(wasCurrent)
			{		//if item lost focus, check whether player typed valid name (non-empty)
			boost::trim(playerName);
			if(playerName.empty())
				playerName = langData["misc"]["defaultPlayerName"];
			}
		}
	MenuTextItem::update(isCurrent);
	if(isCurrent || (alpha > 0.0))
		{
		if(isCurrent)
			checkInput();		//check for letters, digits, etc. or [Backspace]
		decAbs<float>(alpha, alphaTimer.restart() / 1000.0 * NAME_EDIT_ALPHA_SPEED);
		}
	}

void Options::PlayerNameItem::addChar(char c)
	{
	if(playerName.size() < HighScore::PLAYER_NAME_MAX_LENGTH)
		playerName += c;
	}

void Options::PlayerNameItem::checkInput()
	{
	if(win.keyPressed(VK_BACK) && !playerName.empty())
		playerName.erase(playerName.size() - 1, 1);
	char c;
	if(win.keyPressed(' '))
		addChar(' ');		//space
	for(c = 'A'; c <= 'Z'; ++c)		//letters
		if(win.keyPressed(c))
			if(win.keyDown(VK_SHIFT))		//check whether Shift was pressed (capital or normal letters)
				addChar(c);
			else
				addChar(tolower(c));
	for(c = '0'; c <= '9'; ++c)
		if(win.keyPressed(c))
			addChar(c);
	}

//----------------------------------------------------------------------------

Options::VideoItem::VideoItem(CuTeWindow& parent):
	MenuTextItem(parent.extensions(), langData["optionsMenu"]["video"]), curMode(parent.mode())
	{
	MyXML::KeysRange items = langData["optionsMenu"].keys("videoSubItem");
	for(MyXML::KeyIterator i = items.first; i != items.second; ++i)
		addSubItem(new MenuItem(parent.extensions().outlineFonts(), i->second));
	}

void Options::VideoItem::draw(bool isCurrent)
	{
	MenuItem::draw(isCurrent);
	if(isCurrent)
		{
		int mode = subMenu()->currentIndex();		//which video mode is selected
		glColorHSV(M_PI / 3, 1.0, 0.7);
		bitmapFonts.pos(0.2, -0.3);
		if((mode == CuTeWindow::W_800x600) || (mode == CuTeWindow::F_800x600))
			bitmapFonts << "800x600  ";
		else
			bitmapFonts << "1024x768 ";
		if(mode < CuTeWindow::F_800x600)
			bitmapFonts << langData["optionsMenu"]["videoWindow"].value();
		else
			bitmapFonts << langData["optionsMenu"]["videoFullscreen"].value();
		if(mode != curMode)
			{
			glColorHSV(4 * M_PI / 3, 1.0, 0.5);
			bitmapFonts.pos(0.1, -0.4) << langData["misc"]["restart"].value();
			}
		}
	}

//----------------------------------------------------------------------------

Options::LanguageItem::LangFileInfo::LangFileInfo(const filesys::path& iFilePath):
	filePath(iFilePath)
	{
	const MyXML::Key langFile(filePath.string());
	const MyXML::Key& aboutKey = langFile["info"];
	nameEnglish = aboutKey["name"]["english"];
	nameNative = aboutKey["name"]["native"];
	author = aboutKey["about"]["author"];
	email = aboutKey["about"]["email"];
	version = aboutKey["about"]["version"];
	date = aboutKey["about"]["date"];
	}

//----------------------------------------------------------------------------

Options::LanguageItem::LanguageItem(MyOGL::Extensions& extensions, MyXML::Key& iLangKey, bool& iNeedRestart):
	MenuTextItem(extensions, langData["optionsMenu"]["language"]), langKey(iLangKey),
		needRestart(iNeedRestart)
	{
	const filesys::directory_iterator end;
	//loop through all the files in lang subdurectory
	for(filesys::directory_iterator langFile("lang"); langFile != end; ++langFile)
		addLangItem(*langFile);
	}

void Options::LanguageItem::addLangItem(const filesys::path& filePath)
	{
	langFiles.push_back(LangFileInfo(filePath));
	addSubItem(new MenuItem(outlineFonts, langFiles.back().nameNative));
	if(filePath.leaf() == langKey.value())
		{		//set the language sub menu current item to the proper language
		curLang = langFiles.size() - 1;
		subMenu()->currentIndex(curLang);
		}
	}

void Options::LanguageItem::draw(bool isCurrent)
	{
	MenuTextItem::draw(isCurrent);
	if(isCurrent)
		{
		const LangFileInfo langFile = langFiles[subMenu()->currentIndex()];
		glColorHSV(3.0, 0.7, 0.8);
		bitmapFonts.pos(0.2, 0.05) << langFile.nameNative << " (" << langFile.nameEnglish << ", '" <<
			langFile.filePath.string() << "')";
		glColorHSV(0.0, 0.0, 0.6);
		bitmapFonts.pos(0.1, -0.05) << langInfo["about"]["translationL"].value();
		bitmapFonts.pos(0.1, -0.15) << langInfo["about"]["emailL"].value();
		bitmapFonts.pos(0.1, -0.25) << langInfo["about"]["versionL"].value();
		glColorHSV(2.0, 1.0, 0.6);
		bitmapFonts.pos(0.41, -0.05) << langFile.author;
		bitmapFonts.pos(0.41, -0.15) << langFile.email;
		bitmapFonts.pos(0.41, -0.25) << langFile.version << ", " << langFile.date;
		//set the needRestart flag and display the restart info if it was set
		if(needRestart = (curLang != subMenu()->currentIndex()))
			{
			glColorHSV(4 * M_PI / 3, 1.0, 0.5);
			bitmapFonts.pos(0.2, -0.4) << langData["misc"]["restart"].value();
			}
		}
	}

//----------------------------------------------------------------------------

Options::Options(CuTeWindow& win, MyXML::Key& iUser, Difficulty& difficulty,
	Game::Controls& iControls):
		MenuScene(win, difficulty), user(iUser), needRestart(false), controls(iControls)
	{
	buildMenu();
	menu[VIDEO].subMenu()->currentIndex(win.mode());
	menu[CUBOID_SIZE].subMenu()->currentIndex(difficulty.size() - Difficulty::SIZE_MIN);
	menu[CUBOID_DEPTH].subMenu()->currentIndex((difficulty.depth() - Difficulty::DEPTH_MIN) / 2);
	menu[BLOCKS_SET].subMenu()->currentIndex(difficulty.blocksSet());
	menu[SOUNDS].subMenu()->currentIndex((iUser["sounds"].value() == "1")? 0 : 1);
	}

Options::~Options()
	{
	user["player"] = playerName;		//save some data changed in menu
	user["video"] = menu[VIDEO].subMenu()->currentIndex();
	user["sounds"] = (menu[SOUNDS].subMenu()->currentIndex() + 1) % 2;
	}

void Options::buildMenu()
	{
	MyOGL::OutlineFonts& fonts = win.extensions().outlineFonts();
	menu.addItem(new PlayerNameItem(win, playerName));
	MenuItem* last;		//pointer to the last added item (only if it will have sub menu)
	last = menu.addItem(new MenuItem(fonts, langData["optionsMenu"]["cuboidSize"]));
	for(int size = Difficulty::SIZE_MIN; size <= Difficulty::SIZE_MAXX; ++size)
		last->addSubItem(new MenuItem(fonts, lexical_cast<string>(size)));

	last = menu.addItem(new MenuItem(fonts, langData["optionsMenu"]["cuboidDepth"]));
	for(int depth = Difficulty::DEPTH_MIN; depth <= Difficulty::DEPTH_MAX; depth += 2)
		last->addSubItem(new MenuItem(fonts, lexical_cast<std::string>(depth)));

	last = menu.addItem(new MenuItem(fonts, langData["optionsMenu"]["blocksSet"]));
	for(int bset = 0; bset < 3; ++bset)
		last->addSubItem(new MenuItem(fonts, Difficulty::blocksSetStr(bset)));

	menu.addItem(new VideoItem(win));		//sub items are created in constructor automatically
	menu.addItem(new LanguageItem(win.extensions(), user["language"], needRestart));
	menu.addItem(new MenuItem(fonts, langData["optionsMenu"]["keyboard"]));

	last = menu.addItem(new MenuItem(fonts, langData["optionsMenu"]["sounds"]));
	last->addSubItem(new MenuItem(fonts, langData["sounds"]["on"]));
	last->addSubItem(new MenuItem(fonts, langData["sounds"]["off"]));
	}

void Options::checkInput()
	{
	MenuScene::checkInput();
	//update the difficulty info depending on the current menu positions
	difficulty.size(menu[CUBOID_SIZE].subMenu()->currentIndex() + Difficulty::SIZE_MIN);
	difficulty.depth(menu[CUBOID_DEPTH].subMenu()->currentIndex() * 2 + Difficulty::DEPTH_MIN);
	difficulty.blocksSet(menu[BLOCKS_SET].subMenu()->currentIndex());
	sounds.enable(menu[SOUNDS].subMenu()->currentIndex() == 0);
	//Enter pressed, video menu is current and the user changed the video mode
	if(win.keyPressed(VK_RETURN))
		switch(menu.currentIndex())
			{
			case VIDEO:
				if(win.mode() != menu[VIDEO].subMenu()->currentIndex())
					restart();		//This indicates to restart the whole game, not the options menu scene!
				break;
			case LANGUAGE: if(needRestart) restart();	break;
			case KEYBOARD:
				ControlsChange controlsChange(win, controls, difficulty);
				controlsChange.start();
				break;
			}
	}

//----------------------------------------------------------------------------
