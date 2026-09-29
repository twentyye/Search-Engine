#include "CacheManager.h"
#include "Configuration.h"

#include <iostream>

namespace wdcpp
{
CacheManager *CacheManager::_pInstance = CacheManager::getInstance(); 

CacheManager *CacheManager::getInstance()
{
    if (_pInstance == nullptr)
    {
        _pInstance = new CacheManager();
        atexit(destroy);
    }
    return _pInstance;
}

CacheGroup &CacheManager::getCacheGroup(size_t idx)
{
    return _caches[idx];
}

CacheManager::CacheManager()
    : _cacheNums(stoul(Configuration::getInstance()->getConfigMap()["workernum"])),
      _maxRecord(stoul(Configuration::getInstance()->getConfigMap()["recordnum"])),
      _caches(_cacheNums, _maxRecord) 
{
  
    // for (auto &group : _caches)
    // {
    //     group.load(Configuration::getInstance()->getConfigMap()["LRU"]);
    // }
}

void CacheManager::destroy()
{
    using namespace std;
    cout << "void CacheManager::destroy()" << endl;
    if (_pInstance)
    {
        delete _pInstance;
        _pInstance = nullptr;
    }
}

void CacheManager::sync()
{
    using namespace std;
    cout << "timer thread: start sync" << endl;

    auto &first_group = _caches[0];
    first_group._onlyRead = true;
    for (auto &group : _caches)
    {
        auto &pendingCache = group._pendingUpdateCache;
#ifdef __DEBUG__
        // printf("\t(File:%s, Func:%s(), Line:%d)\n", __FILE__, __FUNCTION__, __LINE__);
        cout << "group._pengdingCache.size() = " << pendingCache.size() << endl;
#endif
        for (auto &record : pendingCache._resultList)
        {
            first_group.insertRecord(record.first, record.second);
#ifdef __DEBUG__
            // printf("\t(File:%s, Func:%s(), Line:%d)\n", __FILE__, __FUNCTION__, __LINE__);
            cout << "first_group._mainCache.size() = " << first_group._mainCache.size() << endl;
#endif
        }
        pendingCache.clear();
    }

    // first_group.dump(Configuration::getInstance()->getConfigMap()["LRU"]);

    for (auto &group : _caches)
    {
        group._onlyRead = true;
        group.update(first_group);
        group._onlyRead = false;
    }
    first_group._onlyRead = false;

#ifdef __DEBUG__
    printf("\t(File:%s, Func:%s(), Line:%d)\n", __FILE__, __FUNCTION__, __LINE__);
    cout << "timer thread: end sync" << endl;
#endif
}
}; // namespace wdcpp
