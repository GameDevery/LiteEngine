#ifndef COLOR_H
#define COLOR_H

#include "Core/Interfaces/Object.h"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

class ENGINE_API Color : public Object
{
public:
	static Color White;
	static Color Black;
	static Color Red;
	static Color Green;
	static Color Blue;

	unsigned char r, g, b, a;

public:
	Color ();
	Color (const Color& other);
	Color (unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
	Color (const glm::vec3& color);
	Color (const glm::vec4& color);

	glm::vec3 ToVector3 () const;
	glm::vec4 ToVector4 () const;
};

#endif