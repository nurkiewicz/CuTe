//----------------------------------------------------------------------------

///@file
///Mathematical 2D or 3D point template.
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

#ifndef POINT_H
#define POINT_H

//----------------------------------------------------------------------------

#include <boost/operators.hpp>

//----------------------------------------------------------------------------

namespace CuTe
	{

//----------------------------------------------------------------------------

	///Math 2D or 3D point.
	///@author Tomasz Nurkiewicz, T.Nurkiewicz@stud.elka.pw.edu.pl
	///@date Feb 2006
	///@par
	///Althouh this template could hold points with coordinates of any type and with any
	///dimensions, it is onyl specialized for 2D nad 3D. More sophisticated points aren't
	///used in CuTe.
	template<typename T, int DIM>
	class Point;

//----------------------------------------------------------------------------

	///Point template specialization for 2D.
	template<typename T>
	class Point<T, 2>: boost::equality_comparable<Point<T, 2>,
		boost::addable<Point<T, 2>,
		boost::subtractable<Point<T, 2> > > >
		{
		public:
			///Constructor.
			///If the initial values aren't specified, the default-constructed values of
			///type T are assigned to all the coordinates.
			///@param x Initial X coodinate value.
			///@param y Initial Y coodinate value.
			Point(const T& x = T(), const T& y = T()): x_(x), y_(y)	{}
			///Returns X coordinate.
			///Non-const version allows to modify the X coordinate
			T& x()	{return x_;}
			///Returns X coordinate.
			///Const version allows only to read the X coordinate
			const T& x() const	{return x_;}
			///Returns Y coordinate.
			///Non-const version allows to modify the Y coordinate
			T& y()	{return y_;}
			///Returns Y coordinate.
			///Const version allows only to read the Y coordinate
			const T& y() const	{return y_;}
			///Allows to add two point's coordinates
			///Satisfies the operation: (x1, y1) += (x2, y2) = (x1 + x2, y1 + y2). boost library
			///automatically generates operator+() for this class.
			Point& operator+=(const Point& right)
				{x_ += right.x_; y_ += right.y_; return *this;}
			///Allows to subtract two point's coordinates
			///Satisfies the operation: (x1, y1) -= (x2, y2) = (x1 - x2, y1 - y2). boost library
			///automatically generates operator-() for this class.
			Point& operator-=(const Point& right)
				{x_ -= right.x_; y_ -= right.y_; return *this;}
			///Comparison operator.
			///Returns true only if all the coordinates are equal. boost library automatically
			///adds the operator!=() for this class.
			friend bool operator==(const Point& left, const Point& right)
				{return (left.x_ == right.x_) && (left.y_ == right.y_);}
		private:
			///X coordinate value.
			///@sa x()
			T x_;
			///Y coordinate value.
			///@sa y()
			T y_;
		};

//----------------------------------------------------------------------------

	///Point template specialization for 3D.
	template<typename T>
	class Point<T, 3>: boost::equality_comparable<Point<T, 3>,
		boost::addable<Point<T, 3>,
		boost::subtractable<Point<T, 3>,
		boost::multipliable<Point<T, 3>, T,
		boost::dividable<Point<T, 3>, T> > > > >
		{
		public:
			///Constructor.
			///If the initial values aren't specified, the default-constructed values of
			///type T are assigned to all the coordinates.
			///@param x Initial X coodinate value.
			///@param y Initial Y coodinate value.
			///@param z Initial Z coodinate value.
			Point(const T& x = T(), const T& y = T(), const T& z = T()): x_(x), y_(y), z_(z)	{}
			///Returns X coordinate.
			///Non-const version allows to modify the X coordinate
			T& x()	{return x_;}
			///Returns X coordinate.
			///Const version allows only to read the X coordinate
			const T& x() const	{return x_;}
			///Returns Y coordinate.
			///Non-const version allows to modify the Y coordinate
			T& y()	{return y_;}
			///Returns Y coordinate.
			///Const version allows only to read the Y coordinate
			const T& y() const	{return y_;}
			///Returns Z coordinate.
			///Non-const version allows to modify the Z coordinate
			T& z()	{return z_;}
			///Returns Z coordinate.
			///Const version allows only to read the Z coordinate
			const T& z() const	{return z_;}
			///Allows to add two point's coordinates
			///Satisfies the operation: (x1, y1) += (x2, y2) = (x1 + x2, y1 + y2). boost library
			///automatically generates operator+() for this class.
			Point& operator+=(const Point& right)
				{x_ += right.x_; y_ += right.y_; z_ += right.z_; return *this;}
			///Allows to subtract two point's coordinates
			///Satisfies the operation: (x1, y1) -= (x2, y2) = (x1 - x2, y1 - y2). boost library
			///automatically generates operator-() for this class.
			Point& operator-=(const Point& right)
				{x_ -= right.x_; y_ -= right.y_; z_ -= right.z_; return *this;}
			///Multiplies every coordinate by the given scalar.
			///This operator satisfies the operation: (x, y, z) * t = (x * t, y * t, z * t)
			Point& operator*=(const T& right)
				{x_ *= right; y_ *= right; z_ *= right; return *this;}
			///Divides every coordinate by the given scalar.
			///This operator satisfies the operation: (x, y, z) / t = (x / t, y / t, z / t)
			Point& operator/=(const T& right)
				{x_ /= right; y_ /= right; z_ /= right; return *this;}
			///Assignment operator.
			Point& operator=(const Point& right)
				{x_ = right.x_; y_ = right.y_; z_ = right.z_; return *this;}
			///Comparison operator.
			///Returns true only if all the coordinates are equal. boost library automatically
			///adds the operator!=() for this class.
			friend bool operator==(const Point& left, const Point& right)
				{return (left.x_ == right.x_) && (left.y_ == right.y_) && (left.z_ == right.z_);}
		private:
			///X coordinate value.
			///@sa x()
			T x_;
			///Y coordinate value.
			///@sa y()
			T y_;
			///Z coordinate value.
			///@sa z()
			T z_;
		};

//----------------------------------------------------------------------------

	}		//namespace CuTe

//----------------------------------------------------------------------------

#endif		//#define POINT_H

//----------------------------------------------------------------------------
