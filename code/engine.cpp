//----------------------------------------------------------------------------

///@file
///CuTe game engine class definition
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

#include "MyXML/myxml.h"
#include "engine.h"
using namespace std;
using namespace CuTe;
using boost::lexical_cast;

//----------------------------------------------------------------------------

///Special algorithm to replace four values.
///You can switch values in four different variables as follows:
///e1 <- e2 <- e3 <- e4 <- e1 (if right = false) or
///e1 -> e2 -> e3 -> e4 -> e1 (if right = true);
///where x <- y means "x i set to y".
///This algorithm is used during rotation, where four points are moved simultaneously.
///@param e1 First value
///@param e2 Second value
///@param e3 Third value
///@param e4 Fourth value
///@param right See above
///@sa Block::rotateX()
///@sa Block::rotateY()
///@sa Block::rotateZ()
template<typename T>
void shift4val(T &e1, T &e2, T &e3, T &e4, bool right = true)
	{
	const T temp = e1;
	if(right)
		{
		e1 = e4;
		e4 = e3;
		e3 = e2;
		e2 = temp;
		}
	else
		{
		e1 = e2;
		e2 = e3;
		e3 = e4;
		e4 = temp;
		}
	}

//----------------------------------------------------------------------------

Block::Block(const MyXML::Key& blockData, const Engine& parent)
	{
	int x, y, z;
	for(x = 0; x < 5; ++x)			//initialize the 3D blockCubes array
		for(y = 0; y < 5; ++y)
			for(z = 0; z < 5; ++z)
				blockCubes[x][y][z] = false;
	size_ = lexical_cast<int>(blockData.attribute("size"));
	range_ = size_ / 2;
	pos_.x() = pos_.y() = parent.size() / 2;
	pos_.z() = parent.depth() - 1;
	//get all the <xdata> keys by saving iterator pointing to the first key
	//should be SIZE*SIZE <xdata> keys inside every "block" key
	MyXML::KeysMap::const_iterator xData = blockData.keys("xdata").first;
	for(z = range_; z >= -range_; --z)
		for(y = range_; y >= -range_; --y, ++xData)
			for(x = 0; x < size_; ++x)
				blockCubes[x + 2 - range_][y + 2][z + 2] = xData->second.value()[x] == 'X';
	}

bool Block::operator()(int x, int y, int z) const
	{
	if((abs(x) > 2) || (abs(y) > 2) || (abs(z) > 2))
		throw CuTeEx("Block coordinates are out of range");
	return blockCubes[2 + x][2 + y][2 + z];
	}

void Block::rotateX(bool CCW)
	{
	int x;
	if(range_ > 0)		//block is at least 3x3x3
		for(x = 2 - range_; x <= 2 + range_; ++x)
			{
			shift4val(blockCubes[x][3][3], blockCubes[x][1][3], blockCubes[x][1][1], blockCubes[x][3][1], CCW);
			shift4val(blockCubes[x][3][2], blockCubes[x][2][3], blockCubes[x][1][2], blockCubes[x][2][1], CCW);
			}
		if(range_ > 1)		//block's size is 5x5x5
			for(x = 2 - range_; x <= 2 + range_; ++x)
				{
				shift4val(blockCubes[x][4][4], blockCubes[x][0][4], blockCubes[x][0][0], blockCubes[x][4][0], CCW);
				shift4val(blockCubes[x][3][4], blockCubes[x][0][3], blockCubes[x][1][0], blockCubes[x][4][1], CCW);
				shift4val(blockCubes[x][2][4], blockCubes[x][0][2], blockCubes[x][2][0], blockCubes[x][4][2], CCW);
				shift4val(blockCubes[x][1][4], blockCubes[x][0][1], blockCubes[x][3][0], blockCubes[x][4][3], CCW);
				}
	}

void Block::rotateY(bool CCW)
	{
	int y;
	if(range_ > 0)		//block is at least 3x3x3x
		for(y = 2 - range_; y <= 2 + range_; ++y)
			{
			shift4val(blockCubes[3][y][3], blockCubes[3][y][1], blockCubes[1][y][1], blockCubes[1][y][3], CCW);
			shift4val(blockCubes[3][y][2], blockCubes[2][y][1], blockCubes[1][y][2], blockCubes[2][y][3], CCW);
			}
		if(range_ > 1)		//block's size is 5x5x5
			for(y = 2 - range_; y <= 2 + range_; ++y)
				{
				shift4val(blockCubes[4][y][0], blockCubes[0][y][0], blockCubes[0][y][4], blockCubes[4][y][4], CCW);
				shift4val(blockCubes[4][y][1], blockCubes[1][y][0], blockCubes[0][y][3], blockCubes[3][y][4], CCW);
				shift4val(blockCubes[4][y][2], blockCubes[2][y][0], blockCubes[0][y][2], blockCubes[2][y][4], CCW);
				shift4val(blockCubes[4][y][3], blockCubes[3][y][0], blockCubes[0][y][1], blockCubes[1][y][4], CCW);
				}
	}

void Block::rotateZ(bool CCW)
	{
	int z;
	if(range_ > 0)		//block is at least 3x3x3x
		{
		for(z = 2 - range_; z <= 2 + range_; ++z)
			{
			shift4val(blockCubes[3][3][z], blockCubes[1][3][z], blockCubes[1][1][z], blockCubes[3][1][z], CCW);
			shift4val(blockCubes[2][3][z], blockCubes[1][2][z], blockCubes[2][1][z], blockCubes[3][2][z], CCW);
			}
		if(range_ > 1)		//block's size is 5x5x5
			for(z = 2 - range_; z <= 2 + range_; ++z)
				{
				shift4val(blockCubes[4][4][z], blockCubes[0][4][z], blockCubes[0][0][z], blockCubes[4][0][z], CCW);
				shift4val(blockCubes[3][4][z], blockCubes[0][3][z], blockCubes[1][0][z], blockCubes[4][1][z], CCW);
				shift4val(blockCubes[2][4][z], blockCubes[0][2][z], blockCubes[2][0][z], blockCubes[4][2][z], CCW);
				shift4val(blockCubes[1][4][z], blockCubes[0][1][z], blockCubes[3][0][z], blockCubes[4][3][z], CCW);
				}
		}
	}

//----------------------------------------------------------------------------

void Engine::Points::addNewBlock(const Block &block)
	{
	int cubes = 0;
	for(int x = -block.range(); x <= block.range(); ++x)
		for(int y = -block.range(); y <= block.range(); ++y)
			for(int z = -block.range(); z <= block.range(); ++z)
				if(block(x, y, z))
					++cubes;
	points += (cubes * 3 - 2) * multiplier;
	}

void Engine::Points::cheat()
	{
	if(points <= CHEAT_MIN_POINTS)
		points = 0;
	else
		if(points <= 2 * CHEAT_MIN_POINTS)
			points -= CHEAT_MIN_POINTS;
		else
			points /= 2;
	}

//----------------------------------------------------------------------------

Engine::Engine(const Difficulty& difficulty):
	cuboid(boost::extents[difficulty.size() + 2 * WALL_THICKNESS]
		[difficulty.size() + 2 * WALL_THICKNESS]
		[difficulty.depth() + 2 * WALL_THICKNESS]), removedPlanes(difficulty.depth() + 1),
	points_(difficulty.size()), size_(difficulty.size()), depth_(difficulty.depth())
	{
	cuboid.reindex(-WALL_THICKNESS);		//start all indexes from -WALL_THICKNESS
	loadBlocks(difficulty.blocksSet());
	for(int z = -WALL_THICKNESS; z < depth_ + WALL_THICKNESS; ++z)
		for(int y = -WALL_THICKNESS; y < size_ + WALL_THICKNESS; ++y)
			for(int x = -WALL_THICKNESS; x < size_ + WALL_THICKNESS; ++x)
				cuboid[x][y][z] = (x < 0) || (x >= size_) || (y < 0) || (y >= size_) || (z < 0) || (z >= depth_);
	current = getRandomBlock();
	for(int z = 0; z <= current.range(); ++z, --current.pos().z())
		if(canPut(current))		//move block forward as much, as it is needed to put it on a cuboid
			break;
	next = getRandomBlock();
	}

void Engine::loadBlocks(int blocksSet)
	{
	MyXML::Key blocksData("data/blocks.xml");		//read the data.xml's blocks data
	std::pair<MyXML::KeysMap::const_iterator, MyXML::KeysMap::const_iterator> blocksRange =
		blocksData.keys("block");		//find all "block" keys
	for(MyXML::KeysMap::const_iterator i = blocksRange.first; i != blocksRange.second; ++i)
		//load block only if its set is less or equal the choosen one
		if(lexical_cast<int>(i->second.attribute("set")) <= blocksSet)
			blocks.push_back(new Block(i->second, *this));		//save created Block object
	}

bool Engine::operator()(int x, int y, int z) const
	{
	if((x >= size_ + WALL_THICKNESS) || (x <= -WALL_THICKNESS) ||
		(y >= size_ + WALL_THICKNESS) || (y <= -WALL_THICKNESS) ||
		(z >= depth_ + WALL_THICKNESS) || (z <= -WALL_THICKNESS))
			throw CuTeEx("Coordinates in cuboid are out of range");
	return cuboid[x][y][z];
	}

bool Engine::canPut(const Block &block) const
	{
	for(int x = -block.range(); x <= block.range(); ++x)
		for(int y = -block.range(); y <= block.range(); ++y)
			for(int z = -block.range(); z <= block.range(); ++z)
				if(block(x, y, z) && cuboid[x + block.pos().x()][y + block.pos().y()][z + block.pos().z()])
					return false;		//collision: there's a cube in a block and in a cuboid
	return true;
	}

bool Engine::filledPlane(int z)
	{
	for(int x = 0; x < size(); ++x)
		for(int y = 0; y < size(); ++y)
			if(!cuboid[x][y][z])
				return false;
	return true;
	}

void Engine::removeFilledPlanes()
	{
	int x, y, z;
	for(z = 0; z <= depth_; ++z)
		removedPlanes[z] = filledPlane(z);		//mark planes to be removed
	z = 0;
	while(!removedPlanes[z])
		++z;		//first filled plane (may be the end, which means there were no filled planes found)
	int moved = 0;		//how many plames should move back
	while(z < depth_)
		{
		while(removedPlanes[z])		//find first plane to be moved
			{
			++z;
			++moved;
			}
		int last = z + 1;		//last - plane after the last to be moved
		while(!removedPlanes[last])
			++last;
		for(; z < last; ++z)
			for(y = 0; y < size_; ++y)
				for(x = 0; x < size_; ++x)
					cuboid[x][y][z - moved] = cuboid[x][y][z];		//move cubes back
		}
	if(moved > 0)		//do any moves only if some planes were actually removed
		{
		for(z = depth_ - moved; z < depth_; ++z)		//clear the most top planes if some planes were removed
			for(y = 0; y < size_; ++y)
				for(x = 0; x < size_; ++x)
					cuboid[x][y][z] = false;
		points_.addFilledPlanes(moved);		//moved stores the number of removed planes
		if(empty())
			points_.addBonus();		//add bonus points if whole cuboid empty
		}
	}

void Engine::switchBlocks()
	{
	int x, y, z;
	points_.addNewBlock(current);		//add points for current block
	for(z = -current.range(); z <= current.range(); ++z)
		for(y = -current.range(); y <= current.range(); ++y)
			for(x = -current.range(); x <= current.range(); ++x)
				if(current(x, y, z))		//saves a current block on a cuboid
					cuboid[current.pos().x() + x][current.pos().y() + y][current.pos().z() + z] = true;
	removeFilledPlanes();		//if some Z plane is filled with cubes, remove it
	current = next;
	next = getRandomBlock();
	for(z = 0; z <= current.range(); ++z, --current.pos().z())
		if(canPut(current))		//move block forward as much, as it is needed to put it on a cuboid
			return;
	gameOver();		//new block can't be put on the cuboid, game is overed
	}

bool Engine::move(int shiftX, int shiftY)
	{
	current.pos().y() += shiftY;		//temporarily move block up
	current.pos().x() += shiftX;
	if(!canPut(current))
		{
		current.pos().y() -= shiftY;		//block can't be placed uppper
		current.pos().x() -= shiftX;
		return false;
		}
	return true;
	}

bool Engine::moveForward()
	{
	--current.pos().z();		//temporarily move block further to the screen
	if(canPut(current))
		return true;
	++current.pos().z();		//block can't be placed further
	switchBlocks();		//so it is saved on a cuboid
	return false;
	}
bool Engine::tryMove(Block &block)
	{
	int shift;		//how many fields did the block was moved to finish rotation
	for(shift = 0; block.pos().x() - block.range() < 0; ++shift)
		{		//too close to the left wall
		++block.pos().x();
		if(canPut(block))
			return true;
		}
	block.pos().x() -= shift;		//still can't rotate, move back to start position
	for(shift = 0; block.pos().x() + block.range() > size_ - 1; ++shift)
		{		//too close to the left wall
		--block.pos().x();
		if(canPut(block))
			return true;
		}
	block.pos().x() += shift;
	for(shift = 0; block.pos().y() - block.range() < 0; ++shift)
		{		//too close to the floor
		++block.pos().y();
		if(canPut(block))
			return true;
		}
	block.pos().y() -= shift;
	for(shift = 0; block.pos().y() + block.range() > size_ - 1; ++shift)
		{		//too close to the ceiling
		--block.pos().y();
		if(canPut(block))
			return true;
		}
	block.pos().y() += shift;
	for(shift = 0; block.pos().z() + block.range() > depth_ - 1; ++shift)
		{
		--block.pos().z();
		if(canPut(block))
			return true;
		}
	block.pos().z() += shift;
	return false;
	}

bool Engine::tryPut(Block &block)
	{
	if(canPut(block) || tryMove(block))
		{		//block can be placed on a cuboid after rotation (and optionally after additional move)
		current = block;		//replace current cube with temporary
		return true;
		}
	else
		return false;
	}

bool Engine::rotateXCW()
	{
	Block temp = current;		//creates temporary copy of a current block
	temp.rotateX(false);		//rotates a copy
	return tryPut(temp);		//try to put onto the cuboid
	}

bool Engine::rotateXCCW()
	{
	Block temp = current;		//creates temporary copy of a current block
	temp.rotateX(true);		//rotates a copy
	return tryPut(temp);		//try to put onto the cuboid
	}

bool Engine::rotateYCW()
	{
	Block temp = current;		//creates temporary copy of a current block
	temp.rotateY(false);		//rotates a copy
	return tryPut(temp);		//try to put onto the cuboid
	}

bool Engine::rotateYCCW()
	{
	Block temp = current;		//creates temporary copy of a current block
	temp.rotateY(true);		//rotates a copy
	return tryPut(temp);		//try to put onto the cuboid
	}

bool Engine::rotateZCW()
	{
	Block temp = current;		//creates temporary copy of a current block
	temp.rotateZ(false);		//rotates a copy
	return tryPut(temp);		//try to put onto the cuboid
	}

bool Engine::rotateZCCW()
	{
	Block temp = current;		//creates temporary copy of a current block
	temp.rotateZ(true);		//rotates a copy
	return tryPut(temp);		//try to put onto the cuboid
	}

int Engine::distance()
	{
	int dist = 0;
	--current.pos().z();
	while(canPut(current))
		{
		--current.pos().z();		//move block forward as far as it is possible
		++dist;
		}
	current.pos().z() += dist + 1;		//move block back to its start position
	return dist;
	}

bool Engine::empty() const
	{
	for(int x = 0; x < size(); ++x)
		for(int y = 0; y < size(); ++y)
			for(int z = 0; z < depth(); ++z)
				if(cuboid[x][y][z])
					return false;		//return false if any cubes were found
	return true;
	}

//----------------------------------------------------------------------------
