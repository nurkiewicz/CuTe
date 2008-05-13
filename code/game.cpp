//----------------------------------------------------------------------------

///@file
///Definitions of CuTe main game classes.
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
#include <algorithm>
#include <complex>
#include <cmath>
#include <boost/lexical_cast.hpp>
#include "language.h"
#include "game.h"
using namespace CuTe;
using boost::lexical_cast;
using MyOGL::glColorHSV;
using std::complex;
using std::polar;

//----------------------------------------------------------------------------

const float Game::Camera::ROTATION_SPEED = 90.0;
const float Game::Camera::AUTO_SPEED = 300.0;
const float Game::Camera::ZOOM_SPEED = 8.0;

Game::Camera::Camera(const Game& iParent):
	GLEngine::Camera(iParent.engine().size(), iParent.engine().depth(), ROTATION_SPEED), prevState(VIEW),
		mode(MODE_SOLID_BOTTOM), parent(iParent), border(4.0 / iParent.engine().size())
	{
	}

void Game::Camera::update()
	{
	//Ctrl or Shift were pressed, move the camera
	if(parent.win.keyDown(parent.controls(Controls::CAMERA_SET_TEMPORARY)) ||
		parent.win.keyDown(parent.controls(Controls::CAMERA_SET_PERMANENT)))
		{
		if(prevState == VIEW)
			lastSolid = pos();		//save the position before pressing Ctrl or Shift
		prevState = parent.win.keyDown(parent.controls(Controls::CAMERA_SET_PERMANENT))? EDIT_PERM : EDIT_TEMP;
		float timeElapsed = timer.restart() / 1000.0;		//update camera position
		if(parent.win.keyDown(parent.controls(Controls::MOVE_UP)))
			pos().x() += timeElapsed * ROTATION_SPEED;
		if(parent.win.keyDown(parent.controls(Controls::MOVE_DOWN)))
			pos().x() -= timeElapsed * ROTATION_SPEED;
		if(parent.win.keyDown(parent.controls(Controls::MOVE_LEFT)))
			pos().y() += timeElapsed * ROTATION_SPEED;
		if(parent.win.keyDown(parent.controls(Controls::MOVE_RIGHT)))
			pos().y() -= timeElapsed * ROTATION_SPEED;
		if(parent.win.keyDown(parent.controls(Controls::CAMERA_ZOOM_IN)))
			pos().z() += timeElapsed * ZOOM_SPEED;
		if(parent.win.keyDown(parent.controls(Controls::CAMERA_ZOOM_OUT)))
			pos().z() -= timeElapsed * ZOOM_SPEED;
		for(int digit = 1; digit <= 9; ++digit)		//check digit keys 1-9
			if(parent.win.keyDown('0' + digit))		//if some key pressed, save camera position on specified slot num.
				slots[digit] = pos();
		speed() = ROTATION_SPEED;
		}
	else
		{
		if(prevState == EDIT_TEMP)
			pos() = lastSolid;
		prevState = VIEW;
		for(int digit = 0; digit <= 9; ++digit)		//check digit keys 0-9
			if(parent.win.keyPressed('0' + digit))		//if some key pressed, load camera position from specified slot num.
				pos() = slots[digit];		//load desired slot
		speed() = AUTO_SPEED;
		}
	if(parent.win.keyPressed(parent.controls(Controls::CAMERA_MODE_0)))		//changing camera modes
		mode = MODE_SOLID_BOTTOM;
	if(parent.win.keyPressed(parent.controls(Controls::CAMERA_MODE_1)))
		mode = MODE_SOLID_TOP;
	if(parent.win.keyPressed(parent.controls(Controls::CAMERA_MODE_2)))
		mode = MODE_BLOCK;
	if(parent.win.keyPressed(parent.controls(Controls::CAMERA_DEFAULT_POS)))
		pos() = slots[0];		//load default position after Tab press
	GLEngine::Camera::update();		//call base class update() for smooth moving
	}

void Game::Camera::placeBase()
	{
	update();
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -5.5 + solid.z() + shift.z());
	glRotatef(solid.x() + shift.x(), 1.0, 0.0, 0.0);
	glRotatef(solid.y() + shift.y(), 0.0, 1.0, 0.0);
	}

void Game::Camera::place()
	{
	switch(mode)
		{
		case MODE_SOLID_BOTTOM: GLEngine::Camera::place(); break;		//base class place() is sufficient
		case MODE_SOLID_TOP:		//special place() routines for this mode
			placeBase();
			glTranslatef(-2.0, -2.0, -depth);
			break;
		case MODE_BLOCK:
			{
			placeBase();
			glTranslatef(-parent.engine().blockPos().x() * border, -parent.engine().blockPos().y() * border,
				-parent.engine().blockPos().z() * border);
			break;
			}
		default:
			throw CuTeEx("Bad camera mode");
		}
	}

//----------------------------------------------------------------------------

const float Game::Input::DEAD_ZONE_RADIUS = 22.0;
const float Game::Input::ZONE_MOVE_SPEED = DEAD_ZONE_RADIUS / 4;

Game::Input::Input(Game &iParent, bool iEnableMouse): parent(iParent), wheel(0),
	deadZonePos(parent.win.width() / 2, parent.win.height() / 2), enableMouse(iEnableMouse)
	{
	}

int Game::Input::angleToDirection(float angle)
	{
	if(abs(angle) < M_PI / 4)
		return 0;
	else
		if(abs(angle) > 3 * M_PI / 4)
			return 2;
		else
			if(angle > 0.0)
				return 3;
	return 1;
	}

void Game::Input::rotateBlockXY(int direction)
	{
	switch(direction)
		{
		case 0: parent.engine_.rotateYCCW(); break;
		case 1: parent.engine_.rotateXCW(); break;
		case 2: parent.engine_.rotateYCW(); break;
		default: parent.engine_.rotateXCCW(); break;
		}
	}

void Game::Input::moveBlock(int direction)
	{
	switch(direction)
		{
		case 0: parent.engine_.moveRight(); break;
		case 1: parent.engine_.moveUp(); break;
		case 2: parent.engine_.moveLeft(); break;
		default: parent.engine_.moveDown(); break;
		}
	}

void Game::Input::rotateBlockZ(int newWheel)
	{
	if(wheel > newWheel)		//wheel rotated forward
		parent.engine_.rotateZCW();
	else
		if(wheel < newWheel)		//wheel rotated backward
			parent.engine_.rotateZCCW();
	wheel = newWheel;		//save current wheel position
	}

void Game::Input::updateCursor()
	{
	int x = parent.win.mouse().pos.x;		//new X and Y mouse coordinates
	int y = parent.win.mouse().pos.y;
	bool changed = false;		//was the mouse position changed externally (using mouse_event)
	if(x == parent.win.width() - 1)	//if cursor exceed left edge of a screen, move back to the left side and vice versa
		{
		x = 4;
		deadZonePos = complex<float>(deadZonePos.real() - parent.win.width() - 5, deadZonePos.imag());
		changed = true;
		}
	else
		if(x == 0)
			{
			x = parent.win.width() - 5;
			deadZonePos = complex<float>(deadZonePos.real() + parent.win.width() - 5, deadZonePos.imag());
			changed = true;
			}
	if(y == parent.win.height() - 1)
		{
		y = 4;
		deadZonePos = complex<float>(deadZonePos.real(), deadZonePos.imag() - parent.win.height() - 5);
		changed = true;
		}
	else
		if(y == 0)
			{
			y = parent.win.height() - 5;
			deadZonePos = complex<float>(deadZonePos.real(), deadZonePos.imag() + parent.win.height() - 5);
			changed = true;
			}
	if(changed)		//some cursor position changed
		{
		mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, x * (65535 / parent.win.width()), y * (65535 / parent.win.height()), 0, 0);
		pos = complex<float>(x, y);
		}
	else
		pos = complex<float>(parent.win.mouse().pos.x, parent.win.mouse().pos.y);
	}

void Game::Input::mouseCheck()
	{
	updateCursor();		//update and get new mouse position
	//count the distance between current mouse position and its dead zone position
	const float zoneAbs = abs(pos - deadZonePos);
	const float zoneDirection = angleToDirection(arg(pos - deadZonePos));
	if(zoneAbs > DEAD_ZONE_RADIUS)
		{		//mouse cursor exceed dead zone borders
		if(parent.win.mouse().rButton)
			moveBlock(zoneDirection);
		else
			rotateBlockXY(zoneDirection);
		deadZonePos = pos;
		}
	else
		if(zoneAbs > 1.0)
			deadZonePos += polar<float>(ZONE_MOVE_SPEED / 1000.0 * timer, arg(pos - deadZonePos));
	timer.restart();
	rotateBlockZ(parent.win.mouse().wheel);
	if(parent.win.mouse().lButton)
		parent.engine_.moveForward();
	}

void Game::Input::keyboardCheck()
	{
	if(!parent.win.keyDown(parent.controls(Controls::CAMERA_SET_TEMPORARY)) &&
		!parent.win.keyDown(parent.controls(Controls::CAMERA_SET_PERMANENT)))
		{		//none of Shift and Ctrl were pressed, just move the block
		if(parent.win.keyDown(parent.controls(Controls::MOVE_UP)))
			parent.engine().moveUp();
		if(parent.win.keyDown(parent.controls(Controls::MOVE_DOWN)))
			parent.engine().moveDown();
		if(parent.win.keyDown(parent.controls(Controls::MOVE_RIGHT)))
			parent.engine().moveRight();
		if(parent.win.keyDown(parent.controls(Controls::MOVE_LEFT)))
			parent.engine().moveLeft();
		}
	if(parent.win.keyDown(parent.controls(Controls::MOVE_FORWARD)))
		parent.engine().moveForward();
	if(parent.win.keyDown(parent.controls(Controls::ROTATE_XCW)))		//block rotations
		parent.engine().rotateXCW();
	if(parent.win.keyDown(parent.controls(Controls::ROTATE_XCCW)))
		parent.engine().rotateXCCW();
	if(parent.win.keyDown(parent.controls(Controls::ROTATE_YCW)))
		parent.engine().rotateYCW();
	if(parent.win.keyDown(parent.controls(Controls::ROTATE_YCCW)))
		parent.engine().rotateYCCW();
	if(parent.win.keyDown(parent.controls(Controls::ROTATE_ZCW)))
		parent.engine().rotateZCW();
	if(parent.win.keyDown(parent.controls(Controls::ROTATE_ZCCW)))
		parent.engine().rotateZCCW();
	//init cheat-class if F1 pressed and cheat class idle
	if(parent.win.keyPressed(parent.controls(Controls::CHEAT)) && (parent.cheater.state() == BlockAnalyzer::IDLE))
		{
		parent.cheater.process();		//start cheat-machine analyzer
		parent.engine().points().cheat();		//take away some half of points
		}
	}

bool Game::Input::check()
	{
	if(parent.engine().pauseInfo.mode() == GLEngine::PauseInfo::RUNNING)		//game normally running
		if(parent.win.keyPressed(VK_ESCAPE))
			parent.engine().pause(true);		//pause the game
		else		//if game still isn't paused, normally check controls
			{
			keyboardCheck();
			if(enableMouse)
				mouseCheck();
			}
	else		//game is paused
		if(parent.win.keyPressed(VK_ESCAPE)
			&& (parent.engine().pauseInfo.mode() != GLEngine::PauseInfo::GAME_OVER))
			parent.engine().pause(false);		//resume game if pause is not caused by game over
		else
			if(parent.win.keyPressed(VK_RETURN))
				return true;
	return false;
	}

//----------------------------------------------------------------------------

const Game::Controls::ActionData Game::Controls::actionsData[] = {
	{"rotateXCW", 'Q'}, {"rotateXCCW", 'A'},
	{"rotateYCW", 'W'}, {"rotateYCCW", 'S'},
	{"rotateZCW", 'E'}, {"rotateZCCW", 'D'},
	{"moveLeft", VK_LEFT}, {"moveRight", VK_RIGHT}, {"moveUp", VK_UP}, {"moveDown", VK_DOWN},
	{"moveForward", ' '},
	{"cameraSetTemporary", VK_CONTROL}, {"cameraSetPermanent", VK_SHIFT}, {"cameraZoomIn", VK_NEXT},
	{"cameraZoomOut", VK_PRIOR}, {"cameraDefaultPos", VK_TAB}, 
	{"cameraMode0", 'Z'}, {"cameraMode1", 'X'}, {"cameraMode2", 'C'}, {"cheat", VK_F1}};

Game::Controls::Controls(MyXML::Key& iControlsKey):	controlsKey(&iControlsKey)
	{
	for(int action = 0; action < ALL_ACTIONS; ++action)
		actions[action] = static_cast<unsigned char>(MyXML::readKeyDef(
			(*controlsKey)[actionsData[action].actionName],
			static_cast<int>(actionsData[action].defaultKey)));
	}

Game::Controls::Controls(const Controls& src):	controlsKey(NULL)
	{
	actions = src.actions;
	}

Game::Controls::~Controls()
	{
	if(controlsKey != NULL)
		for(int action = 0; action < ALL_ACTIONS; ++action)
			(*controlsKey)[actionsData[action].actionName] = static_cast<int>(actions[action]);
	}

std::pair<const std::string, const std::string> Game::Controls::actionStrs(int action)
	{
	checkActionNum(action);
	MyXML::Key& label = langData["actionLabels"][actionsData[action].actionName];
	return std::make_pair(label["short"], label["desc"]);
	}

void Game::Controls::checkActionNum(int action)
	{
	if((action < 0) || (action >= ALL_ACTIONS))
		throw CuTeEx("Bad control key number: " + lexical_cast<std::string>(action));
	}

unsigned char Game::Controls::operator()(int action) const
	{
	checkActionNum(action);
	return actions[action];
	}

unsigned char& Game::Controls::operator()(int action)
	{
	checkActionNum(action);
	return actions[action];
	}

const std::string Game::Controls::keyCodeToStr(unsigned char keyCode)
	{
	if(((keyCode >= 'A') && (keyCode <= 'Z')) || ((keyCode >= '0') && (keyCode <= '9')))
		return '[' + std::string(1, keyCode) + ']';
	if((keyCode >= VK_F1) && (keyCode <= VK_F24))
		return "[F" + lexical_cast<std::string>(static_cast<int>(keyCode - VK_F1 + 1)) + ']';
	if((keyCode >= VK_NUMPAD0) && (keyCode <= VK_NUMPAD9))
		return "[Numpad " + lexical_cast<std::string>(static_cast<int>(keyCode - VK_NUMPAD0)) + ']';
	switch(keyCode)
		{
		case VK_BACK: return "[Backspace]";
		case VK_TAB: return "[Tab]";
		case VK_RETURN: return "[Enter]";
		case VK_SHIFT: return "[Shift]";
		case VK_CONTROL: return "[Ctrl]";
		case VK_MENU: return "[Alt]";
		case VK_PAUSE: return "[Pause]";
		case VK_ESCAPE: return "[Escape]";
		case VK_SPACE: return langData["keyNames"]["space"].value();
		case VK_PRIOR: return "[PgUp]";
		case VK_NEXT: return "[PgDn]";
		case VK_END: return "[End]";
		case VK_HOME: return "[Home]";
		case VK_LEFT: return langData["keyNames"]["left"].value();
		case VK_UP: return langData["keyNames"]["up"].value();
		case VK_RIGHT: return langData["keyNames"]["right"].value();
		case VK_DOWN: return langData["keyNames"]["down"].value();
		case VK_SNAPSHOT: return "[PrintScr]";
		case VK_INSERT: return "[Insert]";
		case VK_DELETE: return "[Delete]";
		}
	return '[' + lexical_cast<std::string>(static_cast<int>(keyCode)) + ']';
	}

int Game::Controls::controlAction(unsigned char keyCode)
	{
	int dist = std::distance(actions.begin(), std::find(actions.begin(), actions.end(), keyCode));
	if(dist < ALL_ACTIONS)
		return dist;
	return -1;
	}

//----------------------------------------------------------------------------

Game::Game(CuTeWindow &parentWindow, const Difficulty& iDifficulty, const Controls& iControls):
	CuTeScene(parentWindow), engine_(iDifficulty, win.extensions()), controls(iControls),
		input(*this, parentWindow.mode() > CuTeWindow::W_1024x768), done_(false),
		camera(*this), sideBar(iDifficulty, win.extensions()), cheater(engine_, false)
	{
	}

void Game::drawMainGame()
	{
	//Set the viewport to the left square of the screen
	win.viewport(0, win.height(), win.height(), 0);
	camera.place();		//position the camera
	engine_.draw();		//show main game window
	}

void Game::drawSideBar()
	{
	//Set the viewport to the upper part of info sidebar
	win.viewport(win.height(), win.width(), win.height(), win.width() - win.height(), true);
	SideBar::GameInfo info;		//save game information which will be shown on sidebar
	info.points = engine_.points();
	float temp;
	info.dist = (engine_.distance() + modf(engine_.blockPos().z(), &temp) - 0.5) / (engine_.depth() - 1);
	info.forwardMoveTime = engine_.moveForwardTime();
	info.speedChangeTime = engine_.speedChangeTime();
	info.speed = engine_.speed();
	info.gameTime = engine_.gameTime();
	glDisable(GL_DEPTH_TEST);
	sideBar.draw(info);		//pass game info to sideBar method
	glEnable(GL_DEPTH_TEST);
	}

void Game::drawNextBlock()
	{
	//Lower right corner - next block preview.
	win.viewport(win.height(), win.width(), win.width() - win.height(), 0);
	glTranslatef(0.0, 0.0, -3.0);
	engine_.drawNextBlock();
	}

void Game::refresh()
	{
	if(input.check())
		done();		//call run if user wants to finish the game
	if(cheater.state() != BlockAnalyzer::IDLE)
		cheater.process();		//process cheating analysis if cheater is not idle
	drawMainGame();
	drawSideBar();
	drawNextBlock();
	}

//----------------------------------------------------------------------------
