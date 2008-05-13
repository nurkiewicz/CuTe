//----------------------------------------------------------------------------

///@file
///Definitions of some common functions and templates.
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

#include <sstream>
#include <string>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include "common.h"
using namespace CuTe;
using namespace std;

//----------------------------------------------------------------------------

const std::string CuTe::intToFmtStr(int num)
	{
	string numStr = boost::lexical_cast<string>(num);
	for(int pos = numStr.length() - 3; pos > 0; pos -= 3)
		numStr.insert(pos, ",");
	return numStr;
	}

//----------------------------------------------------------------------------

const std::string CuTe::timeToFmtStr(int gameTime)
	{
	ostringstream os;
	os.fill('0');
	os << setw(2) << gameTime / (10 * 60 * 60) << ":";
	gameTime %= 10 * 60 * 60;
	os << setw(2) << gameTime / (10 * 60) << ":";
	gameTime %= 10 * 60;
	os << setw(2) << gameTime / 10 << ".";
	os << gameTime % 10;
	os.fill(' ');
	return os.str();
	}

//----------------------------------------------------------------------------
