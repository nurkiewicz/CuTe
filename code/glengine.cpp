//----------------------------------------------------------------------------

///@file
///CuTe game engine OpenGL wrapper classes definitions.
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

#define _USE_MATH_DEFINES		///<for MS VC++ compatibility (M_* are not part of the standard)
#include <cmath>
#include <complex>
#include "language.h"
#include "glengine.h"
#include "sounds.h"
using namespace CuTe;
using MyOGL::glColorHSV;
using namespace std;

//----------------------------------------------------------------------------

const float EngineExt::MOVE_SPEED = 8.0;
const float EngineExt::ROTATION_SPEED = 540.0;
const float EngineExt::MOVE_FORWARD_SPEED = 10.0;

const float EngineExt::MINIMAL_ALPHA = 0.01;
const float EngineExt::MAXIMAL_ALPHA = 1.0;
const float EngineExt::BLOCK_BLEND_SPEED = 1.5;
const float EngineExt::PLANES_BLEND_SPEED = 3.0;

EngineExt::EngineExt(const Difficulty& difficulty):
	Engine(difficulty), ALPHA_COEFF(log(MINIMAL_ALPHA / MAXIMAL_ALPHA) / difficulty.depth()),
		blockAlpha_(MINIMAL_ALPHA), blockAlphaShift(0.0), cuboidPlanesShift(difficulty.depth(), 0.0),
		removingPlanes(false), speed_(0), moveForwardPeriod(MOVE_FORWARD_PERIOD_MAX),
		speedChangePeriod(randomSpeedChangePeriod())
	{
	generateBlockGrid();		//generate grid for the first block
	}

bool EngineExt::moveRight()
	{
	//block can't be moved if some planes are during removing...
	//if X coordinate is positive, try to move block right. It possible, change some shift
	if(!removingPlanes && (posShift.x() >= 0.0) &&  Engine::moveRight())
		posShift.x() -= 1.0;
	return true;
	}

bool EngineExt::moveLeft()
	{
	if(!removingPlanes && (posShift.x() <= 0.0) &&  Engine::moveLeft())
		posShift.x() += 1.0;
	return true;
	}

bool EngineExt::moveUp()
	{
	if(!removingPlanes && (posShift.y() >= 0.0) && Engine::moveUp())
		posShift.y() -= 1.0;
	return true;
	}

bool EngineExt::moveDown()
	{
	if(!removingPlanes && (posShift.y() <= 0.0) && Engine::moveDown())
		posShift.y() += 1.0;
	return true;
	}

bool EngineExt::moveForward()
	{
	if(!removingPlanes && (posShift.z() <= 0.0) && Engine::moveForward())
		{
		posShift.z() = 1.0;
		moveForwardTimer.restart();		//reset the timer only when the moving actually taken place
		return true;
		}
	return false;
	}

bool EngineExt::rotateXCW()
	{
	if(!removingPlanes && !rotating() && Engine::rotateXCW())
		{
		angleShift.x() = 90.0;
		generateBlockGrid();
		sounds.play(Sounds::ROTATE);
		return true;
		}
	return false;
	}

bool EngineExt::rotateXCCW()
	{
	if(!removingPlanes && !rotating() && Engine::rotateXCCW())
		{
		angleShift.x() = -90.0;
		generateBlockGrid();
		sounds.play(Sounds::ROTATE);
		return true;
		}
	return false;
	}

bool EngineExt::rotateYCW()
	{
	if(!removingPlanes && !rotating() && Engine::rotateYCW())
		{
		angleShift.y() = 90.0;
		generateBlockGrid();
		sounds.play(Sounds::ROTATE);
		return true;
		}
	return false;
	}

bool EngineExt::rotateYCCW()
	{
	if(!removingPlanes && !rotating() && Engine::rotateYCCW())
		{
		angleShift.y() = -90.0;
		generateBlockGrid();
		sounds.play(Sounds::ROTATE);
		return true;
		}
	return false;
	}

bool EngineExt::rotateZCW()
	{
	if(!removingPlanes && !rotating() && Engine::rotateZCW())
		{
		angleShift.z() = 90.0;
		generateBlockGrid();
		sounds.play(Sounds::ROTATE);
		return true;
		}
	return false;
	}

bool EngineExt::rotateZCCW()
	{
	if(!removingPlanes && !rotating() && Engine::rotateZCCW())
		{
		angleShift.z() = -90.0;
		generateBlockGrid();
		sounds.play(Sounds::ROTATE);
		return true;
		}
	return false;
	}

void EngineExt::switchBlocks()
	{
	Engine::switchBlocks();
	generateBlockGrid();		//generate line grid for the new current block
	posShift.z() = 3.0;		//move block closer the user a bit
	blockAlpha_ = MINIMAL_ALPHA;		//alpha value should change rapidly here (don't use smooth shift)
	}

void EngineExt::update()
	{
	updateTimes();
	//tau is the time which have elapsed since last update();
	const float tau = timer.restart() / 1000.0;
	decAbs(posShift.x(), tau * MOVE_SPEED);
	decAbs(posShift.y(), tau * MOVE_SPEED);
	decAbs(posShift.z(), tau * MOVE_FORWARD_SPEED);		//moving forward with different speed
	if(removingPlanes)
		{
		decAbs<float>(planesAlpha_, tau * PLANES_BLEND_SPEED);		//make removed planes more translucent
		if(planesAlpha_ == 0.0)
			removingPlanes = false;		//removed planes completly transparent...
		}
	else
		for(int z = 0; z < depth(); ++z)	//set shifts for cubes in cuboid after some planes were removed
			decAbs<double>(cuboidPlanesShift[z], tau * MOVE_SPEED);
	//angle which the block have rotated since last update
	const float angle = tau * ROTATION_SPEED;
	decAbs<float>(angleShift, angle);
	///Change the blend speed
	decAbs<float>(blockAlphaShift, tau * BLOCK_BLEND_SPEED);
	}

void EngineExt::generateBlockGrid()
	{
	grid_.clear();		//erases prevoius grid data
	//each field of ?lines (? - x, y or z) corresponds to one line parallel to ? axis
	bool xlines[6][6][6] = {false};
	bool ylines[6][6][6] = {false};
	bool zlines[6][6][6] = {false};
	int x, y, z;
	const int range = currentBlock().range();
	for(x = -range; x <= range; ++x)
		for(y = -range; y <= range; ++y)
			for(z = -range; z <= range; ++z)
				if(currentBlock()(x, y, z))
					{		//if cube found, mark all its 12 borders
					xlines[x + 2][y + 2][z + 2] = !xlines[x + 2][y + 2][z + 2];
					xlines[x + 2][y + 3][z + 2] = !xlines[x + 2][y + 3][z + 2];
					xlines[x + 2][y + 2][z + 3] = !xlines[x + 2][y + 2][z + 3];
					xlines[x + 2][y + 3][z + 3] = !xlines[x + 2][y + 3][z + 3];
					ylines[x + 2][y + 2][z + 2] = !ylines[x + 2][y + 2][z + 2];
					ylines[x + 2][y + 2][z + 3] = !ylines[x + 2][y + 2][z + 3];
					ylines[x + 3][y + 2][z + 2] = !ylines[x + 3][y + 2][z + 2];
					ylines[x + 3][y + 2][z + 3] = !ylines[x + 3][y + 2][z + 3];
					zlines[x + 2][y + 2][z + 2] = !zlines[x + 2][y + 2][z + 2];
					zlines[x + 2][y + 3][z + 2] = !zlines[x + 2][y + 3][z + 2];
					zlines[x + 3][y + 2][z + 2] = !zlines[x + 3][y + 2][z + 2];
					zlines[x + 3][y + 3][z + 2] = !zlines[x + 3][y + 3][z + 2];
					}		//if two cubes has common border, this border is erased
	for(x = 0; x < 6; ++x)
		for(y = 0; y < 6; ++y)
			for(z = 0; z < 6; ++z)
				{		//converts 3 3D arrays into line coordinates
				if(xlines[x][y][z])
					grid_.push_back(Line(
						Point<double, 3>(x - 2.5, y - 2.5, z - 2.5),
						Point<double, 3>(x - 1.5, y - 2.5, z - 2.5)));
				if(ylines[x][y][z])
					grid_.push_back(Line(
						Point<double, 3>(x - 2.5, y - 1.5, z - 2.5),
						Point<double, 3>(x - 2.5, y - 2.5, z - 2.5)));
				if(zlines[x][y][z])
					grid_.push_back(Line(
						Point<double, 3>(x - 2.5, y - 2.5, z - 1.5),
						Point<double, 3>(x - 2.5, y - 2.5, z - 2.5)));
				}
	}

bool EngineExt::tryMove(Block &block)
	{
	Point<int, 3> oldPos(block.pos().x(), block.pos().y(), block.pos().z());		//previous block position
	if(Engine::tryMove(block))
		{
		//sets micro shifts if block was moved during rotation
		//Shifts are for current block, because the function assumes that we are actually checking it
		posShift.x() += oldPos.x() - block.pos().x();
		posShift.y() += oldPos.y() - block.pos().y();
		posShift.z() += oldPos.z() - block.pos().z();
		return true;
		}
	return false;
	}

float EngineExt::blockAlpha()
	{
	const float newAlpha = MAXIMAL_ALPHA * exp(ALPHA_COEFF * (distance() + posShift.z()));
	//Is the difference between previous and current alpha too big?
	if(abs(blockAlpha_ + blockAlphaShift - newAlpha) > 0.03)
		blockAlphaShift += blockAlpha_ - newAlpha;		//if it is, set shifts so that it could change smoothly
	blockAlpha_ = newAlpha;
	return blockAlpha_ + blockAlphaShift;
	}

float EngineExt::planesAlpha() const
	{
	if(removingPlanes)
		return planesAlpha_;
	else		//the planes are completly invisible
		return -1.0;		//to be sure that this will be noticed, -1.0 is returned insted of 0.0
	}

const Point<float, 3> EngineExt::blockPos() const
	{
	return Point<float, 3>(currentBlock().pos().x() + posShift.x() + 0.5,
		currentBlock().pos().y() + posShift.y() + 0.5, currentBlock().pos().z() + posShift.z() + 0.5);
	}

void EngineExt::removeFilledPlanes()
	{
	Engine::removeFilledPlanes();		//call base class method - physically remove planes
	int shift = 0;
	for(int z = 0; z < depth(); ++z)
		if(removedPlane(z))
			++shift;
		else		//set plane shift if some deeper located planes were removed
			cuboidPlanesShift[z - shift] = shift;
	if(shift > 0)		//at least one plane removed
		{
		removingPlanes = true;		//initiate first phase of removing planes
		planesAlpha_ = 1.0;
		sounds.play(Sounds::REMOVING);
		}
	}

void EngineExt::increaseSpeed()
	{
	static const float COEFF = -1.0 / 9.0 * log(static_cast<float>(MOVE_FORWARD_PERIOD_MAX) / 
		MOVE_FORWARD_PERIOD_MIN);		//exponential coefficient used to compute the speed
	if(speed_ < 9)
		{		//9th speed is the fastest
		++speed_;
		moveForwardPeriod = MOVE_FORWARD_PERIOD_MAX * exp(COEFF * speed_);
		speedChangeTimer.restart();		//restart time to next speed change
		pointsMul(speed_ + 1);		//increase points multiplier
		speedChangePeriod = randomSpeedChangePeriod();
		}
	}

void EngineExt::updateTimes()
	{
	if(moveForwardTimer >= moveForwardPeriod)
		//more time elapsed than FORWARD_MOVE_PERIOD - block should be moved forward automatically
		moveForward();
	if(speedChangeTimer >= speedChangePeriod)
		increaseSpeed();
	}

float EngineExt::speedChangeTime() const
	{
	if(speed_ == 9)
		return 1.0;		//if it's the fastest level, don't show this timer value
	return static_cast<float>(SPEED_CHANGE_PERIOD - speedChangeTimer) / SPEED_CHANGE_PERIOD;
	}

float EngineExt::moveForwardTime() const
	{
	if(speed_ == 9)
		return 0.0;
	return static_cast<float>(moveForwardPeriod - moveForwardTimer) / moveForwardPeriod;
	}

void EngineExt::pause(bool pauseState)
	{
	if(pauseState)
		{		//pause all game timers
		timer.pause();
		speedChangeTimer.pause();
		moveForwardTimer.pause();
		gameTimer.pause();
		}
	else
		{		//resume all game timers
		timer.resume();
		speedChangeTimer.resume();
		moveForwardTimer.resume();
		gameTimer.resume();
		}
	}

//----------------------------------------------------------------------------

const float GLEngine::Walls::COLOR_CHANGE_SPEED = 0.5;

GLEngine::Walls::Walls(int size, int depth, double border):
	SIZE(size), DEPTH(depth), BORDER(border), color(0.0), phi(0.0)
	{
	}

void GLEngine::Walls::update()
	{
	const float diff = COLOR_CHANGE_SPEED * timer.restart() / 1000.0;
	color += diff;
	while(color >= 2 * M_PI)
		color -= 2 * M_PI;
	phi += diff * 8;
	while(phi >= 2 * M_PI)
		phi -= 2 * M_PI;
	}

void GLEngine::Walls::draw()
	{
	update();
	glPushMatrix();
	glScalef(BORDER, BORDER, BORDER);
	int x, y, z;
	glBegin(GL_POINTS);
	glColorHSV(color, 0.6, 0.6 + 0.45 * (sin(phi) + 1));
	for(x = 1; x < SIZE; ++x)
		for(y = 1; y < SIZE; ++y)
			glVertex2i(x, y);
	for(z = 0; z <= DEPTH; ++z)
		{
		glColorHSV(color, 0.6, 0.6 + 0.45 * (sin(phi + z / 4.0) + 1));
		for(x = 0; x <= SIZE; ++x)
			{
			glVertex3i(x, 0, z);
			glVertex3i(x, SIZE, z);
			}
		for(y = 1; y <= SIZE; ++y)
			{
			glVertex3i(0, y, z);
			glVertex3i(SIZE, y, z);
			}
		}
	glEnd();
	glPopMatrix();
	}

//----------------------------------------------------------------------------

const float GLEngine::PauseInfo::ROTATION_SPEED = 2.5;
const float GLEngine::PauseInfo::MSG_AMPL = 12.0;

GLEngine::PauseInfo::PauseInfo(MyOGL::Extensions& iExtensions):
	extensions(iExtensions), angle(0.0), mode_(RUNNING), PAUSE_MSG(langData["inGame"]["pause"]),
		GAME_OVER_MSG(langData["inGame"]["gameOver"])
	{
	}

void GLEngine::PauseInfo::draw()
	{
	update();
	glDisable(GL_DEPTH_TEST);
	drawMsg();
	drawHelp();
	glEnable(GL_DEPTH_TEST);
	}

void GLEngine::PauseInfo::update()
	{
	angle += timer.restart() / 1000.0 * ROTATION_SPEED;
	}

void GLEngine::PauseInfo::drawMsg()
	{
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -7.0);
	glColorHSV(angle / 5.0, 1.0, 0.4);
	if(mode() == PAUSED)
		{
		glRotatef(sin(angle) * MSG_AMPL, 0.0, 0.0, 1.0);
		glTranslatef(-extensions.outlineFonts().width(PAUSE_MSG) / 2, -0.22, 0.0);
		extensions.outlineFonts() << PAUSE_MSG;
		}
	else
		{
		glTranslatef(0.0, 0.0, sin(angle));
		glTranslatef(-extensions.outlineFonts().width(GAME_OVER_MSG) / 2, -0.22, 0.0);
		extensions.outlineFonts() << GAME_OVER_MSG;
		}
	}

void GLEngine::PauseInfo::drawHelp()
	{
	glColorHSV(2 * M_PI / 3, 0.3, 0.7);
	if(mode_ == PAUSED)		//don't display info about resume when game is over
		extensions.bitmapFonts().pos(-0.2, -0.9) << langData["inGame"]["esc"].value();
	extensions.bitmapFonts().pos(-0.241, -0.98) << langData["inGame"]["enter"].value();
	}

//----------------------------------------------------------------------------

GLEngine::GLEngine(const Difficulty& difficulty, MyOGL::Extensions& iExtensions):
	EngineExt(difficulty), pauseInfo(iExtensions), extensions(iExtensions),
		walls(difficulty.size(), difficulty.depth(), 4.0 / size()), border(4.0 / size()),
		cubeDisplayList(buildDisplayLists()), nextBlockPreview(*this)
	{
	}

void GLEngine::drawCuboid() const
	{
	glEnable(GL_BLEND);
	int x, y, z;
	for(z = 0; z < depth(); ++z)
		{
		glColorHSV(ZPlanePos(z) * M_PI / 3, 1.0, 1.0);
		for(y = 0; y < size(); ++y)
			for(x = 0; x < size(); ++x)
				if(operator()(x, y, z))		//use overloaded operator operator()
					drawCube(x + 0.5, y + 0.5, ZPlanePos(z) + 0.5, border);
		}
	for(z = 0; z < depth(); ++z)
		if(removedPlane(z) && planesAlpha() > 0.0)
			{
			glColorHSV(z * M_PI / 3, 1.0, 1.0, planesAlpha());
			for(y = 0; y < size(); ++y)
				for(x = 0; x < size(); ++x)
					drawCube(x + 0.5, y + 0.5, z + 0.5, border);
			}
	glDisable(GL_BLEND);
	}

void GLEngine::drawBlock()
	{
	glEnable(GL_BLEND);
	glPushMatrix();		//save cuboid position
	glTranslated(blockPos().x() * border, blockPos().y() * border, blockPos().z() * border);
	glRotated(blockAngles().x(), 1.0, 0.0, 0.0);
	glRotated(blockAngles().y(), 0.0, 1.0, 0.0);
	glRotated(blockAngles().z(), 0.0, 0.0, 1.0);
	alpha = blockAlpha();
	drawBlockGrid();
	drawBlockCubes();
	glPopMatrix();
	glDisable(GL_BLEND);
	}

///@todo use CuTeScene::drawCube() here insted
void GLEngine::drawBlockCubes()
	{
	const int range = currentBlock().range();
	float phi = blockPos().z() - 0.5;		//hue in radians
	while(phi >= 6.0)
	phi -= 6.0;		//trim z to a range <0;6)
	for(int z = -range; z <= range; ++z)
		for(int y = -range; y <= range; ++y)
			for(int x = -range; x <= range; ++x)
				if(currentBlock()(x, y, z))
					{
					glColorHSV((phi + z) * M_PI / 3, 1.0, 1.0, alpha);
					drawCube(x, y, z, border);
					}
	}

///@bug Something's wrong in this function, when blocks goes down very fast (not smooth anim.)
void GLEngine::drawBlockGrid()
	{
	//Grid alpha is decreasing proportionally to alpha increeasing. But when alpha is greater than
	//DELTA, grid alpha is set to 0.0 (it's not even drawn)
	static const float DELTA = 0.8;
	if(alpha < DELTA)		//is the grid alpha greater than 0.0?
		{
		//the more opaque cube walls, the darker grid
		glColorHSV(0.0, 0.0, 0.7 * (1 - alpha / DELTA));
		glPushMatrix();
		glScalef(border, border, border);
		glBegin(GL_LINES);
		for_each(grid().begin(), grid().end(), drawBlockGridLine);
		glEnd();
		glPopMatrix();
		}
	}

void GLEngine::drawCube(double x, double y, double z, double border) const
	{
	extensions.textures().enable();
	extensions.textures().select(0);		//select cube texture
	glPushMatrix();		//saves current position
	glTranslatef(x * border, y * border, z * border);
	border *= 0.99;		//make a cube a little bit smaller
	glScaled(border, border, border);
	glCallList(cubeDisplayList);
	glPopMatrix();
	extensions.textures().disable();
	}

void GLEngine::draw()
	{
	walls.draw();
	switch(pauseInfo.mode())
		{
		case PauseInfo::RUNNING:
			drawCuboid();
			drawBlock();
			update();		//update EngineExt data after redrawing
			break;
		case PauseInfo::GAME_OVER:
			drawCuboid();
			drawBlock();
		case PauseInfo::PAUSED:
			pauseInfo.draw();
		}
	}

GLuint GLEngine::buildDisplayLists()
	{
	GLuint dList = glGenLists(1);
	glNewList(dList, GL_COMPILE);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0); glVertex3d(-0.5, -0.5, -0.5);		//Back surface of a cube
	glTexCoord2i(1, 0); glVertex3d(0.5, -0.5, -0.5);
	glTexCoord2i(1, 1); glVertex3d(0.5, 0.5, -0.5);
	glTexCoord2i(0, 1); glVertex3d(-0.5, 0.5, -0.5);
	glTexCoord2i(0, 0); glVertex3d(-0.5, 0.5, -0.5);		//Left surface
	glTexCoord2i(1, 0); glVertex3d(-0.5, -0.5, -0.5);
	glTexCoord2i(1, 1); glVertex3d(-0.5, -0.5, 0.5);
	glTexCoord2i(0, 1); glVertex3d(-0.5, 0.5, 0.5);
	glTexCoord2i(0, 0); glVertex3d(0.5, -0.5, 0.5);		//Right surface
	glTexCoord2i(1, 0); glVertex3d(0.5, -0.5, -0.5);
	glTexCoord2i(1, 1); glVertex3d(0.5, 0.5, -0.5);
	glTexCoord2i(0, 1); glVertex3d(0.5, 0.5, 0.5);
	glTexCoord2i(0, 0); glVertex3d(-0.5, -0.5, -0.5); //Bottom surface
	glTexCoord2i(1, 0); glVertex3d(0.5, -0.5, -0.5);
	glTexCoord2i(1, 1); glVertex3d(0.5, -0.5, 0.5);
	glTexCoord2i(0, 1); glVertex3d(-0.5, -0.5, 0.5);
	glTexCoord2i(0, 0); glVertex3d(-0.5, 0.5, -0.5);		//Top surface
	glTexCoord2i(1, 0); glVertex3d(-0.5, 0.5, 0.5);
	glTexCoord2i(1, 1); glVertex3d(0.5, 0.5, 0.5);
	glTexCoord2i(0, 1); glVertex3d(0.5, 0.5, -0.5);
	glTexCoord2i(0, 0); glVertex3d(-0.5, -0.5, 0.5);		//Front surface of a cube
	glTexCoord2i(1, 0); glVertex3d(0.5, -0.5, 0.5);
	glTexCoord2i(1, 1); glVertex3d(0.5, 0.5, 0.5);
	glTexCoord2i(0, 1); glVertex3d(-0.5, 0.5, 0.5);
	glEnd();
	glEndList();
	return dList;
	}

void GLEngine::switchBlocks()
	{
	sounds.play(Sounds::SWITCH_BLOCKS);
	EngineExt::switchBlocks();
	nextBlockPreview.switchBlocks();
	}

void GLEngine::pause(bool pauseState)
	{
	EngineExt::pause(pauseState);
	pauseInfo.mode(pauseState? PauseInfo::PAUSED : PauseInfo::RUNNING);
	}

void GLEngine::gameOver()
	{
	EngineExt::gameOver();
	pauseInfo.mode(PauseInfo::GAME_OVER);
	sounds.play(Sounds::GAME_OVER);
	}

//----------------------------------------------------------------------------

const float GLEngine::NextBlockPreview::BLEND_SPEED = 8.0;
const float GLEngine::NextBlockPreview::BEAT_PERIOD = 1.1;;
const float GLEngine::NextBlockPreview::BEAT_PEAK_INTERVAL = GLEngine::NextBlockPreview::BEAT_PERIOD / 5.0;
const float GLEngine::NextBlockPreview::BEAT_MIN = 0.5;
const float GLEngine::NextBlockPreview::BEAT_MAX = 0.55;

GLEngine::NextBlockPreview::NextBlockPreview(GLEngine &iParent):
	parent(iParent), angle(0.0), color(0.0), alphaShift(0.0), beat(BEAT_PERIOD)
	{
	}

void GLEngine::NextBlockPreview::draw()
	{
	update();
	glRotatef(35.0, 1.0, 0.0, 0.0);
	glRotatef(angle, 0.0, -1.0, 0.0);
	const int range = max(parent.nextBlock().range(), previousBlock.range());
	const float border = BEAT_MIN + (BEAT_MAX - BEAT_MIN) *
		(exp(-512.0 * sqr(beat - (BEAT_PERIOD - BEAT_PEAK_INTERVAL) / 2)) +
		exp(-512.0 * sqr(beat - (BEAT_PERIOD + BEAT_PEAK_INTERVAL) / 2)));
	glEnable(GL_BLEND);
	glEnable(GL_FOG);
	glFogf(GL_FOG_START, 2.0);
	glFogf(GL_FOG_END, 4.5);
	for(int y = -range; y <= range; ++y)
		for(int z = -range; z <= range; ++z)
			for(int x = -range; x <= range; ++x)
				{
				float alpha = 0.0;		//current block alpha value
				if(parent.nextBlock()(x, y, z))
					if(previousBlock(x, y, z))
						alpha = 1.0;
					else
						alpha = 1 - alphaShift;
				else
					if(previousBlock(x, y, z))
						alpha = alphaShift;
				if(alpha > 0.0)
					{
					glColorHSV(color, 0.8, 0.8, alpha);
					parent.drawCube(x, y, z, border);
					}
				}
	glDisable(GL_FOG);
	glDisable(GL_BLEND);
	}

void GLEngine::NextBlockPreview::update()
	{
	const float tau = timer.restart() / 1000.0;
	angle += tau * 90.0;		//rotate block
	color += tau * 0.1;		//change the color a bit
	if(color >= 2 * M_PI)
		color -= 2 * M_PI;		//trim color to range <0;2PI)
	decAbs(alphaShift, tau * BLEND_SPEED / 4.0f);
	decAbs(beat, tau);
	if(beat == 0.0)
		beat = BEAT_PERIOD;
	}

void GLEngine::NextBlockPreview::switchBlocks()
	{
	alphaShift = 1.0;
	previousBlock = parent.currentBlock();
	}

//----------------------------------------------------------------------------

GLEngine::Camera::Camera(int iSize, int iDepth, float iSpeed):
	depth(4.0 / iSize * iDepth), speed_(iSpeed)
	{
	}

template<typename T>
void GLEngine::Camera::trimAngle(T &angle)
	{
	if(angle >= 180.0)
		angle -= 360.0;
	else
		if(angle < -180.0)
			angle += 360.0;
	}

void GLEngine::Camera::update()
	{
	shift += solid - pos_;
	solid = pos_;
	trimAngle(shift.x());
	trimAngle(shift.y());

	//The COEFF makes camera move more smooth and simultanous.
	float timeElapsed = timer.restart() / 1000.0;
	const float COEFF = max3(abs(shift.x() / speed_), abs(shift.y() / speed_), 
		abs(shift.z() / speed_));
	if(COEFF > 0.0)		//if this is 0.0, camera isn't on its solid position, should go back to it
		{
		decAbs(shift.x(), timeElapsed * abs(shift.x()) / COEFF);
		decAbs(shift.y(), timeElapsed * abs(shift.y()) / COEFF);
		decAbs(shift.z(), timeElapsed * abs(shift.z()) / COEFF);
		}
	}

void GLEngine::Camera::place()
	{
	update();
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.5 - depth + solid.z() + shift.z());
	glRotatef(solid.x() + shift.x(), 1.0, 0.0, 0.0);
	glRotatef(solid.y() + shift.y(), 0.0, 1.0, 0.0);
	glTranslatef(-2.0, -2.0, 0.0);
	}

//----------------------------------------------------------------------------
