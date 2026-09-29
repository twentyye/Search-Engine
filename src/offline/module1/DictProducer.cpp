#include "DictProducer.h"
#include "SplitTool.h"
#include "Configuration.h"
#include <ErrorCheck>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <iostream>

using std::cout;
using std::endl;
using std::ifstream;
using std::istringstream;
using std::ofstream;

using namespace wdcpp;

DictProducer::DictProducer(const string &dir)
    : _endir(dir)
{
    string enDictPath = Configuration::getInstance()->getConfigMap()["enDict"];
    string enDictIndex = Configuration::getInstance()->getConfigMap()["enDicIndex"];
    string enStopDictPath = Configuration::getInstance()->getConfigMap()["enStop"];

    loadStopWord(enStopDictPath);

    buildEnDict();
    storeDict(enDictPath.c_str());
    buildIndex();
    storeIndex(enDictIndex.c_str());
    cout << "Build En Dict and DictIndex OK" << endl;
} 

DictProducer::DictProducer(const string &dir, SplitTool *splitTool)
    : _splitTool(splitTool), _dir(dir)
{
    string dictPath = Configuration::getInstance()->getConfigMap()["dict"];
    string dictIndex = Configuration::getInstance()->getConfigMap()["dicIndex"];
    string cnStopDictPath = Configuration::getInstance()->getConfigMap()["cnStop"];

    loadStopWord(cnStopDictPath);

    buildCnDict();
    storeDict(dictPath.c_str());
    buildIndex();
    storeIndex(dictIndex.c_str());
    cout << "Build Cn Dict and DictIndex OK" << endl;
} 

void DictProducer::loadStopWord(string stopDictPath)
{
    _stopWord.clear();
    ifstream ifs(stopDictPath);
    if (!ifs.good())
    {
        cout << "ifstream open error" << endl;
        return;
    }
    string line, word;
    while (getline(ifs, line))
    {
        istringstream iss(line); //process with each line
        while (iss >> word)
        {
            _stopWord.insert(word);
        }
        // for (auto &elem : _stopWord)
        // {
        //     cout << elem << endl;
        // }
    }
    ifs.close();
}

void DictProducer::buildEnDict()
{
    stringstream ss;
    string line, word, in_word;

    _files.clear();
    showFiles();
    getFiles(_endir);

    // showFiles();
    for (auto &file : _files)
    {
        ifstream ifs(file);
        if (!ifs.good())
        {
            cout << "ifstream open error" << endl;
            return;
        }

        while (getline(ifs, line))
        {
            in_word.clear();
            istringstream iss(line); 

            while (iss >> word)
            {
                // cout << word << endl;
                in_word.clear();
                for (char c : word)
                {
                    
                    if (c >= 'A' && c <= 'Z')
                    {
                        c += 32;
                    }
                 
                    if (isalpha(c))
                    {
                        in_word += c;
                    }
                    else
                        break;
                }

                if (in_word[in_word.size() - 1] == '-')
                    in_word.resize(in_word.size() - 1);

                if (in_word != " " && find(_stopWord.begin(), _stopWord.end(), in_word) == _stopWord.end())
                    ++_dict2[in_word];
                else
                    continue;
            }
        }
        ifs.close();
    }
}

void DictProducer::buildCnDict()
{
    getFiles(_dir);
    // showFiles();
    for (auto &file : _files)
    {
        ifstream ifs(file, std::ios::ate);
        if (!ifs.good())
        {
            cout << "openArtDict file fail" << endl;
            return;
        }

        size_t length = ifs.tellg(); //total article length
        ifs.seekg(std::ios_base::beg);
        char *buff = new char[length + 1]; //read in once
        ifs.read(buff, length + 1);
        string txt(buff);

        delete buff;

        vector<string> tmp = _splitTool->cut(txt);

        for (auto &elem : tmp)
        {
            if (!_stopWord.count(elem) && getByteNum_UTF8(elem[0]) == 3)
            {
                auto exist = _dict2.find(elem);
                if (exist != _dict2.end())
                {
                    ++_dict2[elem];
                }
                else
                {
                    _dict2.insert(std::make_pair(elem, 1));
                }
            }
        }
        ifs.close();
    }
}

void DictProducer::buildIndex()
{
    int i = 0;
    for (auto &elem : _dict2)
    {
        string word = elem.first;
        size_t charNums = word.size() / getByteNum_UTF8(word[0]); 
        for (size_t idx = 0, n = 0; n != charNums; ++idx, ++n)
        {
            size_t charLen = getByteNum_UTF8(word[idx]);
            string subWord = word.substr(idx, charLen);
            _index[subWord].insert(i);
            idx += (charLen - 1);
        }
        ++i;
    }
}

void DictProducer::storeDict(const char *filepath)
{
    ofstream ofs(filepath);
    if (!ofs.good())
    {
        cout << "error in storeDict" << endl;
        return;
    }
    for (auto &it : _dict2)
    {
        ofs << it.first << "  " << it.second << endl;
    }
    ofs.close();
}

void DictProducer::storeIndex(const char *filepath)
{
    ofstream ofs(filepath);
    if (!ofs.good())
    {
        cout << "error in store index" << endl;
        return;
    }
    for (auto &it : _index)
    {
        ofs << it.first << "  ";
        for (auto &set_it : it.second)
        {
            ofs << set_it << " ";
        }
        ofs << endl;
    }
    ofs.close();
}

void DictProducer::showFiles() const
{
    for (auto &elem : _files)
    {
        cout << elem << endl;
    }
}

void DictProducer::showDict() const
{
    for (auto &it : _index)
    {
        cout << it.first << " ";
        for (auto &set_it : it.second)
        {
            cout << set_it << " ";
        }
    }
}

void DictProducer::getFiles(string dir) 
{

    DIR *dirp = opendir(dir.c_str());
    ERROR_CHECK(dirp, nullptr, "opendir");

    struct dirent *pdirent;
    while ((pdirent = readdir(dirp)) != NULL)
    {
        if (strcmp(pdirent->d_name, ".") == 0 || strcmp(pdirent->d_name, "..") == 0)
            continue;

        string filePath;
        filePath = dir + "/" + pdirent->d_name;

        _files.push_back(filePath);
    }
    closedir(dirp);
}

size_t DictProducer::getByteNum_UTF8(const char byte)
{
    int byteNum = 0;
    for (size_t i = 0; i < 6; ++i)
    {
        if (byte & (1 << (7 - i)))
            ++byteNum;
        else
            break;
    }
    return byteNum == 0 ? 1 : byteNum;
}