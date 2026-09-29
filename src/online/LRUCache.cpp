#include "LRUCache.h"
#include "Thread.h"

#include <iostream>
#include <fstream>
#include <sstream>
using std::ifstream;
using std::istringstream;
using std::ofstream;

namespace wdcpp
{
extern __thread size_t __thread_id; 

LRUCache::LRUCache(size_t capacity)
    : _capacity(capacity)
{
}

bool LRUCache::isHit(const string &query)
{
    return _hashMap.count(query) > 0;
}

string LRUCache::getRecord(const string &query)
{
    if (!isHit(query))
        return "";
    std::cout << "No." << __thread_id << " cache hit!" << std::endl;
   
    _resultList.splice(_resultList.begin(), _resultList, _hashMap[query]);
    return _hashMap[query]->second;
}

void LRUCache::insertRecord(const string &query, const string &result)
{
    if (isHit(query))
    {
        _resultList.splice(_resultList.begin(), _resultList, _hashMap[query]);
    }
    else
    {
        _resultList.push_front({query, result}); 
        _hashMap[query] = _resultList.begin();   
        if (_resultList.size() > _capacity)      
        {
            auto back_key = _resultList.back().first; 
            _resultList.pop_back();                 
            _hashMap.erase(back_key);                 
        }
    }
}

// void LRUCache::load(const string &path)
// {
//     ifstream ifs(path);
//     if (!ifs)
//     {
//         std::cout << "LRU cache file open failed" << std::endl;
//     }

//     string line;
//     while (getline(ifs, line))
//     {
//         istringstream iss(line);
//         string key, value;
//         iss >> key >> value;
//         insertRecord(key, value);
//     }

//     ifs.close();
// }

// void LRUCache::dump(const string &path)
// {
//     ofstream ofs(path);
//     if (!ofs)
//     {
//         std::cout << "LRU cache file open failed" << std::endl;
//     }

//     auto it = _resultList.rbegin();
//     int count = 0;
//     for (; it != _resultList.rend(); ++it)
//     {
//         ofs << it->first << " " << it->second << std::endl;
//     }

//     ofs.close();
// }

void LRUCache::clear()
{
    _resultList.clear();
    _hashMap.clear();
}

void LRUCache::update(const LRUCache &cache)
{
    for (auto it = cache._resultList.rbegin(); it != cache._resultList.rend(); ++it)
    {
        insertRecord(it->first, it->second);
    }
}

size_t LRUCache::size() const
{
    return _resultList.size();
}
}; // namespace wdcpp
