//----------------------------------------------------------------------------

///@file
///CuTeWindow class definitions.
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

#include <boost/lexical_cast.hpp>
#include "scene.h"
#include "language.h"
using namespace CuTe;
using boost::lexical_cast;

//----------------------------------------------------------------------------

CuTeWindow::CuTeWindow(int iMode):
MyOGL::Window("CuTe (C) 2005-06 Tomasz Nurkiewicz",
		((iMode == W_800x600) || (iMode == F_800x600))? 800 : 1024,
		((iMode == W_800x600) || (iMode == F_800x600))? 600 : 768,
		(iMode < F_800x600)? MyOGL::WINDOWED : MyOGL::FULLSCREEN,
		MyOGL::TEXTURES | MyOGL::BITMAP_FONTS | MyOGL::FPS_COUNTER | MyOGL::OUTLINE_FONTS),
		mode_(iMode)
	{
	initGL();
	}

void CuTeWindow::initGL()
	{
	extensions().textures().load("data/tx00.dat", GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST);
	extensions().textures().load("data/tx01.dat", GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST);
	extensions().textures().load("data/tx02.dat", GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST);
	extensions().outlineFonts().useTextures(extensions().textures(), 2);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
	glFogi(GL_FOG_MODE, GL_LINEAR);
	}

//----------------------------------------------------------------------------

void CuTeScene::drawCubeWallX(double x, double y, double z, double scale)
	{
	x *= scale;
	y = (y + 0.5) * scale;
	z = (z + 0.5) * scale;
	glBegin(GL_TRIANGLE_STRIP);		//right or left wall
	glTexCoord2f(1.0, 1.0);
	glVertex3f(x, y, z);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(x, y, z - scale);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(x, y - scale, z);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(x, y - scale, z - scale);
	glEnd();
	}

void CuTeScene::drawCubeWallY(double x, double y, double z, double scale)
	{
	x = (x + 0.5) * scale;
	y *= scale;
	z = (z + 0.5) * scale;
	glBegin(GL_TRIANGLE_STRIP);		//top and bottom wall
	glTexCoord2f(1.0, 1.0);
	glVertex3f(x, y, z - scale);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(x - scale, y, z - scale);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(x, y, z);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(x - scale, y, z);
	glEnd();
	}

void CuTeScene::drawCubeWallZ(double x, double y, double z, double scale)
	{
	glBegin(GL_TRIANGLE_STRIP);		//front or back wall
	glTexCoord2f(1.0, 1.0);
	glVertex3f((x + 0.5) * scale, (y + 0.5) * scale, z * scale);
	glTexCoord2f(0.0, 1.0);
	glVertex3f((x - 0.5) * scale, (y + 0.5) * scale, z * scale);
	glTexCoord2f(1.0, 0.0);
	glVertex3f((x + 0.5) * scale, (y - 0.5) * scale, z * scale);
	glTexCoord2f(0.0, 0.0);
	glVertex3f((x - 0.5) * scale, (y - 0.5) * scale, z * scale);
	glEnd();
	}

void CuTeScene::drawCube(double x, double y, double z, double scale)
	{
	drawCubeWallX(x - 0.5, y, z, scale);
	drawCubeWallX(x + 0.5, y, z, scale);
	drawCubeWallY(x, y - 0.5, z, scale);
	drawCubeWallY(x, y + 0.5, z, scale);
	drawCubeWallZ(x, y, z - 0.5, scale);
	drawCubeWallZ(x, y, z + 0.5, scale);
	}

//----------------------------------------------------------------------------
