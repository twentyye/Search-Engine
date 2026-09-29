#pragma once
#include "tinyxml2.h"
using namespace tinyxml2;

#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <fstream>
using std::cin;
using std::cout;
using std::endl;
using std::ofstream;
using std::regex;
using std::string;
using std::vector;

namespace wdcpp
{
struct RssItem // store the content of an item
{
    string title;
    string link;
    string description;
    string content;

    bool check()
    {
        if (title == "" || link == "" || description == "")
            return true;
        if (title == description) 
            return true;
        if (description.size() <= title.size()) 
            return true;
        for (auto &s : description)
        {
            if (s != ' ' || s != '\n' || s != '\t')
                return false; // if description have non-special characters,store this item 
        }
        return true;
    }
};

class RssPraser 
{
public:
    explicit RssPraser(const char *);

    void dump(const string &); // save to specified file
    vector<RssItem> &getRssItems();

private:
    void prase(const char *); 
    string dissolve(string);  

private:
    vector<RssItem> _rss; // store all file of parsing (including  the home page)
};
}; // namespace wdcpp
