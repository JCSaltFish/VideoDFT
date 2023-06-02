#include <algorithm>

#include "pathutil.h"

namespace PathUtil
{
    std::string NativePath(const std::string& path)
    {
        std::string res(path);
#if defined(_WIN32)
        std::replace(res.begin(), res.end(), '/', '\\');
#else
        std::replace(res.begin(), res.end(), '\\', '/');
#endif
        return res;
    }

    std::string UniversalPath(const std::string& path)
    {
        std::string res(path);
        std::replace(res.begin(), res.end(), '\\', '/');
        return res;
    }
}
