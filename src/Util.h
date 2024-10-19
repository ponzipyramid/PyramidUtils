#pragma once

namespace Util
{
	inline std::string Upper(std::string a_str)
	{
		std::string data{ a_str };
		std::transform(data.begin(), data.end(), data.begin(),
			[](unsigned char c) { return (char)std::toupper(c); });

		return data;
	}

	inline std::string HexString(int a_num)
	{
		std::stringstream stream;
		stream << std::hex << a_num;
		std::string result(stream.str());

		result = Upper(result);

		return "0x" + result;
	}
}