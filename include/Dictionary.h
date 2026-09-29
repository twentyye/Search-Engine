#pragma once
#include <map>
#include <vector>
#include <string>
#include <set>
#include <iostream>

using std::cout;
using std::endl;
using std::map;
using std::pair;
using std::set;
using std::string;
using std::vector;

namespace wdcpp
{
class Dictionary
{
public:
    static Dictionary *getInstance();
    static void destory();

    ~Dictionary(){};
    /* void initDict(const string &dictPath); */
    const vector<pair<string, int>> &getDict();
    const map<string, set<int>> &getIndexTable();
    void print()
    {
        for (auto it = _dict.begin(); it != _dict.end(); ++it)
        {
            cout << it->first << "\t" << it->second << endl;
        }
    }
    /* vector<string> doQuery(const string &key); */
    /* string doQuery(const string &key); */
    // vector<CandidateResult> doQuery(const string &key);

private:
    void initDict();
    void initIndex();
#if 0
    size_t nBytesCode(const char ch);
    size_t length(const string &str);
    int editDistance(const std::string & lhs, const std::string &rhs);

    bool dis_compare(CandidateResult &lhs, CandidateResult &rhs);
#endif
private:
    Dictionary(){}; //Singleton class
    static Dictionary *_singletonDict;
    vector<pair<string, int>> _dict;
    map<string, set<int>> _index; // load  dictionary file and  index file separately 
    /* vector<string> _isVisited; */
};
};
