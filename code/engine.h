//----------------------------------------------------------------------------

///@file
///CuTe game engine.
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

#ifndef ENGINE_H
#define ENGINE_H

//----------------------------------------------------------------------------

#include <vector>
#include <boost/multi_array.hpp>
#include "common.h"
#include "point.h"
#include "difficulty.h"

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///Exception class for CuTe game
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///This class is thrown when some logic erros occur while using CuTe classes
	class CuTeEx: public std::logic_error
		{
		public:
			///Constructor.
			///Sends specified message to its base class logic_error
			///@param s message which will be available in exception from what() method
			CuTeEx(const std::string &s): std::logic_error(s)	{}
		};

//----------------------------------------------------------------------------

	class Engine;		//forward declaration

	///All data and actions connected to blocks.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///In this class there are information about size of a block, particular cubes which the block
	///is built from and the position of a block in a cuboid.
	///Besides the class deliver essential method for rotating and moving block block.
	class Block
		{
		private:
			///All cubes which the block is built in.
			///@note blockCubes[2][2][2] is just in the middle of a block
			///@sa operator()(int x, int y, int z) const
			bool blockCubes[5][5][5];
			///Size of a block.
			///Can be one of: 1, 3, 5 (must be an odd number)
			int size_;
			///The range of cubes in a block.
			///Although the cubes data is a simple array, operator() uses non-typical array indexes
			///<-range; range>. Thanks to that cube (0, 0, 0) is right in the middle (no corner)
			///@sa operator()(int x, int y, int z) const
			int range_;
			///Position of a block in a game cuboid.
			Point<int, 3> pos_;
		public:
			///Default constructor.
			///Sometimes the Block object must be created without loading some actual block data onto it.
			///This can be done using the default constructor but remember that object initialized in such
			///a way is completly useless and not valid.
			///@par
			///You can always check whether the Block object is "empty" (default constructed): the size()
			///method will return zero (in properly constructed object this should be at leat 1).
			///@sa Block(const MyXML::Key& blockData, const Engine& parent);
			Block(): size_(0), range_(0)	{}
			///Normal object constructor.
			///This constructor creates a Block object and loads the block data from a XML key given as
			///a parameter.
			///@param blockData XML key containing an information about the block size, set and most
			///important a set of "xdata" keys with coded block information (cubes).
			///@param parent Parent game engine object, needed to obtain the information about the game
			///size and depth (the block is positioned properly after initialization).
			///@sa Engine::loadBlocks() for the details about blockData key structure.
			Block(const MyXML::Key& blockData, const Engine& parent);
			///Returns size of a block
			///@return Size of a block
			///@sa size_
			int size() const	{return size_;}
			///Returns range of block cubes
			///@return Range of block cubes
			///@sa range_ for more information
			int range() const	{return range_;}
			///Overloaded operator ().
			///You can read whether at specified field in block there is or isn't a cube.
			///Just type b(0, 1, -2), where b is an object of type Block.
			///@param x X coordinate of field in block. Can be in range <-range; range>
			///@param y Y coordinate of field in block. Can be in range <-range; range>
			///@param z Z coordinate of field in block. Can be in range <-range; range>
			///@return True if there is a cube on specified field, otherwise false
			///@warning For great code simplifying I was forced to sacrifice current block range checking.
			///It does not mean that you can require any coordinates you want (exception
			///will be thrown when the absolute value of a coordinate exceed 2), but you are able to
			///read every cube in a block no matter how big the block is (exceed range value).
			bool operator()(int x, int y, int z) const;
			///Returns current block position.
			///This function allows only to read the block position.
			///@return (x, y, z) position of a block (cube located right in the middle)
			///@sa pos()
			const Point<int, 3>& pos() const	{return pos_;}
			///Returns current block position.
			///This function allows to read and change the block position.
			///@return (x, y, z) position of a block (cube located right in the middle)
			///@sa pos() const
			Point<int, 3>& pos()	{return pos_;}
			///Rotates block around X axis.
			///@param CCW If true, rotates counterclockwise, otherwise clockwise
			///@sa Engine::rotateXCW()
			///@sa Engine::rotateXCCW()
			void rotateX(bool CCW);
			///Rotates block around Y axis.
			///@param CCW If true, rotates counterclockwise, otherwise clockwise
			///@sa Engine::rotateYCW()
			///@sa Engine::rotateYCCW()
			void rotateY(bool CCW);
			///Rotates block around Z axis.
			///@param CCW If true, rotates counterclockwise, otherwise clockwise
			///@sa Engine::rotateZCW()
			///@sa Engine::rotateZCCW()
			void rotateZ(bool CCW);
		};

//----------------------------------------------------------------------------

	///Essential data for CuTe game engine.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///In this class there are stored cuboid and blocks data.
	///Some most important method are here: rotation, ability to put block in specific place,
	///loading blocks from file, etc.
	class Engine
		{
		private:

			///Point counter classes for CuTe.
			///This class counts points which the user have achieved in the game so far.
			///It encapsulates the points adding routins, so that the environment class doesn't have to
			///take care about them.
			class Points
				{
				private:
					///Minimum amout of points to take away when cheating.
					///When user cheats during the game, game takes him away half of his points, but it must
					///be at least CHEAT_MIN_POINTS.
					///@sa cheat()
					static const int CHEAT_MIN_POINTS = 10000;
					///Width and height of the associated CuTe cuboid.
					const int GAME_SIZE;
					///Points count (0 at the beginning)
					int points;
					///Points multiplier.
					///All added points will be multiplied by this valie.
					///@par
					///This value default to 1, which means to multiplication at all.
					///@sa mul() for more details.
					int multiplier;
				public:
					///Constructor.
					///@param gameSize Width and height of a game.
					///@sa GAME_SIZE
					Points(int gameSize): GAME_SIZE(gameSize), points(0), multiplier(1)	{}
					///Adds points if some plane(s) were removed.
					///@param planesCount How many planes were removed at the same time; the more planes
					///removed, the bigger point bonus player will achieve.
					void addFilledPlanes(int planesCount)
						{points += sqr(GAME_SIZE) * GAME_SIZE * sqr(planesCount) * multiplier;}
					///Adds some points after block was saved on a cuboid.
					///The bigger the block was (was built from more cubes), the more point will be added.
					///@param block Block which you want to count and add points.
					void addNewBlock(const Block &block);
					///Bonus points for cleaning whole cuboid.
					///When after removing filled planes there are no cubes on cuboid left at all,
					///you'll get a bonus points.
					void addBonus()	{points += sqr(sqr(GAME_SIZE)) * multiplier;}
					///Overloaded conversion perator int().
					///Thanks to this operator you may read points from Points object really simple.
					operator int() const	{return points;}
					///Sets the points multiplier.
					///This feature can be used somewhere in your game to multiply all points which user
					///got by some specified value. I use it to multiply all the points by the speed level
					/// - so that the bigger speed, the more points user get.
					///@sa multiplier.
					void mul(int newMultiplier)	{multiplier = newMultiplier;}
					///Take away some points when user cheats.
					///When user cheats during the game, game takes him away half of points which he got so far,
					///but it must be at least CHEAT_MIN_POINTS.
					///@sa CHEAT_MIN_POINTS
					void cheat();
				};

			///Thickness of a game cuboid walls.
			///This additional wall is invisible for the user, but is used when checking if the block can be
			///put in near the wall. The wall is simply treated as a solid bunch of cubes so that canPut()
			///method has simplier work: it only checks for cubes collisions since walls are cubes too.
			///@sa cuboid
			static const int WALL_THICKNESS = 2;
			///3D matrix corresponding to main game cuboid.
			///If on location (x, y, z) there is true, it means that there is a cube
			///(only solid cubes are stored here, not the cubes which are a part of currently visible Block).
			///@note (x, y) = (0, 0) is the down left corner in a cuboid Z plane. Greater z, nearer the user we are
			///(z = 0 is the furthest Z plane)
			///@sa operator()(int x, int y, int z)
			boost::multi_array<bool, 3> cuboid;
			///Table of Z coordinates of planes, which were removed lastly.
			///Z coords of all Z planes which were removed are stored here. If removedPlanes[x] = true
			///than Z plane x was removed during last call to removeFilledPlanes.
			///Information from this array can be read by removedPlane(int z)
			///@sa removeFilledPlanes()
			///@sa removedPlane(int z)
			std::vector<bool> removedPlanes;
			///Points counter object.
			///@sa Engine::Points
			Points points_;
			///Current block data.
			///This is the block which is currently visible for the user.
			///@sa Block
			///@sa next
			Block current;
			///Next block data.
			///This block will take place of current, when the second one will be saved on cuboid.
			///@sa current
			Block next;
			///List of blocks which are available for player.
			///The blocks data is loaded earlier. This list may vary depending on how difficult blocks set
			///have the player choosen.
			///@sa loadBlocks()
			std::vector<const Block*> blocks;
			///Loads blocks data from the XML data source.
			///This method loads all blocks data (sizes and position of block cubes) from external
			///XML source (file or key).
			///@par XML block data format
			///Inside "blocks" key there are several nested "block" keys. Each block key has this
			///attributes:
			///@verbatim
			/// <block set="N" size="N">
			///@endverbatim
			///set is the block set (see above) and size is the size (width, height and depth) of block
			///space (block itself might be smaller.<br>
			///Inside this tag there are 'size' * 'size' lines of "xdata" keys, each of which has a
			///series of dots or X's (there are 'size' of those symbol). Each "xdata" key
			///corresponds to one line parallel to X axis (with constant Y and Z values). Every line
			///is one (y, z) line.<br>
			///X mean that on this position there is a cube. Everything else is an empty place.
			///@param blocksSet Blocks set to load. Blocks set determinates the difficulty of blocks - 
			///the higher the number, the more difficult the blocks. This parameters limits loading
			///blocks only to those, which set is less or equal than specified.
			void loadBlocks(int blocksSet);
			///Returns random block from available ones
			///@return reference to a randomly choosen block from blocks list
			///@sa current
			///@sa next
			///@sa blocks
			const Block &getRandomBlock() const	{return *blocks[rand() % blocks.size()];}
			///Is the specified Z plane fully filled by cubes.
			///@param z Z plane which you want to check.
			///@return True if specified plane is fully filled with cubes. Otherwise false.
			///@sa removeFilledPlanes()
			bool filledPlane(int z);
			///Checks whether the whole cuboid is empty.
			///This function is called when some planes were removed - after that it might happen
			///that no cubes left on cuboid - bonus points should be added then.
			///@return True if no cubes left on a cuboid. Otherwise false.
			///@sa removeFilledPlanes()
			bool empty() const;
			///Tries to put given block on a cuboid.
			///Checks whether the given block can be put on a cuboid. If it can't, tries to move it
			///using tryMove(). This function is called by every public rotate*() functions.
			///@param block Block to be checked.
			///@return True if block can be put on cuboid (maybe after additional moving), otherwise
			///false.
			///@sa canPut() and tryMove()
			///@sa rotateXCW(), rotateXCCW(), rotateYCW(), rotateYCCW(), rotateZCW(), rotateZCCW()
			bool tryPut(Block &block);
			///Internal moving procedure used to simplify moving code.
			///This function moves the block in any direction and is called by all move*() methods.
			///@param shiftX How many units to move the block horizontally (e.g. -1 is left)
			///@param shiftY How many units to move the block vertically (e.g. 1 is up)
			///@return True if block was moved left succesfully, otherwise (there was a collision after move)
			///returns false.
			bool move(int shiftX, int shiftY);
			///Size of cuboid.
			///This is the width and height of a cuboid. Cuboid positions can be from <0;size - 1>,
			///however because of inivisible walls, this range is widen to <-WALL_THICKNESS; size + WALL_THICKNESS - 2>
			///@sa size()
			///@sa depth_
			///@sa WALL_THICKNESS
			const int size_;
			///Depth of a cuboid.
			///Depth can be in range <0, depth_ - 1>, but because of invisible additional walls, this is
			///actually <-WALL_THICKNESS, depth_ + WALL_THICKNESS - 1>
			///@sa depth()
			///@sa size_
			///@sa WALL_THICKNESS
			const int depth_;
		protected:
			///Check which Z planes are filled and removes them.
			///Method sets the removedPlanes array and moves those not filled block, which were higher
			///filled ones.
			///@sa removedPlanes
			///@sa switchBlocks()
			///@sa filledPlane(int z)
			virtual void removeFilledPlanes();
			///Tries to move block if it is close to the walls and can't be rotated because of that.
			///If block is too close to the wall it might happen that after rotation some cubes in a block
			///will collide with walls - in this situation rotation can't be finished.
			///This function tries to move the block further to the wall, so that it might be rotated.
			///@param block Block which we want to move. This is the block <b>after</b> rotation. Its position
			///will be changed, if the tryMove() function succesfully found new position further to the wall.
			///Otherwise it will remain in place.
			///@return True if function found a new non-colliding place for a block (block is set on this
			///position). Otherwise false.
			///@sa Block::rotateX()
			///@sa Block::rotateY()
			///@sa Block::rotateZ()
			virtual bool tryMove(Block &block);
			///Replaces current block with next one and picks up next randomly.
			///After call to moveForward() it might happen, that block can't be pushed further the screen and
			///current block must be saved on its position. After that next block becomes current, and next
			///is set to a randomly generated next one.
			///But after that there is possible, that some plane(s) is fully filled with cubes and should be
			///removed from cuboid. This job is done by removeFilledPlanes()
			///@sa moveForward()
			///@sa removeFilledPlanes()
			virtual void switchBlocks();
			///Counts the distance between current block and a cubes in cuboid.
			///The distance is the lowest distance between any cube in a block and a corresponding
			///cube under him located in the cuboid. In other ways it is the number how many you should
			///move your block forward (minus one) to save your block on a cuboid.
			///@par
			///This value is needed to calculate the present alpha of a current block (it depends on
			///that distance).
			///@sa countAlpha()
			virtual int distance();
			///Run when the game is overed.
			///Create your own overriden version of gameOver() in derived class. It will be called
			///when the Engine class found the game is over.
			virtual void gameOver() = 0;
		public:
			///Constructor.
			///@param difficulty Stores information about the game difficulty, which are the game cuboid
			///size, depth and the desired blocksSet. All this information is used when creating game
			///cuboid data.
			Engine(const Difficulty& difficulty);
			///Destructor.
			///Releases memory of blocks, allocated in loadBlocks()
			///@sa loadBlocks()
			virtual ~Engine()	{for_each(blocks.begin(), blocks.end(), deleter<const Block>);}
			///Returns the game cuboid size.
			///@return Size of the game cuboid.
			///@sa size_
			int size()	const {return size_;}
			///Returns the game cuboid depth.
			///@return Depth of the game cuboid.
			///@sa depth_
			int depth()	const {return depth_;}
			///Reading cuboid data.
			///This function returns the cuboid data to environment.
			///@param x X coordinate which of cube which we want to read. Can be in range
			///(-WALL_THICKNESS; size_ + WALL_THICKNESS) (wall data <b>is</b> available for users).
			///@param y Y coordinate; see x for more details and range.
			///@param z Z coordinate; Can be in range <0;depth_ - 1>
			///@return True if on specified (x, y, z) position there is a cube. If field is empty
			///return false
			virtual bool operator()(int x, int y, int z) const;
			///Checkes whether specified Z plane was removed in last move.
			///If removedPlane(z) = true it means that z-th plane was removed (was fully filled)
			///in last call to moveForward()
			///@sa removedPlanes
			///@sa moveForward()
			virtual bool removedPlane(int z) const	{return removedPlanes[z];}
			///Returns a current block.
			///@return const reference to a current block. Thanks to that environment can read current block data
			///(for example for drawing) but it can't change it.
			///@sa nextBlock()
			virtual const Block &currentBlock() const	{return current;}
			///Returns a next block.
			///@return const reference to a next block.
			///@sa currentBlock()
			virtual const Block &nextBlock() const	{return next;}
			///Tries to move current block left.
			///@return True if block was moved left succesfully, otherwise (there was a collision after move)
			///returns false.
			virtual bool moveLeft()	{return move(-1, 0);}
			///Tries to move current block right.
			///@return True if block was moved left succesfully, otherwise (there was a collision after move)
			///returns false.
			virtual bool moveRight()	{return move(1, 0);}
			///Tries to move current block up.
			///@return True if block was moved left succesfully, otherwise (there was a collision after move)
			///returns false.
			virtual bool moveUp()	{return move(0, 1);}
			///Tries to move current block down.
			///@return True if block was moved left succesfully, otherwise (there was a collision after move)
			///returns false.
			virtual bool moveDown()	{return move(0, -1);}
			///Tries to move current block forward (deeper the screen).
			///This function may be called in two situations: when player wants to manually spped-up the
			///game by forcing the block more forward, or (more often) by the game itself.
			///If the block can't be moved deeper the screen, it means that it found some obstacles on its
			///way and should be saved on a cuboid -> the switchBlocks() method is called.
			///@return True if block was moved forward succesfully. False means that the block was
			/// saved on cuboid.
			///@sa switchBlocks()
			virtual bool moveForward();
			///Rotates current block around X axis clockwise
			///@sa Block::rotateX()
			virtual bool rotateXCW();
			///Rotates current block around X axis counterclockwise
			///@sa Block::rotateX()
			virtual bool rotateXCCW();
			///Rotates current block around Y axis clockwise
			///@sa Block::rotateY()
			virtual bool rotateYCW();
			///Rotates current block around Y axis counterclockwise
			///@sa Block::rotateY()
			virtual bool rotateYCCW();
			///Rotates current block around Z axis clockwise
			///@sa Block::rotateZ()
			virtual bool rotateZCW();
			///Rotates current block around Z axis counterclockwise
			///@sa Block::rotateZ()
			virtual bool rotateZCCW();
			///Game points counter.
			///@return Points object reference to allow user read the points using operator int().
			virtual Points& points() {return points_;}
			///Changes points multiplier.
			///@sa Points::mul()
			virtual void pointsMul(int newMul)	{points_.mul(newMul);}
			///Checkes whether block can be put on cuboid.
			///Very important function can be used in two situations: when player wants to rotate block
			///(rotation is impossible if after this action block would collide with other cubes),
			///or when game automatically moves block to the bottom (lower Z coord) (if this move is
			///impossible it means that block reached cubes on cuboid and should be saved on it.
			///@param block Block which should be checked
			///@return True if specified block can be put in the cuboid without collision
			///@sa loadBlocks()
			bool canPut(const Block &block) const;
	};		//class Engine

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif		//#define ENGINE_H

//----------------------------------------------------------------------------
