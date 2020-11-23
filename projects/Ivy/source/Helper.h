#pragma once
#include "Types.h"

namespace Ivy
{
	inline void RemoveUntilCharacterInString(String& s, char c, bool includeCharacter = true)
	{
		auto pos = s.rfind(c);
		if (pos != std::string::npos)
		{
			s = (includeCharacter) ? s.substr(pos + 1) : s.substr(pos);
		}
	}

	inline void RemoveFromCharacterInStringToEnd(String& s, char c, bool includeCharacter = true)
	{
		auto pos = s.rfind('.');
		if (pos != std::string::npos)
		{
			s = (includeCharacter) ? s.erase(pos) : s.erase(pos + 1);
		}
	}

	inline Vec3 Lerp(const Vec3& A, const Vec3& B, float t)
	{
		return A + (B - A) * t;
	}
}