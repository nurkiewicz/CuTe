//----------------------------------------------------------------------------

///@file
///Some numeric and other common functions.
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

#ifndef COMMON_H
#define COMMON_H

//----------------------------------------------------------------------------

#include <string>
#include "point.h"

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///Simple algorithms for decreasing the absolute value of any type.
	///The idea of this algorithm is to decrease the absolute value of val by diff.
	///It means sub diff from val if val is positive or add diff to val if val is negative.
	///If after this the val change its sign, it is set to 0 (T())
	///@param val Reference to a value that should be changed (it is position or angle)
	///@param diff How much to change the val (distance or angle)
	template<typename T>
	void decAbs(T &val, T diff)
		{
		if(val > T())
			if((val -= diff) < T())
				val = T(); else;
		else
			if(val < T())
				if((val += diff) > T())
					val = T();
		}

	///Overloaded decAbs() version for Point<T, 3> type.
	///Simply calls decAbs<T> for every coordinate of Point<T, 3>
	///@param val Reference to a point which coordinates should be decreased.
	///@param diff How much to change every coordinate
	template<typename T>
	void decAbs(Point<T, 3> &val, T diff)
		{
		decAbs(val.x(), diff);
		decAbs(val.y(), diff);
		decAbs(val.z(), diff);
		}


//----------------------------------------------------------------------------

	///Returns the square of the argument.
	///Uses standard binary operator*() to perform multiplication. Can be used with any data type which
	///has this operator available (especially numeric types).
	///@param val Value to count the squre.
	///@return Squre of the value val.
	template<typename T>
	T sqr(T val)
		{
		return val * val;
		}

//----------------------------------------------------------------------------

	///Deletes specified object using operator delete.
	///Very ofthen we want to delete the whole container of pointers. But using STL's for_each we can't
	///just write:
	///@code
	///for_each(c.begin(), c.end(), ptr_fun(operator delete))
	///@endcode
	///We must instead wrap the delete operator into a simple function or functor.
	///@param p Pointer to the object which should be deleted.
	template<typename T>
	void deleter(T *p)
		{
		delete p;
		}

//----------------------------------------------------------------------------

	///Converts integral value for formatted string.
	///Formatted string is a string containing digits separated with commas; every three digits
	///(from the back) are separeted with a single comma. For example:
	///@code
	///std::string s = intToFmtString(12345678);	//"12,345,678"
	///@endcode
	const std::string intToFmtStr(int num);

//----------------------------------------------------------------------------

	///Converts time (in 1/10 second) to formatted string.
	///Formatted string looks like this:
	///@verbatim
	///01:27:14.3
	///@endverbatim
	///which stands for 1 hour, 27 minutes, 14.3 seconds.
	const std::string timeToFmtStr(int gameTime);

//----------------------------------------------------------------------------

	} //namespace CuTe

//----------------------------------------------------------------------------

#endif

//----------------------------------------------------------------------------
