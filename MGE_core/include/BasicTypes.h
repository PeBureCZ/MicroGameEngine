#pragma once

#include <vector>
#include <cmath>
#include <type_traits>

namespace tsmType
{

	struct Color_RGBA
	{
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
		constexpr Color_RGBA()
			: r(255), g(0), b(0), a(255) {}; //red by default
		constexpr Color_RGBA(unsigned char R, unsigned char G, unsigned char B, unsigned char A = 255)
			: r(R), g(G), b(B), a(A) {};
	};

	template<typename T>
	struct Size
	{
		T width;
		T height;
		constexpr Size(T newWidth = 0.0f, T newHeight = 0.0f)
		{
			width = newWidth;
			height = newHeight;
		}

		constexpr bool operator ==(const Size& other) const noexcept
		{
			return width == other.width && height == other.height;
		}

		constexpr bool operator !=(const Size& other) const noexcept
		{
			return width != other.width || height != other.height;
		}
	};

	template<typename T>
	class Point
	{
	public:
		T x;
		T y;
		constexpr Point(T newX = T{}, T newY = T{})
			: x(newX), y(newY)
		{}

		inline Point<int> asInt() const noexcept
		{
			return Point<int>(static_cast<int>(x), static_cast<int>(y));
		}

		inline Point<float> asFloat() const noexcept
		{
			return Point<float>(static_cast<float>(x), static_cast<float>(y));
		}

		bool operator ==(const Point<T>& other) const noexcept
		{
			return x == other.x && y == other.y;
		}

		Point<T> operator+(const Point<T>& other) const noexcept
		{
			return Point<T>(x + other.x, y + other.y);
		}

		Point<T> operator -(const Point<T>& other) const noexcept
		{
			return Point<T>(x - other.x, y - other.y);
		}

		Point<T>& operator +=(const Point<T>& other) noexcept
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		Point<T>& operator /(float divider) noexcept
		{
			x /= divider;
			y /= divider;
			return *this;
		}

		Point<T>& operator *(float multiplier) noexcept
		{
			x *= multiplier;
			y *= multiplier;
			return *this;
		}

		Point<T>& operator -=(const Point<T>& other) noexcept
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		Point<T>& operator /=(const T divider) noexcept
		{
			x /= divider;
			y /= divider;
			return *this;
		}

		Point<T>& operator *=(const T multiplier) noexcept
		{
			x *= multiplier;	
			y *= multiplier;
			return *this;
		}

		template<typename U>
		Point<T>& operator=(const Point<U>& other) noexcept
		{
			x = static_cast<T>(other.x);
			y = static_cast<T>(other.y);
			return *this;
		}
	};

	template<typename S, typename T>
	Point<std::common_type_t<T, S>> operator*(S num, const Point<T>& p) noexcept
	{
		using R = std::common_type_t<T, S>;
		return Point<R>(p.x * num, p.y * num);
	}

	template<typename T>
	class Line
	{
	public:
		Point<T> p1;
		Point<T> p2;

		constexpr Line() = default;

		constexpr Line(const Point<T>& a, const Point<T>& b)
			: p1(a), p2(b)
		{
		}

		// Length of the line segment
		T length() const noexcept
		{
			auto dx = p2.x - p1.x;
			auto dy = p2.y - p1.y;
			return static_cast<T>(std::sqrt(dx * dx + dy * dy));
		}

		// Direction vector (p2 - p1)
		Point<T> direction() const noexcept
		{
			return p2 - p1;
		}

		// Interpolation: t in [0,1]
		Point<T> pointAt(T t) const noexcept
		{
			return p1 + (p2 - p1) * t;
		}

		// Midpoint convenience
		Point<T> midpoint() const noexcept
		{
			return pointAt(static_cast<T>(0.5));
		}
	};

	using FLine = Line<float>;
	using DLine = Line<double>;
	using ILine = Line<int>;


	template<typename T>
	class Curve
	{
	public:
		Curve(std::vector<Point<T>> newPoints)
		: points(std::move(newPoints))
		{

		}
	private:
		std::vector<Point<T>> points;
	};
}

using FPoint = tsmType::Point<float>;
using IPoint = tsmType::Point<int>;
using DPoint = tsmType::Point<double>;

namespace tsmBasic
{
	[[nodiscard]] inline float getDistance(FPoint pointA, FPoint pointB)
	{
		tsmType::FLine line(pointA, pointB);
		return line.length();
	}
}
