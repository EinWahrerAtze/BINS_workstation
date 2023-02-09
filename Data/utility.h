//
//  utility.h
//  BINS_workstation
//
//  Created by Denis Fedorov on 02.02.2023.
//

#pragma once
#include <format>
#include <string>
#include <string_view>

// Unility namespace. Contains enum class 'text' to manipulates with output text and 'apply' function,
// which takes two arguments - template value and colour code - and then return std::string with colored text.
// The specialization of std::formatter allows to pass the class as argument to std::format().

namespace ws::data::utility
{
	enum class text : uint32_t
	{
		DEFAULT = 0,
		BOLD	= 1,
		RED		= 91,
		GREEN	= 92
		// no need for other colours for now
	};

	template <typename T>
	std::string apply(const T & object, text text)
	{
		return std::format("{}{}{}", text, object, utility::text::DEFAULT);
	}
}

template <>
struct std::formatter<ws::data::utility::text> : std::formatter<std::string_view>
{
	template <typename T>
	auto format(ws::data::utility::text text, T & t)
	{
		return std::formatter<std::string_view>::format("\u001b[" + std::to_string(static_cast<uint32_t>(text)) + "m", t);
	}
};
