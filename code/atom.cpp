//----------------------------------------------------------------------------

///@file
///Atom visualization implementation.
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
#include <algorithm>
#include <functional>
#include <complex>
#include "atom.h"
using namespace CuTe;
using MyOGL::glColorHSV;

//----------------------------------------------------------------------------

const float Atom::Electron::RADIUS_MIN = 0.5;
const float Atom::Electron::RADIUS_MAX = 1.0;
const float Atom::Electron::BASE_SPEED = 100.0;
const float Atom::Electron::TAIL_LENGTH = 2.0;

Atom::Electron::Electron():
	color(static_cast<float>(rand()) / RAND_MAX * 2 * M_PI), angle(0.0),
	radius(RADIUS_MIN + (static_cast<float>(rand()) / RAND_MAX) * (RADIUS_MAX - RADIUS_MIN)),
	xEcliptic((static_cast<float>(rand()) / RAND_MAX) * 360.0),
	yEcliptic((static_cast<float>(rand()) / RAND_MAX) * 360.0), speed(BASE_SPEED / radius)
	{
	}

void Atom::Electron::update()
	{
	angle += timer.restart() / 1000.0 * speed;
	}

void Atom::Electron::draw()
	{
	update();
	glEnable(GL_BLEND);
	glPushMatrix();
	glRotatef(xEcliptic, 1.0, 0.0, 0.0);
	glRotatef(yEcliptic, 0.0, 1.0, 0.0);
	std::complex<float> pos = std::polar<float>(radius, angle * M_PI / 180.0);
	glBegin(GL_LINE_STRIP);
	for(int point = 0; point < TAIL_POINTS;
		++point, pos *= std::polar<float>(1.0, TAIL_LENGTH / TAIL_POINTS))
		{
		glColorHSV(color, 0.2, 1.0, static_cast<float>(point) / TAIL_POINTS);
		glVertex2f(pos.real(), pos.imag());
		}
	glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
	}

//----------------------------------------------------------------------------

const float Atom::ROTATION_SPEED = 25.0;

Atom::Atom(int iElectronsCount): rot(0.0), coreModelList(0), electronsCount(iElectronsCount)
	{
	electrons = new Electron[electronsCount];
	}

void Atom::draw()
	{
	update();
	glRotatef(rot, 0.2, 0.5, 0.8);
	if(coreModelList > 0)		//draw model only if useModel was used
		{
		glColorHSV(rot * M_PI / 180.0, 0.4, 1.0);
		glCallList(coreModelList);
		}
	std::for_each(electrons, electrons + electronsCount, std::mem_fun_ref(&Electron::draw));
	}

void Atom::update()
	{
	rot += timer.restart() / 1000.0 * ROTATION_SPEED;
	if(rot > 360.0)
		rot -= 360.0;
	}

//----------------------------------------------------------------------------
