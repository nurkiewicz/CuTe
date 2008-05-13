//----------------------------------------------------------------------------

///@file
///BlockAnalyzer class definitions.
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

#include "blockanalyzer.h"
using namespace CuTe;
using boost::lexical_cast;
using std::string;

//----------------------------------------------------------------------------

const string BlockAnalyzer::BlockPos::rotCodes[] = {
		"", "x", "XX", "X", "yZ", "y", "yz", "zzY", "zz", "YYX", "YY", "YYx",
		"Zy", "ZYY", "ZY", "Z", "YZ", "Y", "Yz", "Yzz", "zy", "zyy", "zY", "z"};

//----------------------------------------------------------------------------

BlockAnalyzer::BlockAnalyzer(EngineExt& iParent, bool startImmediately):
	cuboidHeights(boost::extents[iParent.size() + 2][iParent.size() + 2]), state_(IDLE), parent(iParent)
	{
	if(startImmediately)
		startProcess();
	}

void BlockAnalyzer::startProcess()
	{
	block = parent.currentBlock();
	block.pos().z() = parent.depth() - 1 - 2;		//move the block forward to not collide with top wall
	best_.reset();		//reset the best block data
	rotation = 0;
	state(PROCESSING);
	countCuboidHeights();		//count heights of every (x, y) position on cuboid
	process();		//process all possible block combinations
	}

void BlockAnalyzer::process()
	{
	//sequence of 24 unique block rotations
	static const string rotationCodes = "xxxyzzzyxxxzyyyxzzzxyyyz";
	MyOGL::Timer analysysTime;
	do
		{
		switch(state())
			{
			case PROCESSING:
				checkAllPositions();		//check all (x, y) block positions
				rotateBlock(rotationCodes[rotation++]);	//rotate block after checking all available (x, y) positions
				if(rotation >= ALL_ROTATIONS)
					{
					state(TRANSFORMING);		//processing done, start transforming
					transformRot = 0;
					transformationTimer.restart();
					}
				break;
			case TRANSFORMING:
				transformBlock();
				if(state() != TRANSFORMING)
					return;
				if(transformationTimer > MAX_TRANSFORMATION_TIME)
					state(GAMEOVER);		//transformations take too long time, game is over
				break;
			case IDLE: startProcess(); break;		//processing done in previous process() call, start again
			}
		}
	while(analysysTime < ANALYSYS_MAX_TIME);
	}

void BlockAnalyzer::state(int newState)
	{
	if((newState < IDLE) || (newState > GAMEOVER))
		throw CuTeEx("Bad analyzer state number: " + lexical_cast<string>(newState));
	state_ = newState;
	}

void BlockAnalyzer::transformBlock()
	{
	const std::string& curRotCodes = best_.rotations();
	if((transformRot < curRotCodes.length()))		//there are some rotations left
		if(rotateCurrentBlock(curRotCodes[transformRot]))		//could perform rotation?
			++transformRot;		//prepare for next rotation
	if(parent.currentBlock().pos().x() > best_.x())		//move block a bit to its destination
		parent.moveLeft();
	if(parent.currentBlock().pos().x() < best_.x())
		parent.moveRight();
	if(parent.currentBlock().pos().y() > best_.y())
		parent.moveDown();
	if(parent.currentBlock().pos().y() < best_.y())
		parent.moveUp();
	//update state_ to IDLE if transformation done
	if((transformRot >= curRotCodes.length()) &&
		(parent.currentBlock().pos().x() == best_.x()) &&
		(parent.currentBlock().pos().y() == best_.y()))
			state(IDLE);
	}

void BlockAnalyzer::checkAllPositions()
	{
	for(int y = 0; y < parent.size(); ++y)
		for(int x = 0; x < parent.size(); ++x)
			{
			block.pos().x() = x;
			block.pos().y() = y;
			if(parent.canPut(block))
				{
				int factor = countFactor();
				if((factor > best_.factor_) || ((factor == best_.factor_) &&
					(BlockPos::rotationsCount(rotation) < BlockPos::rotationsCount(best_.rotation))))
					{
					best_.x_ = x;
					best_.y_ = y;
					best_.factor_ = factor;
					best_.rotation = rotation;
					}
				}
			}
	}

void BlockAnalyzer::rotateBlock(char axis)
	{
	switch(toupper(axis))
		{
		case 'X': block.rotateX(isupper(axis)); break;
		case 'Y': block.rotateY(isupper(axis)); break;
		case 'Z': block.rotateZ(isupper(axis)); break;
		default: throw CuTeEx("Bad block rotation char code");
		};
	}

bool BlockAnalyzer::rotateCurrentBlock(char axis)
	{
	switch(axis)
		{
		case 'x': return parent.rotateXCW(); break;
		case 'y': return parent.rotateYCW(); break;
		case 'z': return parent.rotateZCW(); break;
		case 'X': return parent.rotateXCCW(); break;
		case 'Y': return parent.rotateYCCW(); break;
		case 'Z': return parent.rotateZCCW(); break;
		default: throw CuTeEx("Bad block rotation char code");
		}
	}

void BlockAnalyzer::countCuboidHeights()
	{
	int x;
	for(x = 0; x < parent.size(); ++x)		//heights on a cuboid
		for(int y = 0; y < parent.size(); ++y)
			{
			int z;
			for(z = parent.depth() - 1; z >= 0; --z)	{
				if(parent(x, y, z))
					break;	}
			cuboidHeights[1 + x][1 + y] = z;
			}
	for(int p = 1; p <= parent.size(); ++p)		//set height borders
		{
		cuboidHeights[p][0] = cuboidHeights[p][1] + 1;
		cuboidHeights[p][parent.size() + 1] = cuboidHeights[x][parent.size()] + 1;
		cuboidHeights[0][p] = cuboidHeights[1][p] + 1;
		cuboidHeights[parent.size() + 1][p] = cuboidHeights[parent.size()][p] + 1;
		}
	}

int BlockAnalyzer::distance() const
	{
	int dist = parent.depth();
	for(int x = -block.range(); x <= block.range(); ++x)
		for(int y = -block.range(); y <= block.range(); ++y)
			for(int z = -block.range(); z <= block.range(); ++z)
				if(block(x, y, z))
					{
					int curDist =
						block.pos().z() + z - cuboidHeights[1 + block.pos().x() + x][1 + block.pos().y() + y] - 1;
					if(curDist < dist)
						dist = curDist;
					break;
					}
	return dist;
	}

int BlockAnalyzer::countFactor()
	{
	int dist = distance();
	block.pos().z() -= dist;		//move block forwars as far as possible
	int heightsFactor = 0;
	int distsFactor = 0;
	int edgeFactor = 0;
	for(int x = -block.range(); x <= block.range(); ++x)
		for(int y = -block.range(); y <= block.range(); ++y)
			for(int z = -block.range(); z <= block.range(); ++z)
				if(block(x, y, z))
					{
					heightsFactor += block.pos().z() + z;
					const int baseX = block.pos().x() + x;
					const int baseY = block.pos().y() + y;
					const int baseZ = block.pos().z() + z;
					if(parent(baseX - 1, baseY, baseZ))
						++edgeFactor;
					if(parent(baseX + 1, baseY, baseZ))
						++edgeFactor;
					if(parent(baseX, baseY - 1, baseZ))
						++edgeFactor;
					if(parent(baseX, baseY + 1, baseZ))
						++edgeFactor;
					if((((z > -2) && !block(x, y, z - 1)) || (z == -2)) && !parent(baseX, baseY, baseZ - 1))
						++distsFactor;
					}
	block.pos().z() += dist;
	return heightsFactor * HEIGHTS_WEIGHT + distsFactor * DISTS_WEIGHT + edgeFactor * EDGES_WEIGHT;
	}

//----------------------------------------------------------------------------
