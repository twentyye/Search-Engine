#pragma once
#include "CompareSimhash.h"
#include "SplitTool.h"

#include <iostream>
#include <vector>
#include <string>
using std::string;
using std::vector;

namespace wdcpp
{
class WebPage;
class PageProcesser
{
public:
    explicit PageProcesser(vector<string> &, vector<WebPage> &);
    ~PageProcesser()
    {
        using namespace std;
        cout << "~PageProcesser()" << endl;
    }

    void process();

    void printPageList();
    void printStopWords();

private:
    void loadStopWords();

    void loadPageFromXML();  
    void cutRedundantPage(); 
    void countFrequence();   

private:
    vector<string> &_filePathList;
    vector<WebPage> &_nonRepetivepageList;
    vector<WebPage> _pageList;
    vector<string> _stopWords;
    // vector<bool> _isDelete;
    CompareSimhash _comparePages; 
    SplitTool _splitTool;         // tool of split words  
};
}; // namespace wdcpp
