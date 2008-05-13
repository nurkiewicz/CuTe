//----------------------------------------------------------------------------

///@file
///Definitions of functions from xmlglcmd.h
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

#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include "xmlglcmd.h"
using namespace CuTe;
using MyOGL::glColorHSV;
using boost::lexical_cast;

//----------------------------------------------------------------------------

///Converts OpenGL enumarator string to its numeric value.
///Converts a string like "QUADS" into GLenum type with a value "GL_QUADS".
///@param command XML key containing the enumerator as its value.
///@return GLenum with the numeric value of the given enumerator.
GLenum toGLenum(const MyXML::Key& command)
	{
	if(command.value() == "TEXTURE_2D")
		return GL_TEXTURE_2D;
	if(command.value() == "LINE_LOOP")
		return GL_LINE_LOOP;
	if(command.value() == "LINES")
		return GL_LINES;
	if(command.value() == "QUADS")
		return GL_QUADS;
	throw std::invalid_argument('\"' + command.value() + "' is not a valid OpenGL enumerator");
	}

//----------------------------------------------------------------------------

void CuTe::executeGLCommand(const MyXML::Key& command)
	{
	const std::string id = command.attribute("id");
	if(id == "vertex")
		{
		glVertex3f(lexical_cast<float>(command["coords"].attribute("x")),
			lexical_cast<float>(command["coords"].attribute("y")),
			lexical_cast<float>(command["coords"].attribute("z")));
		return;
		}
	if(id == "texcoords")
		{
		glTexCoord2f(lexical_cast<float>(command["coords"].attribute("s")),
			lexical_cast<float>(command["coords"].attribute("t")));
		return;
		}
	if(id == "hsvcolor")
		{
		glColorHSV(lexical_cast<float>(command["hsv"].attribute("h")),
			lexical_cast<float>(command["hsv"].attribute("s")),
			lexical_cast<float>(command["hsv"].attribute("v")));
		return;
		}
	if(id == "begin")
		{
		glBegin(toGLenum(command));
		return;
		}
	if(id == "end")
		{
		glEnd();
		return;
		}
	if(id == "enable")
		{
		glEnable(toGLenum(command));
		return;
		}
	if(id == "disable")
		{
		glDisable(toGLenum(command));
		return;
		}
	throw std::invalid_argument('\"' + id + "' is unsupported OpenGL command");
	}

//----------------------------------------------------------------------------

GLuint CuTe::buildDisplayList(const MyXML::Key& commands)
	{
	MyXML::KeysConstRange cmds = commands.keys("cmd");
	GLuint id = glGenLists(1);
	glNewList(id, GL_COMPILE);
	for(MyXML::KeyConstIterator command = cmds.first; command != cmds.second; ++command)
		executeGLCommand(command->second);
	glEndList();
	return id;
	}

//----------------------------------------------------------------------------
