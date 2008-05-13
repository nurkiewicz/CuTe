//---------------------------------------------------------------------------

///@file
///Definitions of functions from hsv2rgb.h.
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

//---------------------------------------------------------------------------

#define _USE_MATH_DEFINES		///<for MS VC++ compatibility (M_* are not part of the standard)
#include <algorithm>
#include <cmath>
#include "hsv2rgb.h"
using namespace std;

//---------------------------------------------------------------------------

template<typename T>
T *MyOGL::hsv2rgb<T>(T hue, T saturation, T value, T rgb[])
	{
	if(saturation > 0.0)
		{
		while(hue >= 2 * M_PI)		//trim hue into <0;2PI) range
			hue -= static_cast<T>(2 * M_PI);
		while(hue < 0.0)
			hue += static_cast<T>(2 * M_PI);
		hue *= static_cast<T>(3 / M_PI);		//scale hue from <0;2PI) to <0;6>
		const int ipart = static_cast<int>(hue);		//integer part of hue
		const T fpart = hue - ipart;		//fractional part of hue
		switch(ipart)
			{
			case 0:
				rgb[0] = value;
				rgb[1] = value * (1 - saturation * (1 - fpart));
				rgb[2] = value * (1 - saturation);
				break;
			case 1:
				rgb[0] = value * (1 - saturation * fpart);
				rgb[1] = value;
				rgb[2] = value * (1 - saturation);
				break;
			case 2:
				rgb[0] = value * (1 - saturation);
				rgb[1] = value;
				rgb[2] = value * (1 - saturation * (1 - fpart));
				break;
			case 3:
				rgb[0] = value * (1 - saturation);
				rgb[1] = value * (1 - saturation * fpart);
				rgb[2] = value;
				break;
			case 4:
				rgb[0] = value * (1 - saturation * (1 - fpart));
				rgb[1] = value * (1 - saturation);
				rgb[2] = value;
				break;
			default:		//sector 5
				rgb[0] = value;
				rgb[1] = value * (1 - saturation);
				rgb[2] = value * (1 - saturation * fpart);
				break;
			}
		}
	else
		fill(rgb, rgb + 3, value);		//all R, G and B value are set to value
	return rgb;
	}

float *MyOGL::hsv2rgb(float hue, float saturation, float value)
	{
	float rgb[3];
	return hsv2rgb(hue, saturation, value, rgb);
	}

//---------------------------------------------------------------------------
