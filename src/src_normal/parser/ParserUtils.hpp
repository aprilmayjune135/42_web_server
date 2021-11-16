#pragma once

# include <string>

namespace WebservUtility
{
	std::size_t findEndLine(std::string & leftover, std::string const & buffer, std::size_t index);
	void skipEndLine(std::string const & buffer, std::size_t & index);
}
