#pragma once
#include "LRUCache.h"

namespace wdcpp
{
class CacheManager;

class CacheGroup
{
    friend class CacheManager;

public:
    CacheGroup(size_t);

    string getRecord(const string &);
    void insertRecord(const string &, const string &);
    void load(const string &);
    void dump(const string &);
    void update(const CacheGroup &);

private:
    LRUCache _mainCache;          
    LRUCache _pendingUpdateCache; // update cache
    bool _onlyRead;               // if only-read ,the new records cannot be insert into the two caches
};
}; // namespace wdcpp
