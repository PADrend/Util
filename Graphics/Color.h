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

//! Representation of an RGBA color that is stored as four bytes.
class Color4ub {
	private:
		std::array<uint8_t, 4> values;

	public:
		Color4ub() : Color4ub{0, 0, 0, 255} {
		}
		Color4ub(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255) {
			set(_r, _g, _b, _a);
		}
		inline Color4ub(const Color4f & other);

		bool operator==(const Color4ub & other) const {
			return values == other.values;
		}
		bool operator!=(const Color4ub & other) const {
			return values != other.values;
		}
		void set(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) {
			setR(_r);
			setG(_g);
			setB(_b);
			setA(_a);
		}
		//! @return true iff the color is FULLY transparent
		bool isTransparent() const {
			return getA() == 0;
		}

		//!	@return true iff the color is FULLY opaque
		bool isOpaque() const {
			return getA() == 255;
		}

		const uint8_t * data() const {
			return values.data();
		}

		uint32_t getAsUInt() const {
			return *reinterpret_cast<const uint32_t *>(values.data());
		}

		uint8_t getR() const {
			return values[0];
		}
		uint8_t getG() const {
			return values[1];
		}
		uint8_t getB() const {
			return values[2];
		}
		uint8_t getA() const {
			return values[3];
		}
		void setR(uint8_t _r) {
			values[0] = _r;
		}
		void setG(uint8_t _g) {
			values[1] = _g;
		}
		void setB(uint8_t _b) {
			values[2] = _b;
		}
		void setA(uint8_t _a) {
			values[3] = _a;
		}

		std::string toString() const {
			std::ostringstream s;
			s << "Color4ub(" << static_cast<int>(getR()) << "," << static_cast<int>(getG()) << "," << static_cast<int>(getB()) << "," << static_cast<int>(getA()) << ")";
			return s.str();
		}

		//! Return a new color with the absolute difference between the two given colors.
		static Color4ub createDifferenceColor(const Color4ub & first, const Color4ub & second) {
			Color4ub result;
			if(first.getR() > second.getR()) {
				result.setR(first.getR() - second.getR());
			} else {
				result.setR(second.getR() - first.getR());
			}
			if(first.getG() > second.getG()) {
				result.setG(first.getG() - second.getG());
			} else {
				result.setG(second.getG() - first.getG());
			}
			if(first.getB() > second.getB()) {
				result.setB(first.getB() - second.getB());
			} else {
				result.setB(second.getB() - first.getB());
			}
			if(first.getA() > second.getA()) {
				result.setA(first.getA() - second.getA());
			} else {
				result.setA(second.getA() - first.getA());
			}
			return result;
		}

		//! @name Serialization
		//@{
	public:
		friend std::ostream & operator<<(std::ostream & out, const Color4ub & color) {
			return out << static_cast<uint32_t>(color.getR()) << ' '
				   << static_cast<uint32_t>(color.getG()) << ' '
				   << static_cast<uint32_t>(color.getB()) << ' '
				   << static_cast<uint32_t>(color.getA());
		}
		friend std::istream & operator>>(std::istream & in, Color4ub & color) {
			uint32_t r;
			uint32_t g;
			uint32_t b;
			uint32_t a;
			in >> r >> g >> b >> a;
			color.set(r, g, b, a);
			return in;
		}
		//@}
};

//! Representation of an RGBA color that is stored as four floats.
class Color4f {
	private:
		float r;
		float g;
		float b;
		float a;

	public:
		Color4f() : r(0.0f), g(0.0f), b(0.0f), a(1.0f) {
		}
		Color4f(float _r, float _g, float _b, float _a = 1.0f) : r(_r), g(_g), b(_b), a(_a) {
		}
		Color4f(const Color4ub & other) : r((1.0f / 255.0f) * other.getR()), g((1.0f / 255.0f) * other.getG()), b((1.0f / 255.0f) * other.getB()), a((1.0f / 255.0f) * other.getA()) {
		}

		Color4f(const std::vector<float> & values) {
			assert(values.size() == 4);
			r = values[0];
			g = values[1];
			b = values[2];
			a = values[3];
		}

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
				set(w * first.r + v * second.r,
					w * first.g + v * second.g,
					w * first.b + v * second.b,
					w * first.a + v * second.a);
			}
		}

		bool operator==(const Color4f & other) const {
			return r == other.r && g == other.g && b == other.b && a == other.a;
		}
		bool operator!=(const Color4f & other) const {
			return r != other.r || g != other.g || b != other.b || a != other.a;
		}
		void set(float _r, float _g, float _b, float _a) {
			r = _r;
			g = _g;
			b = _b;
			a = _a;
		}
		//! @return true iff the color is FULLY transparent
		bool isTransparent() const {
			return a <= 0.0f;
		}

		//! @return true iff the color is FULLY opaque
		bool isOpaque() const {
			return a >= 1.0f;
		}

		const float * data() const {
			return &r;
		}

		const float & getR() const {
			return r;
		}
		const float & getG() const {
			return g;
		}
		const float & getB() const {
			return b;
		}
		const float & getA() const {
			return a;
		}
		void setR(float _r) {
			r = _r;
		}
		void setG(float _g) {
			g = _g;
		}
		void setB(float _b) {
			b = _b;
		}
		void setA(float _a) {
			a = _a;
		}

		const Color4f operator+(const Color4f & other) const {
			return Color4f(r + other.r, g + other.g, b + other.b, a + other.a);
		}
		const Color4f operator-(const Color4f & other) const	{
			return Color4f(r - other.r, g - other.g, b - other.b, a - other.a);
		}
		const Color4f operator*(float f) const {
			return Color4f(r * f, g * f, b * f, a * f);
		}
		const Color4f operator/(float f) const {
			return Color4f(r / f, g / f, b / f, a / f);
		}

		Color4f & operator+=(const Color4f & other) {
			r += other.r;
			g += other.g;
			b += other.b;
			a += other.a;
			return *this;
		}
		Color4f & operator-=(const Color4f & other) {
			r -= other.r;
			g -= other.g;
			b -= other.b;
			a -= other.a;
			return *this;
		}
		Color4f & operator*=(float f) {
			r *= f;
			g *= f;
			b *= f;
			a *= f;
			return *this;
		}
		Color4f & operator/=(float f) {
			r /= f;
			g /= f;
			b /= f;
			a /= f;
			return *this;
		}

		std::string toString() const {
			std::ostringstream s;
			s << "Color4f(" << r << "," << g << "," << b << "," << a << ")";
			return s.str();
		}

		//! @name Serialization
		//@{
	public:
		friend std::ostream & operator<<(std::ostream & out, const Color4f & color) {
			return out << color.r << ' ' << color.g << ' ' << color.b << ' ' << color.a;
		}
		friend std::istream & operator>>(std::istream & in, Color4f & color) {
			return in >> color.r >> color.g >> color.b >> color.a;
		}
		//@}
};

inline Color4ub::Color4ub(const Color4f & other) {
	const uint_fast16_t tmpR = static_cast<uint_fast16_t>(256 * other.getR());
	setR(tmpR > 255 ? 255 : static_cast<uint8_t>(tmpR));
	const uint_fast16_t tmpG = static_cast<uint_fast16_t>(256 * other.getG());
	setG(tmpG > 255 ? 255 : static_cast<uint8_t>(tmpG));
	const uint_fast16_t tmpB = static_cast<uint_fast16_t>(256 * other.getB());
	setB(tmpB > 255 ? 255 : static_cast<uint8_t>(tmpB));
	const uint_fast16_t tmpA = static_cast<uint_fast16_t>(256 * other.getA());
	setA(tmpA > 255 ? 255 : static_cast<uint8_t>(tmpA));
}

}

#endif /* UTIL_COLOR_H_ */
