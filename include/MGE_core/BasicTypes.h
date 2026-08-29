#pragma once

#include <vector>
#include <string>
#include <cmath>
#include <optional>
#include <charconv>
#include <type_traits>

namespace mgeType
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

		[[nodiscard]] constexpr Size<int> asInt() const noexcept
		{
			return Size<int>(static_cast<int>(width), static_cast<int>(height));
		}

		[[nodiscard]] constexpr Size<float> asFloat() const noexcept
		{
			return Size<float>(static_cast<float>(width), static_cast<float>(height));
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

		[[nodiscard]] constexpr Point<int> asInt() const noexcept
		{
			return Point<int>(static_cast<int>(x), static_cast<int>(y));
		}

		[[nodiscard]] constexpr Point<float> asFloat() const noexcept
		{
			return Point<float>(static_cast<float>(x), static_cast<float>(y));
		}

		constexpr bool operator ==(const Point<T>& other) const noexcept
		{
			return x == other.x && y == other.y;
		}

		[[nodiscard]] constexpr Point<T> operator+(const Point<T>& other) const noexcept
		{
			return Point<T>(x + other.x, y + other.y);
		}

		[[nodiscard]] constexpr Point<T> operator -(const Point<T>& other) const noexcept
		{
			return Point<T>(x - other.x, y - other.y);
		}

		constexpr Point<T>& operator +=(const Point<T>& other) noexcept
		{
			x += other.x;
			y += other.y;
			return *this;
		}

		[[nodiscard]] constexpr Point<T> operator /(T divider) const noexcept
		{
			return Point<T>(x / divider, y / divider);
		}

		[[nodiscard]] constexpr Point<T> operator *(T multiplier)	const noexcept
		{
			return Point<T>(x * multiplier, y * multiplier);
		}

		constexpr Point<T>& operator -=(const Point<T>& other) noexcept
		{
			x -= other.x;
			y -= other.y;
			return *this;
		}

		constexpr Point<T>& operator /=(const T divider) noexcept
		{
			x /= divider;
			y /= divider;
			return *this;
		}

		constexpr Point<T>& operator *=(const T multiplier) noexcept
		{
			x *= multiplier;	
			y *= multiplier;
			return *this;
		}

		template<typename U>
		constexpr Point<T>& operator=(const Point<U>& other) noexcept
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

using FPoint = mgeType::Point<float>;
using IPoint = mgeType::Point<int>;
using DPoint = mgeType::Point<double>;

using FSize = mgeType::Size<float>;
using ISize = mgeType::Size<int>;
using DSize = mgeType::Size<double>;

using FLine = mgeType::Line<float>;
using DLine = mgeType::Line<double>;
using ILine = mgeType::Line<int>;

namespace tsmBasic
{
	constexpr double PI = 3.14159265358979323846;

	[[nodiscard]] inline float getDistance(const FPoint& pointA, const FPoint& pointB)
	{
		FLine line(pointA, pointB);
		return line.length();
	}

	[[nodiscard]] inline std::string convertFPointToStr(const FPoint& point)
	{
		return { std::to_string(point.x) + ":" + std::to_string(point.y)};
	}

	/**calculate rotation from two points in wolrd or relative coordination*/
	inline float makeRotation(FPoint pointA, FPoint pointB)
	{
		float dx = pointB.x - pointA.x;
		float dy = pointB.y - pointA.y;
		float angleRadians = std::atan2(dy, dx);
		return angleRadians * static_cast<float>(180.0 / PI);
	}

	[[nodiscard]] inline FPoint getRotatedPointArountPivot(const FPoint& point, const FPoint& pivot, const double angle)
	{
		float radian = static_cast<float>(angle * PI / 180.0);

		float newX = round((point.x - pivot.x) * cos(radian) - (point.y - pivot.y) * sin(radian) + pivot.x);
		float newY = round((point.x - pivot.x) * sin(radian) + (point.y - pivot.y) * cos(radian) + pivot.y);
		return { newX, newY };
	}

	/**from format "floatX:floatY"*/
	[[nodiscard]] inline std::optional<FPoint> convertStrToFPoint(std::string_view text)
	{
		const auto colon = text.find(':');
		if (colon == std::string_view::npos)
			return std::nullopt;

		FPoint point;

		// Parse X
		auto result = std::from_chars
			(
				text.data(),
				text.data() + colon,
				point.x
			);

		if (result.ec != std::errc{} || result.ptr != text.data() + colon)
			return std::nullopt;

		// Parse Y
		result = std::from_chars
			(
				text.data() + colon + 1,
				text.data() + text.size(),
				point.y
			);

		if (result.ec != std::errc{} || result.ptr != text.data() + text.size())
			return std::nullopt;

		return point;
	}
}
