//----------------------------------------------------------------------------

///@file
///Simple sounds in CuTe game class declaration.
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

#ifndef SOUNDS_H
#define SOUNDS_H

//----------------------------------------------------------------------------

#include <string>

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///Class handling some simple sounds in CuTe game.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Feb 2006
	///@par
	///Sounds are loaded from .wav files.
	class Sounds
		{
		public:
			///Sets the sounds to be enabled by default.
			Sounds(): _enabled(true)	{}
			///Plays a specified sound.
			///@param soundNum Sound identifier to be played.
			///@sa SWITCH_BLOCKS, REMOVING, ROTATE, GAME_OVER
			///@throws std::runtime_error When soundNum is greater or equal to ALL_SOUNDS
			void play(unsigned int soundNum) const;
			///Enables/disables sounds.
			///@param enabled True if you want to enable the sounds, otherwise false.
			void enable(bool enabled)	{_enabled = enabled;}
			///Returns the courret sounds state.
			///@return True if the sounds are enabled at the moment, otherwise false.
			bool enabled() const	{return _enabled;}
			///Switching block sound identifier.
			///@sa play()
			static const int SWITCH_BLOCKS = 0;
			///Removing filled plane sound identifier.
			///@sa play()
			static const int REMOVING = 1;
			///Rotating block sound identifier.
			///@sa play()
			static const int ROTATE = 2;
			///Game over sound identifier.
			///@sa play()
			static const int GAME_OVER = 3;
		private:
			///Sounds enable/disable flag.
			///@sa enable() to set this flag
			///@sa enabled() to read the flag
			bool _enabled;
			///Number of all different sounds.
			///@sa play()
			static const unsigned int ALL_SOUNDS = 4;
			///Paths to the sound .wav files.
			///The leading "data/" is added in play() method.
			static const std::string soundPaths[ALL_SOUNDS];
		};		//class Sounds

//----------------------------------------------------------------------------

	extern Sounds sounds;

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif		//#define SOUNDS_H

//----------------------------------------------------------------------------
