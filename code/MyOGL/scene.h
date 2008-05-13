//----------------------------------------------------------------------------

///@file
///OpenGL scene class.
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
///@par
///This class may simplify writing and displaying OpenGL scenes

//----------------------------------------------------------------------------

#ifndef MYOGL_SCENE_H
#define MYOGL_SCENE_H

//----------------------------------------------------------------------------

#include "window.h"

//----------------------------------------------------------------------------

namespace MyOGL
	{

//----------------------------------------------------------------------------

	///This class was designed to simplify the process of drawing and refreshing OpenGL scene.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	///@par
	///This is an abstract base class. By deriving from it and writing your own refresh() method
	///you can really simply create your OpenGL environment.
	///All you have to do is to create your OpenGL window (using MyOGL::Window object)
	///and pass reference to it to the constructor.
	///@par Example:
	///This code show basic OpenGL framework using MyOGL::Scene object:
	///@code
	/// #include "MyOGL/scene.h"
	/// 
	/// class MyScene: public MyOGL::Scene //(1)
	/// 	{
	/// 	private:
	/// 		void refresh();
	/// 	public:
	/// 		MyScene(MyOGL::Window &win): Scene(win)	{}
	/// 	};
	/// 
	/// void MyScene::refresh()
	/// 	{
	/// 	if(win.keyPressed(VK_ESCAPE))
	/// 		done(); //(2)
	/// 
	/// 	//Put here everything which you want to be executed every frame,
	/// 	//of course including your OpenGL code.
	/// 	}
	/// 
	/// WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	/// 	{
	/// 	try
	/// 		{
	/// 		MyOGL::Window win("My OpenGL Window", 800, 600, MyOGL::WINDOWED); //(3)
	/// 		MyScene scene(win); //(4)
	/// 		scene.start(); //(5)
	/// 		}
	/// 	catch(MyOGL::Exception &e)
	/// 		{
	/// 		MessageBox(NULL, e.what(), "OpenGL Window error", MB_OK | MB_ICONEXCLAMATION);
	/// 		}
	/// 	return 0;
	/// 	}
	/// 
	///@endcode
	/// <table><tr><td>(1)</td><td>Create a class derived from MyOGL::Scene</td></tr>
	/// <tr><td>(2)</td><td>When user pressed [Esc], finish with the scene and exit the program</td></tr>
	/// <tr><td>(3)</td><td>Create a window as you wish</td></tr>
	/// <tr><td>(4)</td><td>Create a scene passing reference to a window created in (3)</td></tr>
	/// <tr><td>(5)</td><td>Start rendering a scene. This method will exit when the user press
	///[Esc] key or click X button on window title bar.</td></tr></table>
	///All you've got to do now is write your OpenGL code in refresh(). You don't even need to bother
	///about clearing the color and depth buffer and swapping display buffers - this is all done in
	///Scene's start loop.
	class Scene
		{
		private:
			///Internal Scene flag indicatint that scene is done.
			///When you call done() method this flag is set. This signals the main loop in start()
			///to finish.
			///@sa done()
			///@sa start()
			bool done_;
			///Informs whether to restart the scene after leaving it.
			///This flag is to true if the user calls restart() somewhere inside the start() scene loop.
			///It then can be read using same restart() method in some external enviornment.
			///@sa restart()
			bool restart_;
		protected:
			///Parent window.
			///In order to make some actions (like refreshing the window), Scene object must have an access
			///to some window, which is called parent window.
			///@sa Scene(MyOGL::Window &parentWindow)
			MyOGL::Window &win;
			///Refreshes the scene.
			///In overriden version of this method you should put all the code (OpenGL, input check, etc.)
			///which you want to be executed every frame.<br>
			///What you don't have to do is to clear the color and depth buffers at the beginning
			///of drawing (it is done automatically), check whether the window is active at the moment
			///(refresh() is called only when MyOGL::Window::active() method returns true) and swap
			///display buffers (call MyOGL::Window::refresh()).
			///@sa start()
			virtual void refresh() = 0;
			///Exit from the scene.
			///Call this method in your refresh() when you want the scene to exit. The reason for that
			///might be user action (like pressing [Esc] in example in Scene class description) or
			///that the animation has finished.
			///@sa done_
			///@sa start()
			virtual void done()	{done_ = true;}
		public:
			///Constrcutor.
			///@param parentWindow Parent Window object of a scene. See win for more details.
			Scene(MyOGL::Window &parentWindow): restart_(false), win(parentWindow)	{}
			///Pure virtual destructor.
			virtual ~Scene() = 0;
			///Executes message loop.
			///This method enters a loop which processes Windows BlockAnalyzerMsg, calls refresh() every frame
			///and checks whether the done() didn't returned true (signal to finish).
			///@sa refresh()
			///@sa done()
			virtual void start();
			///Indicates wish to restart the scene or reading restart state.
			///This function has two different behaviours depending on when and where it was called:
			///@par 1. When scene is running
			///When scene is running (start() was called) this function sets
			///restart_ flag (which then can be read, see point 2.) and calls done() to finish the scene.
			///Pleas note that calling restart() <b>won't restart</b> the scene itself. It only set
			///flag to indicate the scene should be restarted. This information can be then read and scene
			///restarted.
			///@par 2. After finishing the scene
			///In this situation function won't do anything, it will only return the restart state.
			///@return In situation 2. the function will return true if in the last start() call
			///restart() was called indicating wish to restart the scene. In first situation
			///Function return value is useless.
			virtual bool restart();
		};

//----------------------------------------------------------------------------

	}		//namespace MyOGL

//----------------------------------------------------------------------------

#endif

//----------------------------------------------------------------------------
