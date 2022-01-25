#pragma once

#include <comdef.h>
#include <sstream>
#include <unordered_map>
#include <iostream>

namespace Utils
{
    std::unordered_map<std::wstring, std::wstring> splitData(const std::wstring &data);

    std::wstring formatData(const std::vector<std::pair<std::wstring, std::wstring>> &data);

    inline bool checkResult(const char *file, const long line, const char *func, const HRESULT &hr)
    {
        if (FAILED(hr))
        {
            std::cerr << file << line << func << L":\n\t\t\t" << hr << std::endl;
            return false;
        }
        return true;
    }

    std::wstring formatWinError(unsigned long errorCode);
};

#define DN_GROUP_NAME L"desktop-notifications"

#define DN_CHECK_RESULT(hr) Utils::checkResult(__FILE__, __LINE__, __FUNCSIG__, hr)

#define DN_RETURN_ON_ERROR(hr)      \
    do                              \
    {                               \
        HRESULT _tmp = hr;          \
        if (!DN_CHECK_RESULT(_tmp)) \
        {                           \
            return _tmp;            \
        }                           \
    } while (false)
