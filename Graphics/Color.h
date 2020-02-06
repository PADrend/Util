/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_COLOR_H_
#define UTIL_COLOR_H_

#include "../Utils.h"

#include <array>
#include <istream>
#include <ostream>
#include <sstream>
#include <cstdint>
#include <string>
#include <vector>
#include <cassert>

namespace Util {

class Color4f;

/** Representation of an RGBA color that is stored as four bytes.
 * @ingroup graphics
 */
class Color4ub {
		std::array<uint8_t, 4> values;

	public:
		Color4ub() : Color4ub{0, 0, 0, 255} {
		}
		Color4ub(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255) {
			set(_r, _g, _b, _a);
		}
		explicit Color4ub(uint32_t u32) {
			*reinterpret_cast<uint32_t *>(values.data()) = u32;
		}
		explicit inline Color4ub(const Color4f & other); // explicit due to possible information loss
		explicit Color4ub(const std::vector<uint8_t> & arr){
			assert(arr.size() == 4);
			set(arr[0],arr[1],arr[2],arr[3]);
		}
		Color4ub(const Color4ub &) = default;
		Color4ub(Color4ub &&) = default;

		Color4ub& operator=(const Color4ub &) = default;
		Color4ub& operator=(Color4ub &&) = default;
			
		bool operator==(const Color4ub & other) const {
			return values == other.values;
		}
		bool operator!=(const Color4ub & other) const {
			return values != other.values;
		}
		void set(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) {
			r(_r);
			g(_g);
			b(_b);
			a(_a);
		}
		//! @return true iff the color is FULLY transparent
		bool isTransparent() const		{	return a() == 0;	}

		//!	@return true iff the color is FULLY opaque
		bool isOpaque() const 			{	return a() == 255;	}

		const uint8_t * data() const	{	return values.data();	}

		uint32_t getAsUInt() const		{	return *reinterpret_cast<const uint32_t *>(values.data());	}

		uint8_t r() const				{	return values[0];	}
		uint8_t g() const				{	return values[1];	}
		uint8_t b() const				{	return values[2];	}
		uint8_t a() const				{	return values[3];	}
		uint8_t getR() const			{	return r();	}
		uint8_t getG() const			{	return g();	}
		uint8_t getB() const			{	return b();	}
		uint8_t getA() const			{	return a();	}
		void r(uint8_t _r)				{	values[0] = _r;	}
		void g(uint8_t _g)				{	values[1] = _g;	}
		void b(uint8_t _b)				{	values[2] = _b;	}
		void a(uint8_t _a)				{	values[3] = _a;	}
		void setR(uint8_t _r)			{	r(_r);	}
		void setG(uint8_t _g)			{	g(_g);	}
		void setB(uint8_t _b)			{	b(_b);	}
		void setA(uint8_t _a)			{	a(_a);	}

		std::string toString() const {
			std::ostringstream s;
			s << "Color4ub(" << static_cast<int>(r()) << "," << static_cast<int>(g()) << "," << static_cast<int>(b()) << "," << static_cast<int>(a()) << ")";
			return s.str();
		}

		//! Return a new color with the absolute difference between the two given colors.
		static Color4ub createDifferenceColor(const Color4ub & first, const Color4ub & second) {
			Color4ub result;
			if(first.r() > second.r()) {
				result.r(first.r() - second.r());
			} else {
				result.r(second.r() - first.r());
			}
			if(first.g() > second.g()) {
				result.g(first.g() - second.g());
			} else {
				result.g(second.g() - first.g());
			}
			if(first.b() > second.b()) {
				result.b(first.b() - second.b());
			} else {
				result.b(second.b() - first.b());
			}
			if(first.a() > second.a()) {
				result.a(first.a() - second.a());
			} else {
				result.a(second.a() - first.a());
			}
			return result;
		}

		//! @name Serialization
		//@{
	public:
		friend std::ostream & operator<<(std::ostream & out, const Color4ub & color) {
			return out << static_cast<uint32_t>(color.r()) << ' '
				   << static_cast<uint32_t>(color.g()) << ' '
				   << static_cast<uint32_t>(color.b()) << ' '
				   << static_cast<uint32_t>(color.a());
		}
		friend std::istream & operator>>(std::istream & in, Color4ub & color) {
			uint32_t _r,_g,_b,_a;
			in >> _r >> _g >> _b >> _a;
			color.set(_r, _g, _b, _a);
			return in;
		}
		//@}
};

//! Representation of an RGBA color that is stored as four floats.
//! @ingroup graphics
class Color4f {
	private:
		std::array<float, 4> values;		

	public:
		Color4f() : Color4f(0,0,0,1.0) {
		}
		Color4f(float _r, float _g, float _b, float _a = 1.0f) {
			set(_r,_g,_b,_a);
		}
		Color4f(const Color4ub & other) : Color4f((1.0f / 255.0f) * other.getR(),(1.0f / 255.0f) * other.getG(),(1.0f / 255.0f) * other.getB(),(1.0f / 255.0f) * other.getA()) {
		}

		explicit Color4f(const std::vector<float> & arr){
			assert(arr.size() == 4);
			set(arr[0],arr[1],arr[2],arr[3]);
		}
		Color4f(const Color4f &) = default;
		Color4f(Color4f &&) = default;

		/*!
		 * mixing constructor
		 * @param first color for interpolation
		 * @param second color for interpolation
		 * @param v specifies the percentage of first in the result color, clamped to [0,1]
		 * @return the color gained by mapping [0,1] --> [first, second]
		 */
		Color4f(const Color4f & first, const Color4f & second, float v) {
			if(v <= 0.0f) {
				*this = first;
			} else if(v >= 1.0f) {
				*this = second;
			} else {
				const float w = 1.0f - v;
				set(w * first.r() + v * second.r(),
					w * first.g() + v * second.g(),
					w * first.b() + v * second.b(),
					w * first.a() + v * second.a());
			}
		}

		void set(float _r, float _g, float _b, float _a) {
			r(_r);
			g(_g);
			b(_b);
			a(_a);
		}
		//! @return true iff the color is FULLY transparent
		bool isTransparent() const 						{	return a() <= 0.0f;	}

		//! @return true iff the color is FULLY opaque
		bool isOpaque() const							{	return a() >= 1.0f;	}

		const float * data() const						{	return values.data();	}

		float r() const									{	return values[0];	}
		void r(float value) 							{	values[0] = value;	}
		float g() const 								{	return values[1];	}
		void g(float value) 							{	values[1] = value;	}
		float b() const 								{	return values[2];	}
		void b(float value) 							{	values[2] = value;	}
		float a() const 								{	return values[3];	}
		void a(float value) 							{	values[3] = value;	}

		float getR() const								{	return r();	}
		float getG() const 								{	return g();	}
		float getB() const 								{	return b();	}
		float getA() const 								{	return a();	}
		void setR(float value) 							{	r(value);	}
		void setG(float value) 							{	g(value);	}
		void setB(float value) 							{	b(value);	}
		void setA(float value) 							{	a(value);	}

		Color4f abs() const {
			return Color4f( r()<0 ? -r() : r(), g()<0 ? -g() : g(), b()<0 ? -b() : b(), a()<0 ? -a() : a() );
		}

		Color4f & operator=(const Color4f &) = default;
		Color4f & operator=(Color4f &&) = default;
		bool operator==(const Color4f & other) const 	{	return values == other.values;		}
		bool operator!=(const Color4f & other) const 	{	return values != other.values;		}

		Color4f operator+(const Color4f & other) const {
			return Color4f(r() + other.r(), g() + other.g(), b() + other.b(), a() + other.a());
		}
		Color4f operator-(const Color4f & other) const	{
			return Color4f(r() - other.r(), g() - other.g(), b() - other.b(), a() - other.a());
		}
		Color4f operator*(float f) const {
			return Color4f(r() * f, g() * f, b() * f, a() * f);
		}
		Color4f operator*(const Color4f & other) const {
			return Color4f(r() * other.r(), g() * other.g(), b() * other.b(), a() * other.a());
		}
		Color4f operator/(float f) const {
			return Color4f(r() / f, g() / f, b() / f, a() / f);
		}

		Color4f & operator+=(const Color4f & other) {
			values[0] += other.values[0];
			values[1] += other.values[1];
			values[2] += other.values[2];
			values[3] += other.values[3];
			return *this;
		}
		Color4f & operator-=(const Color4f & other) {
			values[0] -= other.values[0];
			values[1] -= other.values[1];
			values[2] -= other.values[2];
			values[3] -= other.values[3];
			return *this;
		}
		Color4f & operator*=(const Color4f & other) {
			values[0] *= other.values[0];
			values[1] *= other.values[1];
			values[2] *= other.values[2];
			values[3] *= other.values[3];
			return *this;
		}
		Color4f & operator*=(float f) {
			values[0] *= f;
			values[1] *= f;
			values[2] *= f;
			values[3] *= f;
			return *this;
		}
		Color4f & operator/=(float f) {
			values[0] /= f;
			values[1] /= f;
			values[2] /= f;
			values[3] /= f;
			return *this;
		}

		std::string toString() const {
			std::ostringstream s;
			s << "Color4f(" << r() << "," << g() << "," << b() << "," << a() << ")";
			return s.str();
		}

		//! @name Serialization
		//@{
	public:
		friend std::ostream & operator<<(std::ostream & out, const Color4f & color) {
			return out << color.r() << ' ' << color.g() << ' ' << color.b() << ' ' << color.a();
		}
		friend std::istream & operator>>(std::istream & in, Color4f & color) {
			return in >> color.values[0] >> color.values[1] >> color.values[2] >> color.values[3];
		}
		//@}
};

inline Color4ub::Color4ub(const Color4f & other) {
	const uint_fast16_t tmpR = static_cast<uint_fast16_t>(256 * std::max(0.0f,other.r()));
	r(tmpR > 255 ? 255 : static_cast<uint8_t>(tmpR));
	const uint_fast16_t tmpG = static_cast<uint_fast16_t>(256 * std::max(0.0f,other.g()));
	g(tmpG > 255 ? 255 : static_cast<uint8_t>(tmpG));
	const uint_fast16_t tmpB = static_cast<uint_fast16_t>(256 * std::max(0.0f,other.b()));
	b(tmpB > 255 ? 255 : static_cast<uint8_t>(tmpB));
	const uint_fast16_t tmpA = static_cast<uint_fast16_t>(256 * std::max(0.0f,other.a()));
	a(tmpA > 255 ? 255 : static_cast<uint8_t>(tmpA));
}

}

//-------------

template <> struct std::hash<Util::Color4ub> {
	std::size_t operator()(const Util::Color4ub& color) const {
		std::size_t result = 0;
		Util::hash_combine(result, color.r());
		Util::hash_combine(result, color.g());
		Util::hash_combine(result, color.b());
		Util::hash_combine(result, color.a());
		return result;
	}
};

//-------------

template <> struct std::hash<Util::Color4f> {
	std::size_t operator()(const Util::Color4f& color) const {
		std::size_t result = 0;
		Util::hash_combine(result, color.r());
		Util::hash_combine(result, color.g());
		Util::hash_combine(result, color.b());
		Util::hash_combine(result, color.a());
		return result;
	}
};

#endif /* UTIL_COLOR_H_ */
