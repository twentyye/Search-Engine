#include "CacheGroup.h"

namespace wdcpp
{
CacheGroup::CacheGroup(size_t capacity)
    : _mainCache(capacity),
      _pendingUpdateCache(capacity),
      _onlyRead(false)
{
}

string CacheGroup::getRecord(const string &query)
{
    return _mainCache.getRecord(query);
}

void CacheGroup::insertRecord(const string &query, const string &result)
{
    _mainCache.insertRecord(query, result);
    if (!_onlyRead) 
        _pendingUpdateCache.insertRecord(query, result);
}

// void CacheGroup::load(const string &path)
// {
// }
// void CacheGroup::dump(const string &path)
// {
// }

void CacheGroup::update(const CacheGroup &group)
{
    _mainCache.update(group._mainCache);
    _pendingUpdateCache.update(group._pendingUpdateCache); 
}
}; // namespace wdcpp
