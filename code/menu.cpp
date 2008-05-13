//----------------------------------------------------------------------------

///@file
///Definitions of menu classes declared in menu.h
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
#include <cmath>
#include "menu.h"
#include "sounds.h"
using namespace CuTe;
using MyOGL::glColorHSV;

//----------------------------------------------------------------------------

const float Menu::COLOR_VALUE_MIN = 0.3;
const float Menu::COLOR_VALUE_MAX = 1.0;

Menu::Menu(): current_(0), previous_(0), angleShift(0.0)
	{
	}

void Menu::drawHorizontally(float scale)
	{
	if(scale < 0.05)
		return;		//don't draw menu if it is too little
	update();
	glFogf(GL_FOG_START, 12.3);
	glFogf(GL_FOG_END, 16.0);
	float angle = angleShift;		//initial items angle
	int pos = current_;
	do
		{
		glLoadIdentity();
		glTranslatef(3.2, -3.7, -14.0);
		glRotatef(10.0, 1.0, 0.0, 0.0);
		glRotatef(angle - 15.0, 0.0, 1.0, 0.0);
		glScalef(scale, scale, scale);
		glTranslatef(0.0, 0.0, 1.8);
		glRotatef(-angle, 0.0, 1.0, 0.0);
		glColorHSV(angle * M_PI / 180.0, 0.9, itemColorValue(pos));
		items[pos]->draw(false);
		angle += ITEMS_ANGLE;
		if(++pos == static_cast<int>(items.size()))
			pos = 0;
		}
	while(pos != current_);
	}

void Menu::drawVertically()
	{
	update();
	glEnable(GL_FOG);
	float angle = angleShift;		//initial items angle
	int pos = current_;
	do
		{
		glFogf(GL_FOG_START, 7.0);
		glFogf(GL_FOG_END, 13.0);
		glLoadIdentity();
		glTranslatef(-2.5, -0.3, -10.0);
		glRotatef(25.0, 0.0, 1.0, 0.0);
		glRotatef(angle, 1.0, 0.0, 0.0);
		glTranslatef(0.0, 0.0, 3.0);
		glRotatef(-angle, 1.0, 0.0, 0.0);
		glColorHSV(angle * M_PI / 180.0, 0.95, itemColorValue(pos));
		items[pos]->draw(pos == current_);
		angle += ITEMS_ANGLE;
		if(++pos == static_cast<int>(items.size()))
			pos = 0;
		}
	while(pos != current_);
	glDisable(GL_FOG);
	}

MenuItem* Menu::addItem(MenuItem *item)
	{
	items.push_back(item);
	ITEMS_ANGLE = 360.0 / items.size();
	return item;
	}

void Menu::update()
	{
	decAbs<float>(angleShift, static_cast<float>(timer.restart()) / ITEMS_PERIOD * ITEMS_ANGLE);
	}

void Menu::previous()
	{
	if(angleShift >= 0.0)
		{
		previous_ = current_;
		if(--current_ < 0)
			current_ = items.size() - 1;
		angleShift -= ITEMS_ANGLE;
		sounds.play(Sounds::ROTATE);
		}
	}

void Menu::next()
	{
	if(angleShift <= 0.0)
		{
		previous_ = current_;
		if(++current_ == static_cast<int>(items.size()))
			current_ = 0;
		angleShift += ITEMS_ANGLE;
		sounds.play(Sounds::ROTATE);
		}
	}

float Menu::itemColorValue(int pos)
	{
	if(pos == current_)
		return COLOR_VALUE_MIN + (1.0f - std::abs(angleShift) / ITEMS_ANGLE) * (COLOR_VALUE_MAX - COLOR_VALUE_MIN);
	if(pos == previous_)
		return COLOR_VALUE_MIN + (std::abs(angleShift) / ITEMS_ANGLE) * (COLOR_VALUE_MAX - COLOR_VALUE_MIN);
	return COLOR_VALUE_MIN;
	}

//----------------------------------------------------------------------------

const float MenuItem::SCALE_CHANGE_SPEED = 4.0;

MenuItem::MenuItem(MyOGL::OutlineFonts& iOutlineFonts, const std::string &iLabel):
	label(iLabel), labelWidth(iOutlineFonts.width(iLabel)),
	shiftSpeed((rand() % 1024) / 200000.0, (rand() % 1024) / 200000.0, (rand() % 1024) / 200000.0), subMenu_(NULL),
	scale(0.0), outlineFonts(iOutlineFonts), wasCurrent(false)
	{
	}

void MenuItem::draw(bool isCurrent)
	{
	glTranslatef(-labelWidth / 2, 0.0, 0.0);
	glTranslatef(0.08 * sin(shiftSpeed.x() * chaosTimer),		//move text a bit creating chaotic movement
		0.08 * sin(shiftSpeed.y() * chaosTimer),
		0.08 * sin(shiftSpeed.z() * chaosTimer));
	outlineFonts << label;

	update(isCurrent);
	if(hasSubMenu())		//sub menu display routines
		{
		if(isCurrent)
			subMenu_->drawHorizontally(1.0 - scale);
		else
			subMenu_->drawHorizontally(scale);
		}
	}

void MenuItem::addSubItem(MenuItem* item)
	{
	if(!hasSubMenu())
		subMenu_ = new Menu;
	subMenu_->addItem(item);
	}

void MenuItem::update(bool isCurrent)
	{
	if(isCurrent != wasCurrent) //item is current but it wasn't or vice versa
		{
		scale = 1.0;
		wasCurrent = isCurrent;
		}
	decAbs<float>(scale, scaleTimer.restart() / 1000.0 * SCALE_CHANGE_SPEED);
	}

//----------------------------------------------------------------------------

MenuTextItem::MenuTextItem(MyOGL::Extensions& extensions, const std::string& label):
	MenuItem(extensions.outlineFonts(), label),
	cursorVisible_(false), bitmapFonts(extensions.bitmapFonts())
	{
	}

void MenuTextItem::update(bool isCurrent)
	{
	if(isCurrent && (cursorTimer > CURSOR_BLINK_RATE))
		{
		cursorTimer.restart();
		cursorVisible_ = !cursorVisible_;
		}
	MenuItem::update(isCurrent);
	}

//----------------------------------------------------------------------------

Atom MenuScene::atom(32);

MenuScene::Background<8, 20> MenuScene::background(0.3, 22.0);

std::string MenuScene::playerName;

MenuScene::MenuScene(CuTeWindow& iWin, Difficulty& iDifficulty):
	CuTeScene(iWin), difficulty(iDifficulty)
	{
	}

void MenuScene::refresh()
	{
	checkInput();
	background.draw(win.extensions().textures());
	menu.drawVertically();
	glLoadIdentity();
	glTranslatef(1.7, 1.3, -6.0);
	win.extensions().textures().select(0);
	atom.draw();
	drawInfo();
	}

void MenuScene::checkInput()
	{
	if(win.keyDown(VK_UP))
		menu.previous();
	if(win.keyDown(VK_DOWN))
		menu.next();
	if((win.keyDown(VK_RIGHT)) && menu.current().hasSubMenu())
		menu.current().subMenu()->next();		//try to move in menu
	if(win.keyDown(VK_LEFT) && menu.current().hasSubMenu())
		menu.current().subMenu()->previous();		//try to move in menu
	if(win.keyPressed(VK_ESCAPE))
		done();
	}

void MenuScene::drawInfo()
	{
	glColorHSV(5 * M_PI / 6, 0.2, 0.4);
	win.extensions().bitmapFonts().pos(-1.08, -1.09) << difficulty;

	const std::string msg = '\'' + playerName + "' @ " + difficulty.levelName();
	win.extensions().bitmapFonts().pos(1.08 - msg.length() * 0.0215, -1.09) << msg;
	}

//----------------------------------------------------------------------------

template<int DETAILS, int WIDTH>
const float MenuScene::Background<DETAILS, WIDTH>::UNIT = 1.0 / DETAILS;

template<int DETAILS, int WIDTH>
MenuScene::Background<DETAILS, WIDTH>::Background(float iBrightness, float iZDist):
	brightness(iBrightness), omegaX((rand() % 1024) / 409600.0),
	omegaY((rand() % 1024) / 409600.0), zDist(-iZDist)
	{
	}

template<int DETAILS, int WIDTH>
void MenuScene::Background<DETAILS, WIDTH>::draw(MyOGL::Textures& textures)
	{
	update();		//update background data before rendering
	textures.enable();
	textures.select(1);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, zDist);
	glColorHSV(0.0, 0.0, brightness);
	glBegin(GL_QUADS);
	Point<float, 2> texCoords;
	Point<float, 2> quadCoords;
	for(int x = 0; x < DETAILS; ++x)
		for(int y = 0; y < DETAILS; ++y)
			{
			texCoords.x() = x * UNIT;
			texCoords.y() = y * UNIT;
			quadCoords.x() = WIDTH * (texCoords.x() - 0.5);
			quadCoords.y() = WIDTH * (texCoords.y() - 0.5);
			glTexCoord2f(texCoords.x(), texCoords.y());
			glVertex3f(quadCoords.x(), quadCoords.y(), heights[x][y]);
			glTexCoord2f(texCoords.x() + UNIT, texCoords.y());
			glVertex3f(quadCoords.x() + WIDTH * UNIT, quadCoords.y(), heights[x + 1][y]);
			glTexCoord2f(texCoords.x() + UNIT, texCoords.y() + UNIT);
			glVertex3f(quadCoords.x() + WIDTH * UNIT, quadCoords.y() + WIDTH * UNIT, heights[x + 1][y + 1]);
			glTexCoord2f(texCoords.x(), texCoords.y() + UNIT);
			glVertex3f(quadCoords.x(), quadCoords.y() + WIDTH * UNIT, heights[x][y + 1]);
			}
	glEnd();
	textures.disable();
	}

template<int DETAILS, int WIDTH>
void MenuScene::Background<DETAILS, WIDTH>::update()
	{
	ampl.x() = DETAILS / 2.0 + (DETAILS / 3.0) * sin(omegaX * timer);
	ampl.y() = DETAILS / 2.0 + (DETAILS / 3.0) * sin(omegaY * timer);
	for(int x = 0; x < DETAILS + 1; ++x)
		for(int y = 0; y < DETAILS + 1; ++y)
			heights[x][y] = WIDTH / 4.0 * exp(-UNIT * (sqr(x - ampl.x()) + sqr(y - ampl.y())));
	}

//----------------------------------------------------------------------------
