//----------------------------------------------------------------------------

///@file
///OpenGL scene class definitions.
///
///@par License:
///@verbatim
///MyOGL - My OpenGL utility, simple OpenGL Windows framework
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

#include "scene.h"
using namespace MyOGL;

//----------------------------------------------------------------------------

Scene::~Scene(void)	{}

void Scene::start()
	{
	restart_ = false;
	done_ = false;
	MSG msg;
	do
		{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}
		if(win.active())
			{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			refresh();
			win.refresh();
			}
		}
	while((msg.message != WM_QUIT) && !done_);
	}

bool Scene::restart()
	{
	const bool oldRestart = restart_;
	restart_ = true;
	done();
	return oldRestart;		//scene is not running, method called elsewhere
	}

//----------------------------------------------------------------------------
