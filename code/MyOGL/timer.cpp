//---------------------------------------------------------------------------

///@file
///Stores declaration of class Timer, used as a simple timer.
///
///@par License:
///@verbatim
///MyOGL - My OpenGL utility, simple OpenGL Windows framework
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
///@author Tomasz Nurkiewicz
///@date Jul 2005-Mar 2006

//---------------------------------------------------------------------------

#include "timer.h"
using namespace MyOGL;
using namespace std;

//---------------------------------------------------------------------------

Timer::operator int() const
	{
	if(pauseTime == 0)
		{
		return ticks2ms(clock() - startTime);
		}
	else
		return ticks2ms(pauseTime - startTime);
	}

Timer &Timer::shift(int shiftTime)
	{
	startTime -= ms2ticks(shiftTime);
	return *this;
	}

int Timer::restart()
	{
	int lastTime = operator int();		//save the current time
	startTime = clock();		//and reset the timer
	pauseTime = 0;
	return lastTime;
	}

void Timer::pause()
	{
	if(pauseTime == 0)
		pauseTime = clock();
	}

void Timer::resume()
	{
	if(pauseTime > 0)
		{
		//calculates the new startTime depending on the amount of time when program was paused
		startTime = clock() - (pauseTime - startTime);
		pauseTime = 0;
		}
	}

//---------------------------------------------------------------------------
