#pragma once
#include "CacheGroup.h"

#include <vector>
using std::vector;

namespace wdcpp
{
class CacheManager
{
public:
    static CacheManager *getInstance();

    CacheGroup &getCacheGroup(size_t);

    void sync();

private:
    CacheManager();
    ~CacheManager() {}

    static void destroy();

private:
    size_t _cacheNums;          // total numbers of working threads 
    size_t _maxRecord;          // the maximum number of records in an LRU cache 
    vector<CacheGroup> _caches; // cache group for all threads
    static CacheManager *_pInstance;
};
}; // namespace wdcpp
