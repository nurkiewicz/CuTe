//----------------------------------------------------------------------------

///@file
///High scores classes definitions.
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

#include <ctime>
#include <iomanip>
#include <sstream>
#include <functional>
#include <boost/lexical_cast.hpp>
#include "common.h"
#include "highscores.h"
using namespace CuTe;
using namespace std;
using boost::lexical_cast;

//----------------------------------------------------------------------------

const std::string HighScore::DateTime::str() const
	{
	ostringstream oss;
	oss.fill('0');
	oss << setw(2) << day << '-' << setw(2) << month << '-' << year << ", " <<  setw(2) <<
		hour << ':' << setw(2) << min;
	return oss.str();
	}

//----------------------------------------------------------------------------

const HighScore::DateTime HighScore::DateTime::getCurrentDateTime()
	{
	time_t t;
	time(&t);
	tm localTime = *localtime(&t);
	DateTime dateTime;
	dateTime.hour = localTime.tm_hour;
	dateTime.min = localTime.tm_min;
	dateTime.day = localTime.tm_mday;
	dateTime.month = localTime.tm_mon + 1;
	dateTime.year = localTime.tm_year + 1900;
	return dateTime;
	}

//----------------------------------------------------------------------------

HighScore::HighScore(const MyXML::Key& key)
	{
	player = key["player"];
	points = lexical_cast<int, std::string>(key["points"]);
	gameTime = lexical_cast<int>(key.attribute("time"));
	dateTime.hour = lexical_cast<int>(key.attribute("hour"));
	dateTime.min = lexical_cast<int>(key.attribute("min"));
	dateTime.hour = lexical_cast<int>(key.attribute("hour"));
	dateTime.day = lexical_cast<int>(key.attribute("day"));
	dateTime.month = lexical_cast<int>(key.attribute("month"));
	dateTime.year = lexical_cast<int>(key.attribute("year"));
	}

void HighScore::init(const std::string &iPlayer, int iPoints, int iGameTime)
	{
	player = iPlayer;		//iPlayer should be at most PLAYER_NAME_MAX_LENGTH characters!
	points = iPoints;
	gameTime = iGameTime;
	dateTime = DateTime::getCurrentDateTime();
	}

const std::string HighScore::mainInfo() const
	{
	ostringstream os;
	os << setw(PLAYER_NAME_MAX_LENGTH) << right << player << ": " << setw(10) << intToFmtStr(points);
	return os.str();
	}

const std::string HighScore::timeInfo() const
	{
	if(points > 0)
		return timeToFmtStr(gameTime) + ": " + dateTime.str();
	return "";
	}

//----------------------------------------------------------------------------

MyXML::Key& CuTe::operator<<(MyXML::Key& destKey, const HighScore& score)
	{
	if(score.points > 0)		//don't save "empty" (with 0 points) scores
		{
		MyXML::Key& scoreKey = destKey.insert("score");
		scoreKey.attribute("hour") = lexical_cast<std::string>(score.dateTime.hour);
		scoreKey.attribute("min") = lexical_cast<std::string>(score.dateTime.min);
		scoreKey.attribute("day") = lexical_cast<std::string>(score.dateTime.day);
		scoreKey.attribute("month") = lexical_cast<std::string>(score.dateTime.month);
		scoreKey.attribute("year") = lexical_cast<std::string>(score.dateTime.year);
		scoreKey.attribute("time") = lexical_cast<std::string>(score.gameTime);
		scoreKey["player"] = score.player;
		scoreKey["points"] = score.points;
		}
	return destKey;
	}

bool CuTe::operator<=(const HighScore& left, const HighScore& right)
	{
	return left.points <= right.points;
	}

//----------------------------------------------------------------------------

bool HighScores::add(const HighScore& score)
	{
	//Find highest score which is less or equal than the new one
	const list<const HighScore>::iterator leScore =
		find_if(scores.begin(), scores.end(), bind2nd(less_equal<HighScore>(), score));
	if(leScore == scores.end())
		return false;		//score is too little to be added to high scores
	scores.insert(leScore, score);		//add new score somewhere in the middle
	scores.pop_back();		//remove the worst score (last before adding)
	return true;
	}

bool HighScores::add(const std::string& player, int points, int gameTime)
	{
	if(points <= 0)
		return false;		//don't save empty scores
	return add(HighScore(player, points, gameTime));
	}

//----------------------------------------------------------------------------

MyXML::Key& CuTe::operator<<(MyXML::Key& destKey, const HighScores& scores)
	{
	list<const HighScore>::const_iterator score = scores.first();
	for(int s = 0; s < HighScores::MAX_COUNT; ++s, ++score)
		destKey << *score;
	return destKey;
	}

const MyXML::Key& CuTe::operator>>(const MyXML::Key& srcKey, HighScores& dest)
	{
	MyXML::KeysConstRange range = srcKey.keys("score");		//find all <score> keys
	for(MyXML::KeyConstIterator i = range.first; i != range.second; ++i)
		dest.add(i->second);
	return srcKey;
	}

//----------------------------------------------------------------------------
