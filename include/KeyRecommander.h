#pragma once

#include "Dictionary.h"
#include <string>
#include <queue>
#include <vector>
#include <set>

using std::priority_queue;
using std::set;
using std::vector;

namespace wdcpp
{

class MyResult
{
public:
    MyResult(string word, int freq, int dist = 999)
        : _word(word), _freq(freq), _dist(dist)
    {
    }
    string getWord() const
    {
        return _word;
    }
    int getFreq() const
    {
        return _freq;
    }
    int getDist() const
    {
        return _dist;
    }
    void setDist(int dist)
    {
        _dist = dist;
    }

private:
    string _word; 
    int _freq;    
    int _dist;    
};

struct MyCompare
{
    bool operator()(const MyResult &lhs, const MyResult &rhs)
    {
        if (lhs.getDist() > rhs.getDist())
        {
            return true;
        }
        else if (lhs.getDist() == rhs.getDist())
        {
            if (lhs.getFreq() < rhs.getFreq()) 
                return true;
            else if (lhs.getFreq() == rhs.getFreq())
            {
                if (lhs.getWord() > rhs.getWord()) //lexicographic sorting
                    return true;
                else
                    return false;
            }
            else
                return false;
        }
        else
            return false;
    }
};

class KeyRecommander
{
public:
    KeyRecommander() = default;
    ~KeyRecommander() = default;
    string doQuery(const string &); 

private:
    void queryIndexTable();                                                                                                
    void statistic(const string &queryWord, set<int> &, priority_queue<MyResult, vector<MyResult>, MyCompare> &resultQue); 
    size_t nBytesCode(const char ch);
    size_t length(const std::string &str);
    int triple_min(const int &a, const int &b, const int &c);
    int distance(const string &, const string &rhs); //calculate the minimum edit distance 

    string serializeForNoting();
    string serialize(const vector<string> &);
};
};
