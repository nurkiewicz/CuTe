//----------------------------------------------------------------------------

///@file
///Two XML keys with language data and additional information.
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
///This file is needed everywhere where game must display some messages.

//----------------------------------------------------------------------------

#ifndef LANGUAGE_H
#define LANGUAGE_H

//----------------------------------------------------------------------------

#include "MyXML/myxml.h"

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///Declaration of a langData variable.
	///This variable is defined in the main.cpp file. It stores all the messages displayed in the
	///game allowing to introduce multiple languages to the game.
	extern MyXML::Key langData;

	///Definition of a langInfo variable.
	///This variable is defined in the main.cpp file. It stores the language additional information
	///like language pack author, fonts, etc.
	extern MyXML::Key langInfo;

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif		//#define LANGUAGE_H

//----------------------------------------------------------------------------
