//---------------------------------------------------------------------------

///@file
///Stores definitions of non-inline methods of class Timer.
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
///@par

//---------------------------------------------------------------------------

#ifndef TIMER_H
#define TIMER_H

//---------------------------------------------------------------------------

#include <ctime>

//---------------------------------------------------------------------------

namespace MyOGL
	{

//---------------------------------------------------------------------------

	///Class used to store and count time in miliseconds.
	///@author Tomasz Nurkiewicz
	///@date Jul 2005-Mar 2006
	///@par Example:
	///Example code of using the class:
	///@code
	/// #include "timer.h"
	/// #include <iostream>
	/// using namespace std;
	///
	/// int main()
	/// 	{
	/// 	MyOGL::Timer t;
	/// 	const int DELAY_1 = 10000, DELAY_2 = DELAY_1 * 2;
	/// 	int x, y;
	/// 	t.restart(); //(1)
	/// 	for(int x = 0; x < DELAY_1; ++x)
	/// 		for(int y = 0; y < DELAY_1; ++y)
	/// 			;
	/// 	cout << "After " << DELAY_1 * DELAY_1 << " operations: " << t << "ms" << endl;
	/// 	cout << endl << "Press [Enter] . . . " << endl;
	/// 	t.pause();
	/// 	getchar();
	/// 	t.resume();
	/// 	for(int x = 0; x < DELAY_2; ++x)
	/// 		for(int y = 0; y < DELAY_2; ++y)
	/// 			;
	/// 	cout << "And another " << DELAY_2 * DELAY_2 << " operations: " << t << "ms" << endl;
	/// 	return 0;
	/// 	}
	///@endcode
	///Please note that calling <code>restart()</code> at (1) is redundant - Timer objects starts automatically after initialization.
	///@par
	///Those are the program results (they may vary depending on your computer speed):
	///@verbatim
	///After 100000000 operations: 261ms
	///
	///Press [Enter] . . .
	///
	///And another 400000000 operations: 1313ms
	///@endverbatim
	///@par Liimitations
	///Timer class uses standard library function clock() from time.h. This function may not be precise enough for some tasks.
	///I will try to find different and portable way to calculate time in the future.
	class Timer
		{
		private:
			std::clock_t startTime;		///<Processor time when the timer started to count
			///Processor time when pause() method was called.
			///0 by default means that pause() wasn't called ever or resume() was called after it
			std::clock_t pauseTime;
			///Static method used to convert time from processor time to miliseconds
			///@param ticks Processor time (returned by clock()) to convert
			///@return time in miliseconds converted from time in processor time
			///@sa ms2ticks(int ms)
			static int ticks2ms(std::clock_t ticks)	{return static_cast<int>(static_cast<double>(ticks) / CLOCKS_PER_SEC * 1000);}
			///Static method used to convert time from miliseconds to processor time
			///@param ms Miliseconds to convert
			///@return processor time converted from time in miliseconds
			///@sa ticks2ms(std::clock_t ticks)
			static std::clock_t ms2ticks(int ms)	{return static_cast<std::clock_t>(ms * CLOCKS_PER_SEC / 1000.0);}
		public:
			///Constructor.
			///Immediately after construction timer is set on
			Timer()	{restart();}
			///Standard convertion from the class Timer to int.
			///This operator is used to simply check the time stored in a class.
			///Consider this example code:
			///@code
			///Timer tm;
			/// // . . .
			///int t = tm;  //(1)
			///cout << tm << "ms" << endl;  //(2)
			///@endcode
			///In both situations code will work properly, because object <code>tm</code> is automatically converted into int,
			///so it can be assigned to and int variable (1) or printed by cout as na ordinary int.
			///@return time in miliseconds which elapsed since Timer was started
			operator int() const;
			///Changes time relatively to current time.
			///It gives opportunity to manually change time stored in Timer object.
			///@par Example:
			///@code
			/// Timer t;
			/// // . . .
			/// t.shift(2000); //add 2 seconds (2000 ms) to timer
			/// // . . .
			/// t.shift(-500); //substract half second from timer
			///@endcode
			///@param shiftTime time to add in miliseconds
			///@sa pause()
			Timer &shift(int shiftTime);
			///Restarts the timer.
			///This method is called automatically when you create a Timer object.
			///You can run it explicitly, if you want to reset timer and start it again.
			///@return Time in ms which was stored in a Timer object just before starting it again.
			///This might be usefull because when you want to read the timer and reset it at the same time
			///you simply call:
			///@code
			/// Timer timer;
			/// // . . .
			/// int t = timer.restart();
			///@endcode
			///insted of:
			///@code
			/// Timer timer;
			/// // . . .
			/// int t = timer;
			/// timer.restart();
			///@endcode
			///@sa pause()
			///@sa resume()
			int restart();
			///Pauses the timer.
			///This method stores the time of pause() call in pauseTime.
			///@sa pauseTime
			///@sa resume()
			///@sa restart()
			void pause();
			///Resumes the timer after the pause() method was called.
			///@sa pause()
			///@sa restart()
			void resume();
		};

//---------------------------------------------------------------------------

	}	//namespace MyOGL

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
