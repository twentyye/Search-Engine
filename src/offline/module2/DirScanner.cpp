#include "DirScanner.h"

#include <ErrorCheck>
#include <sys/types.h>
#include <dirent.h>
#include <iostream>

namespace wdcpp
{
DirScanner::DirScanner(const string &dirPath)
    : _dirPath(dirPath)
{
    // std::cout << "DirScanner()\n";
    traverse();
    // for (auto &item : _filePathList)
    // {
    //     std::cout << item << "\n";
    // }
}

/**
 *  Open the directory at `dirPath` and recursively retrieve the path information for all files within it
 */
void DirScanner::traverse()
{
    traverse(_dirPath);
}

void DirScanner::traverse(const string &dirName)
{
    DIR *fDir = opendir(dirName.c_str());
    ERROR_CHECK(fDir, nullptr, "opendir");

    struct dirent *pDirent;
    while (NULL != (pDirent = readdir(fDir)))
    {
        if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0)
            continue;
        else if (pDirent->d_type == 8)
        {
            string filePath;
            filePath = dirName + "/" + pDirent->d_name;
            _filePathList.push_back(filePath);
        }
        else if (pDirent->d_type == 4)
        {
            string strNextdir = dirName + "/" + pDirent->d_name;
            traverse(strNextdir);
        }
    }

    closedir(fDir);
}

vector<string> &DirScanner::getFilePathList()
{
    return _filePathList;
}
}; // namespace wdcpp
