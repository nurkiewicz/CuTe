//---------------------------------------------------------------------------

///@file
///Functions for converting from HSV to RGB.
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
///Contains declarations of varoius functions used to convert from HSV to RGB color model

//---------------------------------------------------------------------------

#ifndef MYOGL_HSV2RGB_H
#define MYOGL_HSV2RGB_H

//---------------------------------------------------------------------------

namespace MyOGL
	{

//---------------------------------------------------------------------------

	///Converts colors from HSV to RGB model.
	///HSV (Hue, Saturation, Value; aka HSB ...Brithness) is a color model which is more familiar for men.
	///@param hue Color hue; this is an angle in radians and should be in range <0;2PI);
	///it works with all floats as well, but it takes time to trim down big angles (so try to be close to the the given range).
	///@param saturation Color saturation. Should be in range <0.0;1.0>, otherwise the result are undefined.
	///@param value Brithness of a color. Should be in range <0.0;1.0>, otherwise the result are undefined.
	///@param rgb Pointer to and 3-element float array. Converted RGB colors will be stored here (as well as returned by function).
	///@return Pointer to an rgb array given as a parameter. Thanks to that we can use this functions as follows
	///(hue = 2/3PI (120 degrees), saturation and value = 1.0 is a solid green):
	///@code
	///float rgb[3];
	///glColor3fv(hsv2rgb(2 * M_PI / 3, 1.0, 1.0, rgb));
	///@endcode
	///This will cause storing in rgb {0.0, 1.0, 0.0} and changing color in OpenGL to solid bright green.
	///@note This is a function template. Parameter T determinates the numeric data type used during converting.
	///The recommended are float and double. I will do the explicit specialization
	///of a function to work with ints as well
	///@sa hsv2rgb(float hue, float saturation, float value)
	///@sa glColorHSV(float hue, float saturation, float value)
	template<typename T>
	T *hsv2rgb(T hue, T saturation, T value, T rgb[]);

	///Converts colors from HSV to RGB model.
	///This overloaded version don't need float array buffer to store RGB values. You don't have to create
	///another variable, but on the other hand after passing returned pointer to the e.g. glColor3fv() function
	///you won't have access to converted colors (but most of the time you don't have such a need).
	///See hsv2rgb(float hue, float saturation, float value, float rgb[]) for more details.
	///@param hue Color hue; this is an angle in radians and should be in range <0;2PI);
	///Pleas check remarks at hsv2rgb(float hue, float saturation, float value, float rgb[])
	///@param saturation Color saturation. Should be in range <0.0;1.0>, otherwise the result are undefined.
	///@param value Brithness of a color. Should be in range <0.0;1.0>, otherwise the result are undefined.
	///@return Pointer to an RGB float array stored in memory.
	///@par Example:
	///Here is an example shows simplier way to change color:
	///@code
	///glColor3fv(hsv2rgb(2 * M_PI / 3, 1.0, 1.0));
	///@endcode
	///Compare it with example in hsv2rgb(float hue, float saturation, float value, float rgb[]).
	///The float rgb[3] variable weren't needed.
	///@warning Pleas note that pointer returned by this function is valid only immediately after call
	///(like in example). Although this might work fine, you cannot do something like for example:
	///@code
	///float *f = MyOGL::hsv2rgb(rot * M_PI / 180, 1.0, 1.0);
	///glColor3fv(f);
	///@endcode
	///@sa hsv2rgb(float hue, float saturation, float value, float rgb[])
	///@sa glColorHSV(float hue, float saturation, float value)
	float *hsv2rgb(float hue, float saturation, float value);

	
//---------------------------------------------------------------------------

	}		//namespace MyOGL

//---------------------------------------------------------------------------

#endif

//---------------------------------------------------------------------------
