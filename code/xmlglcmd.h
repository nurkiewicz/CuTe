//----------------------------------------------------------------------------

///@file
///Routines for reading and processing OpenGL commands saved in XML file.
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

#ifndef XMLGLCMD_H
#define XMLGLCMD_H

//----------------------------------------------------------------------------

#include "MyXML/myxml.h"
#include "MyOGL/scene.h"

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///Calls OpenGL command coded in XML key.
	///XML key should have the following pattern:
	///@verbatim
	/// <cmd id="SSS" />
	///@endverbatim
	///Where 'SSS' should be replaced with one of this GL id's:
	/// <table><tr><td>vertex</td><td>Put vertex; key should have a sub key "coords" with attributes "x"
	/// "y" and "z"</td></tr>
	/// <tr><td>hsvcolor</td><td>Changes the color using HSV color model; key should have a sub key
	/// "hsv" with attributes "h", "s" and "v"</td></tr>
	/// <tr><td>begin</td><td>Begin OpenGL drawings; the key's value should be one of the: GL_LINES,
	/// GL_LINE_LOOP</td></tr>
	/// <tr><td>end</td><td>Finishes the OpenGL drawings</td></tr>
	/// <tr><td>enable</td><td>calls glEnable(); the key's value should be OpenGL enumerator
	/// to enable (e.g. TEXTURE_2D)</td></tr>
	/// <tr><td>disable</td><td>calls glDisable(); the key's value should be OpenGL enumerator
	/// to disable (e.g. TEXTURE_2D)</td></tr>
	/// <tr><td>texcoords</td><td>calls glTexCoord2f() with given parameters; the key should have a
	/// sub key "coords" which has attributes "s" and "t"</td></tr></table>
	///@param command XML key containing coded OpenGL command.
	void executeGLCommand(const MyXML::Key& command);

	///Builds the OpenGL display list containing desired set of GL commands.
	///Creates an OpenGL display list and returns its alias for futher use in glCallList()
	///@param commands XML key containing coded OpenGL commands how to build display list.
	///@return OpenGL alias to the display list which can be used in the future by glCallList()
	///@sa executeGLCommand()
	GLuint buildDisplayList(const MyXML::Key& commands);

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif		//#define XMLGLCMD_H

//----------------------------------------------------------------------------
