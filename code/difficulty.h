//----------------------------------------------------------------------------

///@file
///Game difficulty info class.
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
///The class has two goals:<br>
///- store and make available such game informations like size, depth, etc.<br>
///- besides storing, make it possible to change these data in the most convenient way for the
///game menu

//----------------------------------------------------------------------------

#ifndef DIFFICULTY_H
#define DIFFICULTY_H

//----------------------------------------------------------------------------

#include <string>
#include "MyXML/myxml.h"

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///Base class for Difficulty.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///This class only stores the essential difficulty level information. It doesn't check, validate
	///or process them in any matter.
	///@sa Difficulty class
	class DifficultyData
		{
		protected:
			///Size of a cuboid game.
			///Size is width and height of a cuboid. It is one of the most important factors
			///which influence the game difficulty.
			int size_;
			///Depth of a cuboid game.
			///It is one of the most important factors which influence the game difficulty.
			int depth_;
			///Blocks set used in a game.
			///Informs which blocks set to use during the game. The bigger set - the harder blocks
			///will be loaded and the game will be more difficult.
			int blocksSet_;
		public:
			friend bool operator<(const DifficultyData& left, const DifficultyData& right);
			friend MyXML::Key& operator<<(MyXML::Key& destKey, const DifficultyData& difficulty);

			///Constructor.
			///Sets up class variables. The values aren't check any how.
			///@param size Intiail size_ value
			///@param depth Intiail depth_ value
			///@param blocksSet Intiail blocksSet_ value
			///@sa Difficulty class.
			DifficultyData(int size, int depth, int blocksSet):
				size_(size), depth_(depth), blocksSet_(blocksSet)	{}
		};

//----------------------------------------------------------------------------

	///Overloaded operator<() for DifficultyData.
	///This operator must be given in order to be able to create a std::map container with
	///DifficultyData objects as keys.<br>
	///It must compare abstract data like structure using some special algorithm.
	///@param left Left-hand argument to compare
	///@param right Right-hand argument to compare
	///@return True if the left object is less than the right
	bool operator<(const DifficultyData& left, const DifficultyData& right);
	///Saves difficulty info in XML key.
	///Saves difficuly info stored in DifficultyData object into MyXML::Key object.
	///The data is saved using this pattern:
	///@verbatim
	/// <difficulty blocksSet="NN" depth="NN" size="NN">
	///@endverbatim
	///where "NN" are the integer values.
	///@param destKey Destination XML Key corresponding to key named "difficulty". The difficulty
	///info will saved as this key's attributes.
	///@param difficulty Difficulty information.
	///@return Reference to destKey key.
	MyXML::Key& operator<<(MyXML::Key& destKey, const DifficultyData& difficulty);

//----------------------------------------------------------------------------

	///Controls over game difficulty level.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///Stores game size's and other settings connected to game difficulty.
	class Difficulty: public DifficultyData
		{
		private:
			///Size of a cuboid at Easy game difficulty level.
			///@sa EASY
			static const int SIZE_EASY = 7;
			///Size of a cuboid at Medium game difficulty level.
			///@sa MEDIUM
			static const int SIZE_MEDIUM = 9;
			///Size of a cuboid at Hard game difficulty level.
			///@sa HARD
			static const int SIZE_HARD = 11;
			///Depth of a cuboid at Easy game difficulty level
			///@sa EASY
			static const int DEPTH_EASY = 11;
			///Depth of a cuboid at Medium game difficulty level
			///@sa EASY
			static const int DEPTH_MEDIUM = 15;
			///Depth of a cuboid at Hard game difficulty level
			///@sa EASY
			static const int DEPTH_HARD = 19;

			///Checks whether the given value is valid.
			///The value is valid when it is inside the given range <minValue; maxValue>.
			///@param value integer value to be checked.
			///@param minValue lower bound of valid range.
			///@param maxValue upper bound of valid range.
			///@return Value after validation. If value matches the given range, it returns value.
			///If it is too small, returns <b>maxValue</b>, if too big - <b>minValue</b>
			///@sa change(), where this method os mostly used.
			static int validate(int value, int minValue, int maxValue);
			
			///Assigns new value to the given variable.
			///Before making and assignment operation, checks the new value using validate() function and
			///assigns the value after validation.
			///@param value Value to be assigned
			///@param newValue value after assignment
			///@param minValue Same as in validate().
			///@param maxValue Same as in validate().
			///@sa validate()
			///@sa void size(int newSize)
			///@sa void depth(int newDepth)
			///@sa void blocksSet(int newBlocksSet)
			///@sa void level(int newLevel)
			void change(int& value, int newValue, int minValue, int maxValue);
			///Game difficulty level.
			///Game size, depth and blocks set influence altogether the game difficulty level (easy,
			///medium, hard). When the size, depth and blocks set do not match any difficulty levels above,
			///the 'Custom' level is choosen.
			///@sa level()
			///@sa findLevel()
			int level_;
			///Tries to find difficulty level matching current size, depth and blocksSet values.
			///If any difficulty level (easy, medium and hard) doesn't match the current values of
			///DifficultyData::size_, DifficultyData::depth_ and DifficultyData::blocksSet_,
			///the "Custom" level is set.
			///@sa level_
			///@sa level()
			void findLevel();
			///XML key storing difficulty information.
			///This key is given in constructor. The difficulty info is read there. Then the user might
			///change the difficulty and in the destructor all the information is saved to the same
			///XML key.
			MyXML::Key& diffData;
			///Changes size, depth and blocks set when changing the difficulty level.
			///When user changes the level using level() method, all size, depth and blocks set values
			///should be updated. This method changes them depending on current level_ value.
			///@sa level(int newLevel)
			///@sa level_
			///@sa validate()
			void changeLevelData();
		public:
			///Minimum size of a cuboid.
			///@sa size(int newSize)
			static const int SIZE_MIN = 5;
			///Maximum size of a cuboid.
			///@sa size(int newSize)
			static const int SIZE_MAXX = 12;
			///Minimum depth of a cuboid.
			///@note Depth values are always odd numbers (9, 11, 13, ...)
			///@sa depth(int newDepth)
			static const int DEPTH_MIN = 9;
			///Maximum depth of a cuboid.
			///@note Depth values are always odd numbers (9, 11, 13, ...)
			///@sa depth(int newDepth)
			static const int DEPTH_MAX = 21;
			///Classic blocks set.
			///Blocks set used at Easy difficulty level. Contains classic 2D Tetris blocks.
			///@sa blocksSet(int newBlocksSet)
			static const int BLOCKS_SET_CLASSIC = 0;
			///Flat blocks set.
			///Blocks set used at Medium difficulty level. Contains some more sophisticated 2D blocks
			///more rare in ordinary Tetris games.
			///@sa blocksSet(int newBlocksSet)
			static const int BLOCKS_SET_FLAT = 1;
			///Extreme blocks set.
			///Blocks set used at Hard difficulty level. True 3D blocks, most difficult to play with.
			///@sa blocksSet(int newBlocksSet)
			static const int BLOCKS_SET_EXTREME = 2;
			///Easy game difficulty level.
			///@sa level(int newLevel)
			static const int EASY = 0;
			///Medium game difficulty level.
			///@sa level(int newLevel)
			static const int MEDIUM = 1;
			///Hard game difficulty level.
			///@sa level(int newLevel)
			static const int HARD = 2;
			///Custom game difficulty level.
			///@sa level(int newLevel)
			static const int CUSTOM = 3;

			///Returns text name of a given blocks set.
			///@par Example:
			///@code
			///cout << '"' << Difficulty::blocksSetStr(Difficulty::EXTREME) << '"' << endl;  //"Extreme"
			///@endcode
			///@param blocksSetNum Blocks set number which name we want to get.
			///@return Text string with the name of a specified blocks set.
			///@sa blocksSetName()
			static const std::string blocksSetStr(int blocksSetNum);
			///Returns text name of a given difficulty level.
			///@par Example:
			///@code
			///cout << '"' << Difficulty::blocksSetStr(Difficulty::MEDIUM) << '"' << endl;  //"Medium"
			///@endcode
			///@param difficultyNum Difficulty level number which name we want to get.
			///@return Text string with the name of a specified difficulty level.
			///@sa levelName()
			static const std::string levelStr(int difficultyNum);

			///Constructor loads difficulty level info from XML data key.
			///@param diffData XML key containing difficulty level info. It is the key: "<CuTe><options>"
			///If the key is empty, some default values will be set (Easy level to be precise).
			Difficulty(MyXML::Key& diffData);
			///Saves back the difficulty level info.
			///Saves the size, depth and blocks set information in the same XML key (diffData) which
			///was given in constructor.
			///@sa diffData
			~Difficulty()	{diffData << *this;}
			///Reads current game cuboid size.
			///@return Size (width and height) of a game cuboid.
			///@sa Use size(int newSize) to change the current size.
			///@sa DifficultyData::size_
			int size() const	{return size_;}
			///Sets new game cuboid size.
			///@param newSize New game cuboid size; value is validated using validate() method before
			///assignignment.
			///@sa size()
			///@sa validate()
			///@sa DifficultyData::size_
			void size(int newSize)	{change(size_, newSize, SIZE_MIN, SIZE_MAXX);}
			///Reads current game cuboid depth.
			///@return Depth of a game cuboid.
			///@sa Use depth(int newDepth) to change the current depth.
			///@sa DifficultyData::depth_
			int depth() const	{return depth_;}
			///Sets new game cuboid depth.
			///@param newDepth New game cuboid depth; value is validated using validate() method before
			///assignignment.
			///@sa depth()
			///@sa validate()
			///@sa DifficultyData::depth_
			void depth(int newDepth)	{change(depth_, newDepth, DEPTH_MIN, DEPTH_MAX);}
			///Reads current game blocks set.
			///@return Blocks set used in a game.
			///@sa Use blocksSet(int newBlocksSet) to change the current blocks set.
			///@sa DifficultyData::blocksSet_
			int blocksSet() const	{return blocksSet_;}
			///Sets new game blocks set.
			///@param newBlocksSet New game blocks set; value is validated using validate() method before
			///assignignment.
			///@sa blocksSet()
			///@sa validate()
			///@sa DifficultyData::blocksSet_
			void blocksSet(int newBlocksSet)
				{change(blocksSet_, newBlocksSet, BLOCKS_SET_CLASSIC, BLOCKS_SET_EXTREME);}
			///Reads current game difficulty level.
			///@return Difficulty level of a game.
			///@sa Use level(int newLevel) to change the current difficulty level.
			///@sa level_
			int level() const	{return level_;}
			///Sets new game difficulty level.
			///@param newLevel New game difficulty level; value is validated using validate() method
			///before assignignment.
			///@sa level()
			///@sa validate()
			///@sa level_
			void level(int newLevel);
			///Returns text name of a current blocks set.
			///@return Text string with the name of a current blocks set.
			///@sa blocksSetStr()
			const std::string blocksSetName() const	{return blocksSetStr(blocksSet_);}
			///Returns text name of a current difficulty level.
			///@return Text string with the name of a current difficulty level.
			///@sa levelStr()
			const std::string levelName() const	{return levelStr(level_);}
		};

//----------------------------------------------------------------------------

	///Overloaded operator<<() for printing the difficulty level info.
	///Sends to the specified output stream difficulty info in formar:
	///@verbatim
	///SIZExSIZExDEPTH @ BLOCKS_SET_NAME
	///@endverbatim
	///Where SIZE, DEPTH, and BLOCKS_SET_NAME are replaced with proper number and texts.
	///@param os Desired output stream (e.g. cout)
	///@param difficulty Difficulty object to be printed out
	///@return same stream as os.
	std::ostream& operator<<(std::ostream& os, const Difficulty& difficulty);

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif

//----------------------------------------------------------------------------
