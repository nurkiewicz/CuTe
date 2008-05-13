//----------------------------------------------------------------------------

///@file
///Definition of Intro class.
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
///@par Definition of Intro class animation, includes all the OpenGL animation code.

//----------------------------------------------------------------------------

#define _USE_MATH_DEFINES		///<for MS VC++ compatibility (M_* are not part of the standard)
#include <cmath>
#include "intro.h"
#include "MyXML/myxml.h"
#include "common.h"
using namespace CuTe;
using MyOGL::glColorHSV;

//----------------------------------------------------------------------------

Intro::Intro(CuTeWindow& win): CuTeScene(win),
	animation(new BumpingCube(Point<double, 3>(21.0, 8.0, -8.0), 
	Point<double, 3>(-8.0, -7.0, 0.0), *this)), phase(PHASE_BUMPING), loadingScreen(false)
	{
	}

bool Intro::keyPressed()
	{
	for(int key = 0; key < 0x100; ++key)
		if(win.keyPressed(key))
			return true;
	return false;
	}

void Intro::refresh()
	{
	glLoadIdentity();
	glEnable(GL_BLEND);
	win.extensions().textures().enable();
	win.extensions().textures().select(0);
	glTranslatef(0.0, 0.0, -25.0);
	if(loadingScreen)
		{
		glRotatef(30.0, -1.2, 0.5, 0.3);
		glColorHSV(2.0, 0.8, 1.0);
		drawCube(0.0, 0.0, 0.0, 5.0);
		done();
		win.extensions().textures().disable();
		glDisable(GL_BLEND);
		return;
		}
	if(keyPressed())
		loadingScreen = true;
	else
		{
		glRotatef(20.0, 0.3, 0.2, 0.6);
		animation->draw();
		}
	if(animation->done())
		{
		delete animation;
		switch(phase++)
			{
			case PHASE_BUMPING: animation = new LogoBlending(*this, cubes); break;
			case PHASE_ALPHA: animation = new LogoShaking(*this, cubes); break;
			case PHASE_SHAKING: animation = new LogoExploding(*this, cubes); break;
			case PHASE_EXPLODING: loadingScreen = true; break;		//finish the scene
			}
		}
	win.extensions().textures().disable();
	glDisable(GL_BLEND);
	}

//----------------------------------------------------------------------------

Intro::Animation::~Animation()	{}

//----------------------------------------------------------------------------

const double Intro::BumpingCube::GRAVITY = 3.8e-5;

Intro::BumpingCube::BumpingCube(const Point<double, 3>& startPos, const Point<double, 3>& endPos, const CuTeScene& iScene):
		Animation(scene, BUMPING_TIME), pos(startPos), speed((endPos - startPos) / BUMPING_TIME), 
		rotAxis(speed.z(), -speed.x()), BOTTOM(endPos.y()), lastTime(0)
	{
	}

void Intro::BumpingCube::draw()
	{
	if(timer <= BUMPING_TIME)
		{
		const int TAU = timer - lastTime;		//time which have elapsed since last refresh()
		lastTime = timer;
		speed.y() -= GRAVITY * TAU;
		pos += speed * TAU;
		if(pos.y() < BOTTOM)
			{
			speed.y() *= -0.8;
			pos.y() = 2 * BOTTOM - pos.y();
			}
		}
	glColorHSV(0.0, 0.6, 1.0);
	glTranslatef(pos.x(), pos.y(), pos.z());
	glRotatef(timer * 360.0 / BUMPING_TIME, rotAxis.x(), 0.0, rotAxis.y());
	scene.drawCube(0.0, 0.0, 0.0);
	}

//----------------------------------------------------------------------------

const double Intro::Cube::ALPHA_SPEED = 0.001;

Intro::Cube::Cube(const MyXML::Key& data)
	{
	pos.x() = boost::lexical_cast<int>(data.attribute("x"));
	pos.y() = boost::lexical_cast<int>(data.attribute("y"));
	pos.z() = static_cast<float>(rand()) / RAND_MAX / 2.0 - 0.25;
	initTime = boost::lexical_cast<int>(data.attribute("time"));
	}

//----------------------------------------------------------------------------

Intro::LogoCubes::LogoCubes()
	{
	const MyXML::Key cubesData("data/intro.xml");		//load cubes positions from file
	MyXML::KeysConstRange cubesRange = cubesData.keys("cube");
	for(MyXML::KeyConstIterator i = cubesRange.first; i != cubesRange.second; ++i)
		push_back(Cube(i->second));
	std::sort(begin(), end());
	}

//----------------------------------------------------------------------------

void Intro::LogoBlending::draw()
	{
	for(LogoCubes::const_iterator i = cubes.begin(); i != cubes.end(); ++i)
		if(timer >= i->initTime)
			{
			glColorHSV((timer - i->initTime) * Cube::ALPHA_SPEED, 0.6, 1.0, (timer - i->initTime) * Cube::ALPHA_SPEED);
			scene.drawCube(i->pos.x(), i->pos.y(), i->pos.z());
			}
	}

//----------------------------------------------------------------------------

const double Intro::LogoShaking::SHAKING_FREQ = 5.0;

void Intro::LogoShaking::draw()
	{
	glTranslatef(0.0, 0.0, sqr(timer / 10000.0) * sin(2 * M_PI * SHAKING_FREQ * timer / 1000.0));
	for(LogoCubes::const_iterator i = cubes.begin(); i != cubes.end(); ++i)
		{
		glColorHSV((timer - i->initTime) * Cube::ALPHA_SPEED, 0.6, 1.0);
		scene.drawCube(i->pos);
		}
	}

//----------------------------------------------------------------------------

const double Intro::LogoExploding::EXPLODING_SPEED = 0.0007;
const double Intro::LogoExploding::ROTATION_SPEED = 60.0;

void Intro::LogoExploding::draw()
	{
	glRotatef(ROTATION_SPEED / 1000.0 * timer, -2.0, 1.0, 0.0);
	for(LogoCubes::iterator i = cubes.begin(); i != cubes.end(); ++i)
		{
		i->pos *= 1.0 + EXPLODING_SPEED * (timer - lastTimer);
		glColorHSV((timer - i->initTime) * Cube::ALPHA_SPEED, 0.6, 1.0);
		scene.drawCube(i->pos.x(), i->pos.y(), i->pos.z());
		}
	lastTimer = timer;
	}

//----------------------------------------------------------------------------
