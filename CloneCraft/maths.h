#pragma once
#include <math.h>
#include <SFML/Graphics/Color.hpp>

namespace maths
{
	typedef unsigned char uint8;
	typedef unsigned short uint16;

	template<class num>
	struct genericVec3
	{
		genericVec3() {}

		genericVec3(num x, num y, num z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}
		num x = 0.f;
		num y = 0.f;
		num z = 0.f;

		genericVec3<num> operator * (num factor)
		{
			return genericVec3<num>(
				this->x * factor,
				this->y * factor,
				this->z * factor
			);
		}
	};

	typedef genericVec3<float> Vec3;
	typedef genericVec3<int> Vec3i;

	template<class num>
	struct Vec2
	{
		Vec2() {}

		Vec2(num x, num y)
		{
			this->x = x;
			this->y = y;
		}
		num x = 0.f;
		num y = 0.f;

		Vec2<num> operator * (num factor)
		{
			return genericVec3<num>(
				this->x * factor,
				this->y * factor
				);
		}
	};

	const float pi = 3.1415926535897932384626433832795;

	auto degreesToRadians(float degrees)
	{
		return degrees / 180 * pi;
	}

	void capDegrees(float& angle)
	{
		while (angle < 0)
		{
			angle += 360;
		}

		while (angle >= 360)
		{
			angle -= 360;
		}
	}
	
	float cosd(float angle)
	{
		return (float)cos(degreesToRadians(angle));
	}

	float sind(float angle)
	{
		return (float)sin(degreesToRadians(angle));
	}

	constexpr float cubeof(float x) { return x * x * x; };
	constexpr int cubeof(int x) { return x * x * x; };

	namespace coord
	{
		auto coordinateToIndex(int x, int y, int z, int size) -> int
		{
			return x * size * size + y * size + z;
		}

		auto indexToCoordinate(int i, int& x, int& y, int& z, int size)
		{
			x = i / size / size;
			i -= x * size * size;
			y = i / size;
			i -= y * size;
			z = i;
			i -= z;
		}
	}

	namespace color
	{
		uint8 normalizeChannel(uint8 channel)
		{
			return uint8( sqrt( float(channel) / 255 ) * 255.f );
		}

		void normalizeRef(sf::Color& color)
		{
			color = sf::Color(
				normalizeChannel(color.r),
				normalizeChannel(color.g),
				normalizeChannel(color.b),
				color.a
			);
		}
	}
}