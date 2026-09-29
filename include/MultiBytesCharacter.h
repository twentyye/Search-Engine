#pragma once

namespace wdcpp
{
/**
 *  Get the number of bytes occupied by a multi-byte character
 */
inline size_t nBytesCode(const char ch)
{
    if (ch & (1 << 7))
    {
        int nBytes = 1;
        for (int idx = 0; idx != 6; ++idx)
        {
            if (ch & (1 << (6 - idx)))
            {
                ++nBytes;
            }
            else
                break;
        }
        return nBytes;
    }
    return 1;
}

size_t howManyBytesWithNCharacter(const char *p, size_t limit, size_t N)
{
    size_t totalChar = 0;
    for (size_t i = 0; i < N; ++i)
    {
        int nBytes = nBytesCode(p[0]);
        p += nBytes;
        if (totalChar + nBytes > limit)
            return limit;
        totalChar += nBytes;
    }
    return totalChar;
}

vector<size_t> getPosPerCharactor(const string &str, size_t end)
{
    vector<size_t> res;
    int totalChar = 0;
    const char *p = str.c_str();
    for (size_t idx = 0; idx < end; ++idx)
    {
        int nBytes = nBytesCode(p[0]);
        p += nBytes;
        res.push_back(totalChar);
        totalChar += nBytes;
        idx += (nBytes - 1);
    }
    return res;
}

}; // namespace wdcpp
