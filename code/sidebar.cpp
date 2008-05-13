//----------------------------------------------------------------------------

///@file
///CuTe main game sidebar view classes definitions.
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
#include <boost/lexical_cast.hpp>
#include "language.h"
#include "sidebar.h"
using namespace CuTe;
using namespace std;
using MyOGL::glColorHSV;
using boost::lexical_cast;

//----------------------------------------------------------------------------

const float SideBar::FloatingBar::WIDTH = 0.1;
const float SideBar::FloatingBar::HEIGHT = 0.7;
const float SideBar::FloatingBar::MAX_POS = 1.0;

SideBar::FloatingBar::FloatingBar(float iBarPosX, float iBarPosY, float iBarAngleZ, float iHue, float iSpeed):
	speed(iSpeed), hue(iHue),
	barPosX(iBarPosX), barPosY(iBarPosY), barAngleZ(iBarAngleZ), pos(0.0), shift(0.0)
	{
	}

void SideBar::FloatingBar::set(float newPos)
	{
	float old = pos;
	if(newPos < 0.0)
		pos = 0.0;
	else
		if(newPos > MAX_POS)
			pos = MAX_POS;
		else		//newPos fits <0; maxPos> range
			pos = newPos;
	shift += old - pos;
	}

void SideBar::FloatingBar::draw()
	{
	update();		//update data before drawing
	glPushMatrix();
	glTranslatef(barPosX, barPosY, 0.0);
	glRotatef(barAngleZ, 0.0, 0.0, 1.0);
	glBegin(GL_LINE_LOOP);
	glColorHSV(hue, 1.0, 0.6);
	glVertex2f(-WIDTH / 2, -HEIGHT / 2);
	glVertex2f(WIDTH / 2, -HEIGHT / 2);
	glColorHSV(hue, 1.0, 0.12);
	glVertex2f(WIDTH / 2, HEIGHT / 2);
	glVertex2f(-WIDTH / 2, HEIGHT / 2);
	glEnd();
	glBegin(GL_QUADS);
	glColorHSV(hue, 0.8, 0.8);
	float height = -HEIGHT / 2 + 0.02;
	glVertex2f(-WIDTH / 2 + 0.02, height);
	glVertex2f(WIDTH / 2 - 0.02, height);
	glColorHSV(hue, 0.8, 0.2);
	height += 0.01 + (HEIGHT - 0.04) * (pos + shift) / MAX_POS;
	glVertex2f(WIDTH / 2 - 0.02, height);
	glVertex2f(-WIDTH / 2 + 0.02, height);
	glEnd();
	glPopMatrix();
	}

//----------------------------------------------------------------------------

const float SideBar::ScoreDisplay::DISPLAY_CHANGE_ACCELERATION = 0.5;

SideBar::ScoreDisplay::ScoreDisplay(MyOGL::OutlineFonts &iOutlineFonts):
	outlineFonts(iOutlineFonts), oldPoints(0), pointsShift(0.0)
	{
	}

void SideBar::ScoreDisplay::draw(int points)
	{
	update(points);
	glColorHSV(2 * M_PI / 3, 1.0, 0.3);
	glTranslatef(-0.45, -0.2, 0.0);
	glRotatef(10.0, 0.0, 0.0, 1.0);
	glScalef(0.2, 0.2, 0.2);
	outlineFonts << intToFmtStr(static_cast<int>(ceil(points - pointsShift)));
	}

void SideBar::ScoreDisplay::update(int points)
	{
	if(points != oldPoints)
		{
		if(pointsShift == 0.0)
			timer.restart();		//restart only when change wasn't in progress
		pointsShift += points - oldPoints;
		oldPoints = points;
		}
	if(pointsShift != 0.0)
		decAbs<float>(pointsShift, timer * timer / 1000000.0 * DISPLAY_CHANGE_ACCELERATION);
	}

//----------------------------------------------------------------------------

SideBar::SideBar(const Difficulty& difficulty, MyOGL::Extensions& iExtensions):
	distBar(0.04, -0.6, -10.0, 0.0, 1.0),
	forwardMoveBar(0.2, -0.55, -10.0, 2 * M_PI / 3, 4.0),
	speedTimeBar(0.36, -0.5, -10.0, 4 * M_PI / 3, 0.3), extensions(iExtensions), scoreDisplay(extensions.outlineFonts()),
	sizeStr(lexical_cast<std::string>(difficulty.size()) + 'x' + lexical_cast<std::string>(difficulty.size()) + 'x' +
		lexical_cast<std::string>(difficulty.depth()))
	{
	}

void SideBar::drawBackground()
	{
	extensions.textures().enable();
	extensions.textures().select(1);
	glColorHSV(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);
	glVertex2f(-0.5, 0.0);
	glTexCoord2i(1, 0);
	glVertex2f(0.5, 0.0);
	glTexCoord2i(1, 1);
	glVertex2f(0.5, 1.0);
	glTexCoord2i(0, 1);
	glVertex2f(-0.5, 1.0);
	glEnd();
	extensions.textures().disable();
	}

void SideBar::draw(const GameInfo &info)
	{
	drawBackground();		//CuTe logo
	distBar.draw(info.dist);		//3 display bars
	forwardMoveBar.draw(info.forwardMoveTime);
	speedTimeBar.draw(info.speedChangeTime);
	scoreDisplay.draw(info.points);
	showOtherData(info.speed, info.gameTime);
	}

void SideBar::showOtherData(int speed, int gameTime)
	{
	glColorHSV(M_PI / 4, 1.0, 0.6);		//game time
	extensions.bitmapFonts().pos(-0.48, -0.55) << timeToFmtStr(gameTime);
	glColorHSV(3 * M_PI / 4, 1.0, 0.6);		//game speed level
	extensions.bitmapFonts().pos(-0.45, -0.7) << langData["inGame"]["speed"].value() << speed;
	glColorHSV(5 * M_PI / 4, 1.0, 0.6);
	extensions.bitmapFonts().pos(-0.42, -0.85) << sizeStr;		//cuboid size and depth info
	glColorHSV(7 * M_PI / 4, 1.0, 0.5);		//FPS counter
	extensions.bitmapFonts().pos(-0.39, -1.0) << "FPS: " << static_cast<int>(extensions.fpsCounter());
	}

//----------------------------------------------------------------------------
