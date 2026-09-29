#pragma once

#include <iostream>
#include <vector>
#include <string>
using std::string;
using std::vector;

namespace wdcpp
{
/*************************************************************
 *
 *   Directory scanning class
 *
 *************************************************************/
class DirScanner
{
public:
    explicit DirScanner(const string &);
    ~DirScanner()
    {
        using namespace std;
        cout << "~DirScanner()" << endl;
    }

    void traverse();
    vector<string> &getFilePathList();

private:
    void traverse(const string &);

private:
    vector<string> _filePathList; // the path of _dirPath all files
    string _dirPath;
};
}; // namespace wdcpp
