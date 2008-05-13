//----------------------------------------------------------------------------

///@file
///Implementation of some message and drawing routines in demo mode.
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

#include "language.h"
#include "demo.h"
using namespace CuTe;
using std::string;
using MyOGL::glColorHSV;
using boost::lexical_cast;

//----------------------------------------------------------------------------

const float DemoEngine::BlockAnalyzerMsg::VERTICAL_DIST = 0.07;

DemoEngine::BlockAnalyzerMsg::BlockAnalyzerMsg(GLEngine& parent, MyOGL::BitmapFonts& iFonts):
	BlockAnalyzer(parent), fonts(iFonts)
	{
	}

const std::string DemoEngine::BlockAnalyzerMsg::codeToDirection(char axis)
	{
	switch(axis)
		{
		case 'x': return "XCW"; break;
		case 'y': return "YCW"; break;
		case 'z': return "ZCW"; break;
		case 'X': return "XCCW"; break;
		case 'Y': return "YCCW"; break;
		case 'Z': return "ZCCW"; break;
		default: throw CuTeEx("Bad block rotation char code");
		}
	}

std::string DemoEngine::BlockAnalyzerMsg::bestStr() const
	{
	string s = '(' + lexical_cast<std::string>(best().x()) + ", " + lexical_cast<std::string>(best().y()) + "): [" +
		lexical_cast<std::string>(best().factor()) + "] ";
	for(string::const_iterator c = best().rotations().begin(); c != best().rotations().end(); ++c)
		s += codeToDirection(*c) + ", ";
	return s;
	}

void DemoEngine::BlockAnalyzerMsg::state(int newState)
	{
	BlockAnalyzer::state(newState);
	switch(newState)
		{		//insert message about changing the state
		case IDLE: insert(langData["inGame"]["waiting"]); break;
		case PROCESSING: insert(langData["inGame"]["processing"]); break;
		case TRANSFORMING: insert(langData["inGame"]["transforming"]); break;
		}
	}

void DemoEngine::BlockAnalyzerMsg::update()
	{
	while((msgs.size() > 0) && (timer >= MESSAGE_VALIDITY_TIME))
		{
		msgs.pop_front();		//remove the oldest message
		timer.shift(-MESSAGE_VALIDITY_TIME);
		}
	if(BlockAnalyzer::state() == PROCESSING)
		{
		string newBest = bestStr();
		if((msgs.size() == 0) || (newBest != msgs.back().second))		//new message differs the last one added...
			insert(newBest);
		}
	vertPosShift = (MESSAGE_VALIDITY_TIME - timer) * VERTICAL_DIST / MESSAGE_VALIDITY_TIME;
	}

bool DemoEngine::BlockAnalyzerMsg::rotateCurrentBlock(char axis)
	{
	if(BlockAnalyzer::rotateCurrentBlock(axis))
		{
		insert("rotate" + codeToDirection(axis) + "()");
		return true;
		}
	return false;
	}

void DemoEngine::BlockAnalyzerMsg::draw()
	{
	update();
	glLoadIdentity();
	fonts.select(1);
	int msgIndex = 0;
	for(msgList::const_iterator msg = msgs.begin(); msg != msgs.end(); ++msg, ++msgIndex)
		{
		const float colorValue = 0.1 + 0.9 * (msgIndex + vertPosShift / VERTICAL_DIST) / ALL_MESSAGES_COUNT;
		const float pos = -0.15 - vertPosShift - msgIndex * VERTICAL_DIST;
		glColorHSV(4.0, 0.3, colorValue);
		fonts.pos(-0.42, pos) << msg->first;		//message time
		glColorHSV(2.5, 0.8, colorValue);
		fonts.pos(-0.18, pos) << msg->second;		//message content
		}
	fonts.select(0);
	}

void DemoEngine::BlockAnalyzerMsg::insert(const std::string& msg)
	{
	msgs.push_back(make_pair(timeToFmtStr(parent.gameTime()) + ':', msg));
	if(msgs.size() > ALL_MESSAGES_COUNT)
		{
		msgs.pop_front();		//remove the oldest message if list full
		timer.restart();
		}
	}

//----------------------------------------------------------------------------

DemoEngine::DemoEngine(const Difficulty& difficulty, MyOGL::Extensions& iExtensions):
	GLEngine(difficulty, iExtensions), analyzer(*this, iExtensions.bitmapFonts()), restart_(false)
	{
	}

void DemoEngine::update()
	{
	GLEngine::update();
	switch(static_cast<BlockAnalyzer>(analyzer).state())
		{
		//move block forward if all transformations had been done
		case BlockAnalyzer::IDLE: moveForward(); break;
		case BlockAnalyzer::GAMEOVER: restart_ = true; break;
		default: analyzer.process(); break;		//continue processing if its not done
		}
	}

void DemoEngine::switchBlocks()
	{
	GLEngine::switchBlocks();
	analyzer.process();		//process next block after switching
	}

//----------------------------------------------------------------------------

Demo::Camera::Camera(const Difficulty& difficulty):
	GLEngine::Camera(difficulty.size(), difficulty.depth(), 3.0), nextUpdateTime(0)
	{
	}

void Demo::Camera::update()
	{
	GLEngine::Camera::update();
	if(timer > nextUpdateTime)
		{
		timer.restart();
		nextUpdateTime = 3 * 1024 + rand() % (2 * 1024);		//randomize next update time
		pos().x() = (rand() % 64 - 32) / 10.0;
		pos().y() = (rand() % 128 - 64) / 10.0;
		pos().z() = (rand() % 64 - 24) / 15.0;
		}
	}

//----------------------------------------------------------------------------

Demo::Demo(CuTeWindow &parentWindow, const Difficulty& difficulty):
	CuTeScene(parentWindow), engine(difficulty, parentWindow.extensions()), camera(difficulty)
	{
	}

void Demo::refresh()
	{
	if(win.keyPressed(VK_ESCAPE))
		done();		//exit when [Esc]
	if(engine.restart())
		restart();
	drawEngine();
	drawInfo();
	drawNextBlock();
	}

void Demo::drawEngine()
	{
	//Set the viewport to the RIGHT square of the screen
	win.viewport(win.width() - win.height(), win.width(), win.height(), 0);
	camera.place();
	engine.draw();
	}

void Demo::drawNextBlock()
	{
	///Lower left corner - next block preview.
	win.viewport(0, win.width() - win.height(), win.width() - win.height(), 0);
	glTranslatef(0.0, 0.0, -3.0);
	engine.drawNextBlock();
	}

void Demo::drawInfo()
	{
	win.viewport(0, win.width() - win.height(), win.height(), win.width() - win.height(), true);
	drawBackground();
	engine.drawBlockAnalyzerMsg();
	glTranslatef(-0.47, 0.16, 0.0);
	glRotatef(-20.0, 0.0, 0.0, 1.0);
	glColorHSV(4.5, 0.6, 0.5);
	glScalef(0.25, 0.25, 0.25);
	win.extensions().outlineFonts() << timeToFmtStr(engine.gameTime());
	}

void Demo::drawBackground()
	{
	win.extensions().textures().enable();
	win.extensions().textures().select(1);
	glColorHSV(0.0, 0.0, 1.0);
	glBegin(GL_QUADS);
	glTexCoord2i(0, 0);
	glVertex2f(-0.5, 0.0);
	glTexCoord2i(1, 0);
	glVertex2f(0.5, 0.0);
	glTexCoord2i(1, 1);
	glVertex2f(0.5, 1.0);
	glTexCoord2i(0, 1);
	glVertex2f(-0.5, 1.0);
	glEnd();
	win.extensions().textures().disable();
	}

//----------------------------------------------------------------------------
