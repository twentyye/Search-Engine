#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <map>
#include <utility>
using std::map;
using std::pair;
using std::string;
using std::unordered_map;
using std::vector;

namespace wdcpp
{
class WebPage;
using PageID = long;
class InvertIndexProcesser
{
public:
    explicit InvertIndexProcesser(vector<WebPage> &, unordered_map<string, unordered_map<PageID, double>> &);
    ~InvertIndexProcesser()
    {
        using namespace std;
        cout << "~InvertIndexProcesser()" << endl;
    }

    void process();

    void printInvertIndexTable();

private:
    vector<WebPage> &_pageList;
    unordered_map<string, unordered_map<PageID, double>> &_invertIndexTable;
    vector<double> _sumOfWeightsPerPage; 
};
}; // namespace wdcpp
