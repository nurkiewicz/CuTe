//----------------------------------------------------------------------------

///@file
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

#include <windows.h>
#include <Mmsystem.h>
#include <stdexcept>
#include "sounds.h"
using namespace CuTe;

//----------------------------------------------------------------------------

namespace CuTe
	{
	Sounds sounds;
	}

//----------------------------------------------------------------------------

const std::string Sounds::soundPaths[Sounds::ALL_SOUNDS] = {
	"sn00.dat", "sn01.dat", "sn02.dat", "sn03.dat"};

void Sounds::play(unsigned int soundNum) const
	{
	if(soundNum >= ALL_SOUNDS)
		throw std::runtime_error("Invalid sound identifier");
 	if(enabled())
		PlaySound(("data/" + soundPaths[soundNum]).c_str(), NULL,
		SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
	}

//----------------------------------------------------------------------------

