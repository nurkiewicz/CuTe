//---------------------------------------------------------------------------

///@file
///Declaration of a template classes to count any frequencies, especially framerate.
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
///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
///@date Jul 2005-Mar 2006
///@par
///First class (EventFreqCounter) can count any frequency, second one  (FPSCounter)
///is a specialization adapted to count frames per second (FPS) e.g. in OpenGL scene.

//---------------------------------------------------------------------------

#ifndef MYOGL_FREQ_COUNTER_H
#define MYOGL_FREQ_COUNTER_H

//---------------------------------------------------------------------------

#include <numeric>
#include "timer.h"

//---------------------------------------------------------------------------

namespace MyOGL	{

//---------------------------------------------------------------------------

///Class adapted to count the frequency of any events.
///@author Tomasz Nurkiewicz
///@date Jul 2005-Mar 2006
///@par
///@code
/// #include "fps.h"
/// #include <iostream>
/// using namespace std;
///
/// int main()
/// 	{
/// 	EventFreqCounter<int, 10000> freq;
/// 	while(!freq.updated())
/// 		{
/// 		int x = 0;
/// 		while(x < 1000)
/// 			++x;
/// 		freq.event();
/// 		}
/// 	cout << freq / 1000.0 << " kHz" << endl;
/// 	return 0;
/// 	}
///@endcode
///On my computer this program after 10 seconds prints (of course this result will vary on your machine):
///@code 391.974 kHz @endcode
///which means that inner while loop ran almost 400 thousand times by average during every second
///@param T Specify what numeric data type to use to store the frequency.
///Floating point types are recommended for better precision
///@param periodLength Length of a single period (time interval between counting the frequency) in miliseconds.
///Pleas note that this class measures frequency in hertz (events per second), not in events per period;
///this parameter is only used to determine how often frequency is updated. It is recommended just to live
///its default value 1000 (1 second)
///@par
///If you want to count frequency in events per period, simply multiply frequency from this class by length of period.
template<typename T, int periodLength = 1000>
class EventFreqCounter
	{
	private:
		///Stores the frequency in previous period.
		///@sa event()
		T freq;
		///Number of events which happend during the current period.
		///@sa event()
		int events;
		///Time which elapsed since the end of previous period.
		///Frequency needs to be updated if this time is bigger than periodLength.
		Timer curPeriodLength;
		///Indicates not red frequency info.
		///If frequency was updated and not red yet, this flag i set until first reading by operator T() method.
		///@sa event()
		bool fUpdated;
		///Used to init the counter.
		///@sa EventFreqCounter()
		///@sa restart()
		void init();
		///Informs whether time since the end of last period is longer than periodLength.
		///If this method returns true, frequency needs to be updated.
		///@return True if time since the end of previous period is longer than specified length of period.
		///@sa event()
		///@sa updated()
		///@sa fUpdated
		bool periodPast()	{return curPeriodLength > periodLength;}
		///Updates frequency.
		///This method needs to be called every time periodPast() returned true.
		///This job is done automatically and internally by a class.
		void update();
	public:
		///Constructor.
		///@sa init()
		EventFreqCounter()	{init();}
		///Overloaded operator T().
		///Very handy method which abels you to treat EventFreqCounter object as a ordinary T variable.
		///Look for an example of such a behaviour in class EventFreqCounter detailed description.
		///@sa EventFreqCounter
		operator T();
		///Count next event.
		///Call this method whenever you want to count frequency of some event.
		///Look for an example of using event() method in class EventFreqCounter detailed description.
		///@sa EventFreqCounter
		void event();
		///Informs whether frequency was updated.
		///Frequency variable freq is updated periodically after every period ends (it is not counted continously for performance reasons).
		///If you want to check whether frequency was updated, simply call this method.
		///It gives you an opportunity to read frequency only once immediately after it was calculated.
		///@return True if frequency was updated since its last read. Otherwise false.
		///@sa fUpdated
		///@sa operator T()
		bool updated();
		///Restars frequency counter.
		///Counter is started by default during its creation, so if you want to start at the different moment, call this method.
		///@sa init()
		///@sa EventFreqCounter()
		void restart();
	};

template<typename T, int periodLength>
void EventFreqCounter<T, periodLength>::event()
	{
	++events;
	if(periodPast())
		update();
	}

template<typename T, int periodLength>
void EventFreqCounter<T, periodLength>::update()
	{
	//frequency = events / T (in s) = events * 1000.0 / periodLength (in ms)
	freq = static_cast<int>(events * 1000.0 / periodLength);
	events = 0;
	curPeriodLength.shift(-periodLength);
	fUpdated = true;
	}

template<typename T, int periodLength>
bool EventFreqCounter<T, periodLength>::updated()
	{
	if(periodPast())
		update();
	return fUpdated;
	}

template<typename T, int periodLength>
EventFreqCounter<T, periodLength>::operator T()
	{
	if(periodPast())
		update();
	fUpdated = false;
	return freq;
	}

template<typename T, int periodLength>
void EventFreqCounter<T, periodLength>::init()
	{
	freq = 0;
	events = 0;
	fUpdated = false;
	}

template<typename T, int periodLength>
void EventFreqCounter<T, periodLength>::restart()
	{
	init();
	curPeriodLength.restart();
	}

//---------------------------------------------------------------------------

///Simple Frames Per Second (FPS) counter based on EventFreqCounter class.
///@author Tomasz Nurkiewicz
///@date Jul 2005-Mar 2006
///@par
///This class counts the number of frame() method calls done every second, and returns that number.
///Besides it can count average FPS from few previous seconds (see PREV_SAVE).
///Counter is refreshed every second (it's not working continously).
///@param PREV_SAVE frames from how many seconds past should be stored. Default value 1 means that it saves framerate only from current second.
///If you want to save framerates for some seconds from the past, choose a bigger number.
///Those information is used by average() method to count the average framerate from the last PREV_SAVE seconds.
///PREV_SAVE = 1 disables this feature.
///@sa frame()
///@sa operator int()
///@sa average()
///@sa updated()
template<int PREV_SAVE = 1>
class FPSCounter
	{
	private:
		///Number of frame() calls in some seconds ago.
		///This is a rotation bufer. Calls count from next second are stored in next array field,
		///but in case we are already in the last field, we're going back to the first one (where the data is oldest).
		///That's how the oldest data is succesively replaced by the newest one.
		///@sa frame()
		///@sa prevFreqFull
		///@sa PREV_SAVE
		///@sa pos
		int prevFreq[PREV_SAVE];
		///Position in prevFreq.
		///prevFreq[pos] stores framerate in the last second. After every second this variable increments
		///on condition that we're not at the end of prevFreq array. If so, it goes back to 0.
		///See explanation in prevFreq.
		///@sa prevFreq
		int pos;
		///Counts frames in current second.
		///Using EventFreqCounter we are counting the frequency of some events (in our case those are drawn frames)
		///and storing it in this variable. Every time EventFreqCounter updates, the value is saved in prevFreq.
		///@sa prevFreq
		///@sa EventFreqCounter
		EventFreqCounter<int> fps;
		///Indicates whether FPSCounter was updated.
		///If this variable is true it means that since last reading the frame counter was updated.
		///@sa updated()
		bool fUpdated;
		///Average framerate from last PREV_SAVE seconds.
		///This is the average value of data stored in prevFreq, returned by average() method.
		///It is counted only once after every counter update for performance reasons.
		///@sa prevFreq
		///@sa average()
		float fAverage;
		///Updates various data in a class.
		///It is called whenever fps object is updated. This method sets up new fAverage and
		///moves throught prevFreq array.
		void update();
		///Resets some various class variables.
		///@sa FPSCounter
		///@sa restart()
		void init();
	public:
		///Constructor.
		///After creating FPSCounter object, counter is started immediately.
		///@sa restart()
		///@sa init()
		FPSCounter()	{init();}
		///Counts frames.
		///Call this method whenever you are drawing frame. This way you will count your refreshed frames.
		///@sa operator int()
		///@sa average()
		void frame();
		///Indicates whether counter was updated.
		///Use this function if you want to read FPS or average FPS only once immediately after it
		///has been updated. This way you always have fresh data in constant periods of time
		///and you don't have to read those data every time you e.g. refresh your scene (redundant).
		///@return True if since last reading or checking the counter state, counter was updated.
		///@sa fUpdated
		bool updated();
		///Overloaded int() operator.
		///Gives you ability to work with FPSCounter as an ordinary int variable.
		///You may read your current FPS simply like this:
		///@code
		///FPSCounter fps;
		/// // . . .
		///cout << fps << " FPS" << endl;
		///@endcode
		///@sa average()
		operator int();
		///Returns average framerate.
		///@return Sum of all frames counted in last PREV_SAVE seconds divided by PREV_SAVE
		///@sa fAverage
		float average();
		///Restarts the counter.
		///Counter is automatically started after it's being created.
		///Call this method to start it again.
		///@sa FPSCounter()
		void restart();
	};

template<int PREV_SAVE>
void FPSCounter<PREV_SAVE>::init()
	{
	pos = PREV_SAVE - 1;
	fUpdated = false;
	fAverage = 0.0;
	fill(prevFreq, prevFreq + PREV_SAVE, 0);
	}

template<int PREV_SAVE>
void FPSCounter<PREV_SAVE>::frame()
	{
	fps.event();
	if(fps.updated())
		update();	//new second, needs to update the FPS counter
	}

template<int PREV_SAVE>
void FPSCounter<PREV_SAVE>::update()
	{
	pos = (pos + 1) % PREV_SAVE;
	prevFreq[pos] = fps;
	fAverage = std::accumulate(&prevFreq[0], &prevFreq[PREV_SAVE], 0) / static_cast<float>(PREV_SAVE);
	fUpdated = true;
	}

template<int PREV_SAVE>
bool FPSCounter<PREV_SAVE>::updated()
	{
	if(fps.updated())
		update();	//new second, needs to update the FPS counter
	return fUpdated;
	}

template<int PREV_SAVE>
FPSCounter<PREV_SAVE>::operator int()
	{
	if(fps.updated())
		update();	//new second, needs to update the FPS counter
	fUpdated = false;
	return prevFreq[pos];
	}

template<int PREV_SAVE>
float FPSCounter<PREV_SAVE>::average()
	{
	if(fps.updated())
		update();	//new second, needs to update the FPS counter
	fUpdated = false;
	return fAverage;
	}

template<int PREV_SAVE>
void FPSCounter<PREV_SAVE>::restart()
	{
	init();
	fps.restart();
	}

//---------------------------------------------------------------------------

}	//namespace MyOGL

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
