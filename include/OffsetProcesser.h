#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <utility>
using std::map;
using std::pair;
using std::vector;

namespace wdcpp
{
class WebPage;

class OffsetProcesser
{
public:
    explicit OffsetProcesser(vector<WebPage> &, vector<pair<size_t, size_t>> &); // parameters passed into a web library
    ~OffsetProcesser()
    {
        using namespace std;
        cout << "~OffsetProcesser()" << endl;
    }

    void process(); // generate offest lirary 

private:
    vector<WebPage> &_pagelist;
    vector<pair<size_t, size_t>> &_offsetlib;
};
};
