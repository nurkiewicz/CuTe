//----------------------------------------------------------------------------

///@file
///Atom visualization in the top right corner in menu.
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
///Classes to visualize some number of electrons rotating around any object.

//----------------------------------------------------------------------------

#ifndef ATOM_H
#define ATOM_H

//----------------------------------------------------------------------------

#include "MyXML/myxml.h"
#include "MyOGL/scene.h"
#include "xmlglcmd.h"

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///One "atom" representing some "electrons" rotating around some object in the middle ("core")
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Jul 2005-Mar 2006
	class Atom
		{
		private:

			///Corresponads to one "electron" rotating around (0, 0, 0) point
			///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
			///@date Jul 2005-Mar 2006
			///@par
			///The electrons looks rather like a comet with a long blended colored tail.
			class Electron
				{
				private:
					///Minimum radius of a spinning electron.
					static const float RADIUS_MIN;
					///Maximum radius of a spinning electron.
					static const float RADIUS_MAX;
					///Speed of an electron at the radius of about 1.0
					///The actual speed is calculated depending on the electron radius.
					///@sa radius
					static const float BASE_SPEED;
					///Length of an electron tail.
					///The length is an angle in radians.
					///@sa TAIL_POINTS
					static const float TAIL_LENGTH;
					///Number of points to use when drawing the tail.
					///The more points you use, the tail would be more smooth, but it will take more time
					///to render it.
					///@sa TAIL_POINTS
					static const int TAIL_POINTS = 10;
					///X gradient of an electron orbit.
					///Before drawing the electron its coordinates will be rotated around X axis using
					///this value. Thanks to it the electron rotates around the whole sphere, not only
					///in X-Y plane.
					///@sa yEcliptic
					///@sa draw()
					float xEcliptic;
					///Y gradient of an electron orbit.
					///Before drawing the electron its coordinates will be rotated around Y axis using
					///this value. Thanks to it the electron rotates around the whole sphere, not only
					///in X-Y plane.
					///@sa xEcliptic
					///@sa draw()
					float yEcliptic;
					///Current electron angle.
					///This value is updated in update() method to make an impression of a moving electron.
					///@sa update()
					float angle;
					///Object timer used when calculating the angle.
					///@sa angle
					///@sa update()
					MyOGL::Timer timer;
					///Updates the electron angle.
					///The value depends on the electron's speed and is calculated on base of timer's time.
					///@sa speed
					///@sa timer
					void update();
					///Electron's color.
					///Each electron has its own randomized color. This is the color's hue in HSV model. It
					///is randomized in constructor.
					const float color;
					///Electron move radius.
					///This should be the value between <RADIUS_MIN; RADIUS_MAX>. The electron's speed
					///depends on its radius
					///@sa speed
					const float radius;
					///Tells how fast is the electron moving.
					///This value is calculated in contructor depending on the values of BASE_SPEED and the
					///radius.
					///@sa BASE_SPEED
					///@sa radius
					const float speed;
				public:
					///Constructor randomizes almost all electron's properties.
					///@sa speed, radius, color etc.
					Electron();
					///Draws the electron.
					///First sets up proper angles and then draws the electron including its colroed tail.
					///@sa xEcliptic and yEcliptic
					void draw();
				};

			///Whole atom ration speed.
			///The whole atom (core and electrons) is rotating slowly around some vector to
			///make the whole animation even more lively.
			///@sa update()
			///@sa rot
			static const float ROTATION_SPEED;
			///Arrays of all atom electrons.
			///The exact number of electrons is given in the constructor.
			///@sa electronsCount;
			///@sa Atom()
			Electron* electrons;
			///Current atom angle.
			///This value stores the current atom angle; its value is calculated in update() method
			///depending on the ROTATION_SPEED.
			///@sa update()
			float rot;
			///Object timer used when calculating rotations.
			///@sa update()
			MyOGL::Timer timer;
			///Updates the atom angle.
			///Updates the rot variable to make the whole atom rotation animation.
			///@sa rot
			void update();
			///OpenGL display list containing the atom core model.
			///This model is drawn in the center (core) of the atom. It might be any OpenGL object.
			///@sa useModel() for choosing the core object
			GLuint coreModelList;
			///Number of all electrons spinning around the core.
			///This number is given in the constructor.
			///@sa electrons
			///@sa draw()
			const int electronsCount;
		public:
			///Creates an array of specified number of electrones.
			///@param iElectronsCount How many electrons rotating around the core to create
			///@sa electrons
			///@sa electronsCount
			Atom(int iElectronsCount);
			///Deletes the electrons array.
			///@sa Atom()
			///@sa electrons
			~Atom()	{delete[] electrons;}
			///Draws the atom.
			///First draws the atom core and then calls draw() for all electrons saved in electrons
			///variable.
			///@sa electrons
			void draw();
			///Specifies model to use as an atom core.
			///This model should be coded in XML key. It is then compiled into OpenGL display list.
			///@param coreModel XML key containing encoded model data.
			///@sa coreModelList
			void useModel(const MyXML::Key& coreModel)	{coreModelList = buildDisplayList(coreModel);}
		};

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif		//#define ATOM_H

//----------------------------------------------------------------------------
