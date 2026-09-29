#pragma once
#include "DirScanner.h"
#include "WebPage.h"
#include "PageProcesser.h"
#include "InvertIndexProcesser.h"
#include "OffsetProcesser.h"

namespace wdcpp
{
class PageLib
{
public:
    explicit PageLib(const string &);
    ~PageLib()
    {
        using namespace std;
        cout << "~PageLib()" << endl;
    }

    void create();
    void store();

private:
    vector<WebPage> _pageList;                                              
    vector<pair<size_t, size_t>> _offsetTable;                              
    unordered_map<string, unordered_map<PageID, double>> _invertIndexTable; 
    DirScanner _dirScanner;
    PageProcesser _pageProcesser;
    InvertIndexProcesser _invertIndexProcesser;
    OffsetProcesser _offsetProcesser;
};
}; // namespace wdcpp
