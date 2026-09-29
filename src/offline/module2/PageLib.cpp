#include "PageLib.h"
#include "Configuration.h"

#include <fstream>
#include <ErrorCheck>
using std::ofstream;

namespace wdcpp
{
PageLib::PageLib(const string &dirPath)
    : _dirScanner(dirPath),
      _pageProcesser(_dirScanner.getFilePathList(), _pageList),
      _invertIndexProcesser(_pageList, _invertIndexTable),
      _offsetProcesser(_pageList, _offsetTable)
{
}

void PageLib::create()
{
    _pageProcesser.process(); 

    _invertIndexProcesser.process(); 

    _offsetProcesser.process(); 
}

void PageLib::store()
{
    string ripepagePath = Configuration::getInstance()->getConfigMap()["ripepage"];
    string offsetPath = Configuration::getInstance()->getConfigMap()["offset"];
    string InvertIndex = Configuration::getInstance()->getConfigMap()["invertIndex"];

    using namespace std;

    // Writing a web library
    ofstream ofs1(ripepagePath);
    if (!ofs1)
    {
        ERROR_PRINT("can not open ripepage.dat");
        exit(EXIT_FAILURE);
    }
    PageID ID = 0;
    for (auto &page : _pageList)
    {
        page.setPageID(ID++);
        page.setPageDoc();
        ofs1 << page.getDoc();
    }
    ofs1.close();

    // Write the inverted index library
    ofstream ofs2(InvertIndex);
    if (!ofs2)
    {
        ERROR_PRINT("can not open offset.dat");
        exit(EXIT_FAILURE);
    }
    for (auto &wordPair : _invertIndexTable)
    {
        ofs2 << wordPair.first << " ";
        for (auto &pagePair : wordPair.second)
        {
            ofs2 << pagePair.first << " "
                 << pagePair.second << " ";
        }
        ofs2 << "\n";
    }
    ofs2.close();

    // Write offset library
    ofstream ofs3(offsetPath);
    if (!ofs3)
    {
        ERROR_PRINT("can not open invertIndex.dat");
        exit(EXIT_FAILURE);
    }
    for (size_t idx = 0; idx < _offsetTable.size(); ++idx)
    {
        ofs3 << _pageList[idx].getDocId() << " "
             << _offsetTable[idx].first << " "
             << _offsetTable[idx].second << " "
             << "\n";
    }
    ofs3.close();

    cout << "store succeed!" << endl;
}
}; // namespace wdcpp
