//----------------------------------------------------------------------------

///@file
///High scores storing and manipulating logic.
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
///Classes for saving, restoring and showing CuTe highest scores in all difficulty levels
///independently.

//----------------------------------------------------------------------------

#ifndef HIGHSCORES_H
#define HIGHSCORES_H

//----------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <list>
#include <algorithm>
#include "MyXML/myxml.h"

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///Class manipulating one particular score.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///Class is designed to store and gain access to the data of one high score.
	class HighScore
		{
		private:

			///Date and time data structure.
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///Stores information about date (day, month and year) and time (hours and minutes).
			///@sa HighScores - container class for HighScore objects.
			struct DateTime
				{
				///Reads current date and time.
				///Reads current system date and time.
				///@return DateTime structure containing current date and time.
				static const DateTime getCurrentDateTime();

				///Hours.
				///Valid values are from 0 to 23.
				int hour;
				///Minutes in current hour.
				///Valid values are from 0 to 59.
				int min;
				///Day in current month.
				///Valid values are from 1 up to 31
				int day;
				///Month in current year.
				///Valid values are from 1 to 12
				int month;
				///Current year.
				///Year is stored in normal format (<b>not</b> as a difference between 1970 or 1900).
				int year;
				///Converts DateTime into string time stamp.
				///@return All the data in DateTime structure encapsulated into one string with the following
				///format:
				///@verbatim
				///DD-MM-YYYY, HH::MM
				///@endverbatim
				const std::string str() const;
				};		//struct DateTime

			///High score player's name.
			///Name of a player who hot the high score.
			///@sa HighScore(const std::string &player, int points, int gameTime)
			///@sa mainInfo()
			std::string player;
			///Game points got by the player.
			///Number of points which the player got.
			///@sa HighScore(const std::string &player, int points, int gameTime)
			///@sa mainInfo()
			int points;
			///Date and time when the high score was done.
			///@sa DateTime structure
			///@sa timeInfo()
			DateTime dateTime;
			///How long the game was.
			///Stores the information how long (in 1/10 sec) it took the player to got the high score.
			///@sa HighScore(const std::string &player, int points, int gameTime)
			///@sa timeInfo()
			int gameTime;
			///Inits all the class variables.
			///dateTime variable is set to current date and time.
			///@param iPlayer Player name, see @ref player
			///@param iPoints Game points, see @ref points
			///@param iGameTime Game time, see @ref gameTime
			///@sa dateTime
			///@sa HighScore(const std::string &player, int points, int gameTime)
			///@sa HighScore()
			void init(const std::string &iPlayer, int iPoints, int iGameTime);
		public:
			friend bool operator<=(const HighScore& left, const HighScore& right);
			friend MyXML::Key& operator<<(MyXML::Key& destKey, const HighScore& score);

			///Maximum player name string length.
			///Maximum number of characters (including white spaces) which the @ref player string
			///can hold.
			static const int PLAYER_NAME_MAX_LENGTH = 16;

			///Constructor.
			///This constructor is used when adding new high score durind the game.
			///@param player Player name
			///@param points Game points
			///@param gameTime Game time
			///@sa init()
			HighScore(const std::string &player, int points, int gameTime)
				{init(player, points, gameTime);}
			///Default constructor.
			///Creates an "empty score", which is a score with a player name "-" and 0 points.
			///This score is treated as an empty slot when displaying the high scores list.
			///@sa HighScore(const std::string &player, int points, int gameTime)
			HighScore()	{init("- ", 0, 0);}
			///Constructor loading data from XML input.
			///This constructor is used to create a high score info based on a specified XML key.
			///@param key Reference to a XML key which should have attributes: hour, min, day, month, year
			///and sub keys player and points.
			HighScore(const MyXML::Key& key);
			///Returns score's main info.
			///Main info is the player's name and the points formatted using commas.
			///@sa timeInfo()
			const std::string mainInfo() const;
			///Returns score's time info.
			///Time info is the information about date and time score got and the game length in format:
			///@verbatim
			///HH:MM:SS.S: DD-MM-YYYY, HH:MM
			///@endverbatim
			///@sa mainInfo()
			const std::string timeInfo() const;
			///Does the HighScore object store empty score?
			///@return True if the HighScore object stores empty score, which means a score with 0 points
			///(not shown in high scores info)
			bool empty() const	{return points == 0;}
		};		//class HighScore

//----------------------------------------------------------------------------

	///Saves high score in a specified XML key.
	///Creates a "score" key and puts all the high score key data into it in the following form:
	///@verbatim
	/// <score day="NN" hour="NN" min="NN" month="NN" time="NNNNNN" year="NNNN">
	/// 	<player>PLAYER_NAME</player>
	/// 	<points>NNNNNN</points>
	/// </score>
	///where "N...N" is replaced with the suitable data.
	///@endverbatim
	MyXML::Key& operator<<(MyXML::Key& destKey, const HighScore& score);
	///Overloaded operator<=() for HighScore object.
	///Is used in HighScore add routines to find the place where to put the new high score.
	///The overloaded operator is used indirectly in STL's algorithm find_if().<br>
	///The operator uses points count to find the less-equal object
	///@param left Left '<=' operand
	///@param right Right '<=' operand
	///@return True if left has less-equal points than right.
	bool operator<=(const HighScore& left, const HighScore& right);

//----------------------------------------------------------------------------

	///Container for all high scores in one particular difficulty level.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///Stores a sorted list of all high scores made at one particular difficulty level.
	class HighScores
		{
		private:
			///List of all high scores.
			///The list is sroted decreasing, so its first element is the highest score. The least
			///always has MAX_COUNT elements (if there's not enough high score at this difficulty level
			///the missing scores are filled with empty ones (0 points)).
			///@sa MAX_COUNT
			///@sa first() to see how to gain access to this list.
			std::list<const HighScore> scores;
			///Adds a score into the container.
			///This is the simplies version called from both other overloaded versions of add().
			///It first checks whether the score has enough points to be put in the high score.
			///@param score HighScore object to be put in HighScores list.
			///@return True if given score had enough points to be put in high scores (on so it happend)
			///otherwise false.
			///@sa Overloaded versions: add(const std::string& player, int points, int gameTime) and
			///add(const MyXML::Key& scoreKey)
			bool add(const HighScore& score);
		public:
			///Maximum (and guaranteed) number of high scores in a scores list.
			///This is the number of HighScore objects kept in scores list. Even if there's
			///not enough high scores to be put in the list, its size is always MAX_COUNT
			///(remaining scores are set to 0 points - empty).
			///@sa first()
			static const int MAX_COUNT = 10;

			///Fills scores list with empty scores.
			///Will the whole scores list with MAX_COUNT number of empty HighScore objects.
			///@sa MAX_COUNT
			HighScores()	{fill_n(back_inserter(scores), MAX_COUNT, HighScore());}
			///Returns first high score kept in scores container.
			///@note You don't need the twin function last() because you now that there are
			///exactly MAX_COUNT objects in the list, so you may increment safely the returned iterator
			///MAX_COUNT - 1 times
			///@return Const iterator to the first score stored in scores list. The first score is the
			///highest one.
			std::list<const HighScore>::const_iterator first() const	{return scores.begin();}
			///Adds a new high score.
			///This method is used after the game was finished. It collects all the data about the
			///game and complete it with current date by itself.
			///@param player High score player name
			///@param points High score points count
			///@param gameTime High score game time (in 1/10 s)
			///@return True if given score had enough points to be put in high scores (on so it happend)
			///otherwise false.
			///@sa bool add(const HighScore& score)
			bool add(const std::string& player, int points, int gameTime);
			///Adds a high score read from XML key.
			///This method is used during loading the high scores from XML file.
			///@param scoreKey Key representing one "score" XML key.
			///@return True if given score had enough points to be put in high scores (on so it happend)
			///otherwise false.
			///@sa bool add(const HighScore& score)
			bool add(const MyXML::Key& scoreKey)	{return add(HighScore(scoreKey));}
			///Returns the count of all high scores.
			///This method <b>does not</b> return the total number of high scores in the list (which
			///is always MAX_COUNT), but the number of non-empty score only <= MAX_COUNT.
			///@return Number of non-empty scores in the list.
			///@sa MAX_COUNT
			///@sa scores
			int count() const {return count_if(scores.begin(), scores.end(),
				std::not1(std::mem_fun_ref(&HighScore::empty)));}
		};

//----------------------------------------------------------------------------

	///Saves high scores in XML key.
	///Saves all the high scores from HighScores object into MyXML::Key object. It simply calls
	///overloaded operator<<() for all HighScore objects stored in HighScores list.
	///@param destKey Destination key; it should be the key named "difficulty" (all the scores
	///will be saved as nested keys).
	///@param scores HighScores object to be saved in XML key.
	///@return Reference to the destKey.
	///@sa operator>>(const MyXML::Key& srcKey, HighScores& dest)
	MyXML::Key& operator<<(MyXML::Key& destKey, const HighScores& scores);
	///Reads all high scores for one particular difficulty level into HighScores object.
	///It searches in srcKey for all nested "score" keys and loads its data into HighScores
	///container.
	///@param srcKey Source XML key; it should be the key named "difficulty" containing nested
	///keys "score"
	///@param dest Destination HighScores container object. All found scores in "score" keys will
	///be saved to in it.
	///@return Const reference to the source key srcKey
	///@sa operator<<(MyXML::Key& destKey, const HighScore& score)
	const MyXML::Key& operator>>(const MyXML::Key& srcKey, HighScores& dest);

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif

//----------------------------------------------------------------------------
