#ifndef __PATHUTIL_H__
#define __PATHUTIL_H__

#include <string>

namespace PathUtil
{
	std::string NativePath(const std::string& path);
	std::string UniversalPath(const std::string& path);
}

#endif
