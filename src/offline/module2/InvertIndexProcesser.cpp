#include "InvertIndexProcesser.h"
#include "WebPage.h"
#include "math.h"

#include <ErrorCheck>
#include <iostream>

namespace wdcpp
{
InvertIndexProcesser::InvertIndexProcesser(vector<WebPage> &pageList,
                                           unordered_map<string, unordered_map<PageID, double>> &invertIndexTable)
    : _pageList(pageList),
      _invertIndexTable(invertIndexTable)
{
}

/**
 *  Generate an inverted index
 */
void InvertIndexProcesser::process()
{
    using namespace std;
    // cout << "beg InvertIndexProcesser::process()" << endl;
    // cout << "_pageList.size() = " << _pageList.size() << endl;

    _sumOfWeightsPerPage.resize(_pageList.size(), 0.0); 

    for (auto &page : _pageList) // WebPage page
    {
        auto &wordsMap = page.getWordsMap(); // unordered_map<string, int> wordsMap
        for (auto &wordPair : wordsMap)      // pair<string, int> wordPair
        {
            string word = wordPair.first;

            int wordNumInPage = wordsMap.size(); 
            if (wordNumInPage < 1)
            {
                ERROR_PRINT("this page contains no word\n");
                exit(EXIT_FAILURE);
            }
            double TF = (double)wordPair.second / wordNumInPage;

            _invertIndexTable[word].insert({page.getDocId(), TF}); // value = TF
        }
    }

    // cout << "_invertIndexTable.size() = " << _invertIndexTable.size() << endl;

    // Iterate through all words
    for (auto &invertIndexPair : _invertIndexTable) // pair<string, unordered_map<int, double>> invertIndexPair
    {
        // Iterate through all article pageIds containing the word
        auto &pageIdMap = invertIndexPair.second; // unordered_map<int, double> pageIdMap
        for (auto &pageIdPair : pageIdMap)        // pair<int, double> pageIdPair
        {
            int pageId = pageIdPair.first;
            double TF = pageIdPair.second;
            int DF = pageIdMap.size(); // 上限 -> N
            int N = _pageList.size();
            double IDF = 0.0;
            if (N != DF)
                IDF = (double)log10((double)N / (DF + 1));
            double w = TF * IDF;

            pageIdPair.second = w; // value = w
            // cout << "_sumOfWeightsPerPage.size() = " << _sumOfWeightsPerPage.size() << endl;
            // cout << "pageId = " << pageId << endl;
            _sumOfWeightsPerPage[pageId] += w * w;
        }
    }

    for (auto &invertIndexPair : _invertIndexTable) // pair<string, set<pair<int, double>>> invertIndexPair
    {
        auto &pageIdMap = invertIndexPair.second; // map<int, double> pageIdMap
        for (auto &pageIdPair : pageIdMap)        // pair<int, double> pageIdPair
        {
            int pageId = pageIdPair.first;
            double sumWeight = _sumOfWeightsPerPage[pageId];
            if (sumWeight == 0.0)
            {
                ERROR_PRINT("this page's sumWeight equal 0.0\n");
                exit(EXIT_FAILURE);
            }
            pageIdPair.second /= sqrt(sumWeight); // value = w'
        }
    }

    // cout << "end InvertIndexProcesser::process()" << endl;

    // printInvertIndexTable();
}

void InvertIndexProcesser::printInvertIndexTable()
{
    using namespace std;
    cout << "_invertIndexTable.size() = " << _invertIndexTable.size() << endl;
    for (auto &wordPair : _invertIndexTable)
    {
        cout << wordPair.first << " ";
        for (auto &pagePair : wordPair.second)
        {
            cout << "<" << pagePair.first << ", " << pagePair.second << "> ";
        }
        cout << endl;
    }
}
}; // namespace wdcpp
