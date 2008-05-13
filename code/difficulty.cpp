//----------------------------------------------------------------------------

///@file
///Game difficiculty Difficulty class definitions.
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

#include <boost/lexical_cast.hpp>
#include "language.h"
#include "difficulty.h"
using namespace CuTe;
using boost::lexical_cast;

//----------------------------------------------------------------------------

bool CuTe::operator<(const DifficultyData& left, const DifficultyData& right)
	{
	if(left.size_ < right.size_)
		return true;
	if(left.size_ > right.size_)
		return false;
	if(left.depth_ < right.depth_)		//now left.size_ == right.size_
		return true;
	if(left.depth_ > right.depth_)
		return false;
	if(left.blocksSet_ < right.blocksSet_)		//now left.depth_ == right.depth_
		return true;
	return false;
	}

MyXML::Key& CuTe::operator<<(MyXML::Key& destKey, const DifficultyData& difficulty)
	{
	destKey.attribute("size") = lexical_cast<std::string>(difficulty.size_);
	destKey.attribute("depth") = lexical_cast<std::string>(difficulty.depth_);
	destKey.attribute("blocksSet") = lexical_cast<std::string>(difficulty.blocksSet_);
	return destKey;
	}

//----------------------------------------------------------------------------

Difficulty::Difficulty(MyXML::Key& iDiffData):
	DifficultyData(MyXML::readAttrDef(iDiffData, "size", SIZE_MEDIUM),
		MyXML::readAttrDef(iDiffData, "depth", DEPTH_MEDIUM),
		MyXML::readAttrDef(iDiffData, "blocksSet", BLOCKS_SET_FLAT)),
	diffData(iDiffData)
	{
	findLevel();
	}

int Difficulty::validate(int value, int minValue, int maxValue)
	{
	if(value < minValue)
		return maxValue;
	if(value > maxValue)
		return minValue;
	return value;
	}

const std::string Difficulty::blocksSetStr(int blocksSetNum)
	{
	switch(blocksSetNum)
		{
		case BLOCKS_SET_CLASSIC: return langData["optionsMenu"]["blocksSetClassic"].value();
		case BLOCKS_SET_FLAT: return langData["optionsMenu"]["blocksSetFlat"].value();
		case BLOCKS_SET_EXTREME: return langData["optionsMenu"]["blocksSetExtreme"].value();
		default: return "";
		};
	}

const std::string Difficulty::levelStr(int difficultyNum)
	{
	switch(difficultyNum)
		{
		case EASY: return langData["mainMenu"]["difficultyEasy"].value();
		case MEDIUM: return langData["mainMenu"]["difficultyMedium"].value();
		case HARD: return langData["mainMenu"]["difficultyHard"].value();
		case CUSTOM: return langData["mainMenu"]["difficultyCustom"].value();
		default: return "";
		}
	}

void Difficulty::change(int& value, int newValue, int minValue, int maxValue)
	{
	//perform even test only when amount is even
	value = validate(newValue, minValue, maxValue);
	findLevel();
	}

void Difficulty::findLevel()
	{
	if((size_ == SIZE_EASY) && (depth_ == DEPTH_EASY) && (blocksSet_ == BLOCKS_SET_CLASSIC))
		{
		level_ = EASY;
		return;
		}
	if((size_ == SIZE_MEDIUM) && (depth_ == DEPTH_MEDIUM) && (blocksSet_ == BLOCKS_SET_FLAT))
		{
		level_ = MEDIUM;
		return;
		}
	if((size_ == SIZE_HARD) && (depth_ == DEPTH_HARD) && (blocksSet_ == BLOCKS_SET_EXTREME))
		{
		level_ = HARD;
		return;
		}
	level_ = CUSTOM;
	}

void Difficulty::level(int newLevel)
	{
	level_ = validate(newLevel, EASY, CUSTOM);
	changeLevelData();
	}

void Difficulty::changeLevelData()
	{
	switch(level_)
		{
		case EASY:
			size_ = SIZE_EASY;
			depth_ = DEPTH_EASY;
			blocksSet_ = BLOCKS_SET_CLASSIC;
			break;
		case MEDIUM:
			size_ = SIZE_MEDIUM;
			depth_ = DEPTH_MEDIUM;
			blocksSet_ = BLOCKS_SET_FLAT;
			break;
		case HARD:
			size_ = SIZE_HARD;
			depth_ = DEPTH_HARD;
			blocksSet_ = BLOCKS_SET_EXTREME;
			break;
		};
	//if we choose custom level, only level_ is changed, but the size_, depth_ etc. are left the same
	}

//----------------------------------------------------------------------------

std::ostream& CuTe::operator<<(std::ostream& os, const Difficulty& difficulty)
	{
	return os << difficulty.size() << 'x' << difficulty.size() << 'x' <<
		difficulty.depth() << " @ " << difficulty.blocksSetName();
	}

//----------------------------------------------------------------------------
