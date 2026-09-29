#include "WebPageSearcher.h"
#include "Configuration.h"
#include "MyLog.h"
#include "MultiBytesCharacter.h"
#include "nlohmann/json.hpp"
#include "fifo_map.hpp"

using namespace nlohmann;

template <class K, class V, class dummy_compare, class A>
using my_workaround_fifo_map = fifo_map<K, V, fifo_map_compare<K>, A>;

using my_json = basic_json<my_workaround_fifo_map>;
using Json = my_json;

#include <ErrorCheck>
#include <set>
#include <math.h>

using std::multiset;

namespace wdcpp
{

WebPageSearcher::WebPageSearcher()
{
    loadFromFile();
}

void WebPageSearcher::loadFromFile()
{
    ifstream stopWordsLib(Configuration::getInstance()->getConfigMap()["stopwords"]);
    if (!stopWordsLib)
    {
        ERROR_PRINT("can not open stop_words.utf8");
        exit(EXIT_FAILURE);
    }

    string word;
    while (getline(stopWordsLib, word))
    {
        _stopWords.push_back(word);
    }

    ifstream offsetLib(Configuration::getInstance()->getConfigMap()["offset"]);
    if (!offsetLib)
    {
        ERROR_PRINT("can not open offset.dat");
        exit(EXIT_FAILURE);
    }

    ifstream ripepageLib(Configuration::getInstance()->getConfigMap()["ripepage"]);
    if (!ripepageLib)
    {
        ERROR_PRINT("can not open ripepage.dat");
        exit(EXIT_FAILURE);
    }

    string offsetLine;
    int docid;
    size_t beg, offset;
    char buf[65535] = {0};

    while (getline(offsetLib, offsetLine))
    {
        stringstream ss(offsetLine);
        ss >> docid >> beg >> offset;

        ripepageLib.read(buf, offset);
        string doc(buf);
        WebPage tmp(doc);
        _pageList.push_back(std::move(tmp));
    }

    ifstream invertIndexLib(Configuration::getInstance()->getConfigMap()["invertIndex"]);
    if (!invertIndexLib)
    {
        ERROR_PRINT("can not open invertIndex.dat");
        exit(EXIT_FAILURE);
    }

    string invertIndexLine;
    string keyWord;
    double weight;

    while (getline(invertIndexLib, invertIndexLine))
    {
        stringstream ss(invertIndexLine);

        ss >> keyWord;
        while (ss)
        {
            ss >> docid >> weight;
            _invertIndexTable[keyWord][docid] = weight;
        }
    }

    stopWordsLib.close();
    offsetLib.close();
    ripepageLib.close();
    invertIndexLib.close();
}

string WebPageSearcher::doQuery(string msg)
{
    using namespace std;
    cout << "doQuery: " << msg << endl;

    WebPage pageX;
    pageX.setPageContent(msg);
    pageX.splitWord(_splitTool, _stopWords);

    unordered_map<string, double> vecX = getVectorX(pageX);

    string response;
    set<PageID> IDs = getIDs(pageX);

    if (IDs.empty())
    {
        LogInfo("webPageSearcher miss: %s", msg.c_str());
        response = serializeForNoting();
    }
    else
    {
        vector<PageID> sortedIDs = getSortedIDs(vecX, IDs);

        setSummarys(sortedIDs, pageX);

        response = serialize(sortedIDs);
    }

    return response;
}

unordered_map<string, double> WebPageSearcher::getVectorX(WebPage &pageX)
{
    unordered_map<string, double> vecX;
    unordered_map<string, int> &wordsMapX = pageX.getWordsMap();

    double sumWeight = 0.0;

    for (auto &wordPair : wordsMapX)
    {
        double TF = (double)wordPair.second / wordsMapX.size();
        int DF = _invertIndexTable[wordPair.first].size() + 1;
        int N = _pageList.size() + 1;

        double IDF = 0.0;
        if (N != DF)
            IDF = log10((double)N / (DF + 1));

        double w = TF * IDF;
        sumWeight += w * w;

        vecX.insert({wordPair.first, w});
    }

    for (auto &wordPair : vecX)
    {
        if (sumWeight == 0)
            wordPair.second = 0;

        wordPair.second /= sqrt(sumWeight);
    }

    return vecX;
}

/**
 * Get the candidate document IDs that contain all query terms.
 */
set<PageID> WebPageSearcher::getIDs(WebPage &pageX)
{
    unordered_map<string, int> &wordsMapX = pageX.getWordsMap();

    vector<set<PageID>> IDsArr;

    for (auto &wordPair : wordsMapX)
    {
        set<PageID> IDs;

        for (auto &invertInvertPair : _invertIndexTable[wordPair.first])
        {
            IDs.insert(invertInvertPair.first);
        }

        IDsArr.push_back(std::move(IDs));
    }

    set<PageID> IDs;

    for (auto &item : IDsArr)
    {
        if (IDs.empty())
            IDs = item;
        else
        {
            set<PageID> tmp;
            set_intersection(
                item.begin(),
                item.end(),
                IDs.begin(),
                IDs.end(),
                inserter(tmp, tmp.begin()));

            swap(IDs, tmp);
        }
    }

    return IDs;
}

struct MyGreater
{
    bool operator()(
        const pair<double, PageID> &lhs,
        const pair<double, PageID> &rhs) const
    {
        if (lhs.first != rhs.first)
            return lhs.first > rhs.first;
        else
            return lhs.second < rhs.second;
    }
}

vector<PageID> WebPageSearcher::getSortedIDs(
    const unordered_map<string, double> &vecX,
    const set<PageID> &IDs)
{
    multiset<pair<double, PageID>, MyGreater> sortCos;

    for (auto &id : IDs)
    {
        unordered_map<string, double> vecY;

        for (auto it = vecX.begin(); it != vecX.end(); ++it)
        {
            double w = _invertIndexTable[it->first][id];
            vecY.insert(std::make_pair(it->first, w));
        }

        double innerProduct = 0;
        double lengthXAbs = 0;
        double lengthYAbs = 0;

        for (auto it = vecX.begin(); it != vecX.end(); ++it)
        {
            innerProduct += it->second * vecY[it->first];
            lengthXAbs += it->second * it->second;
            lengthYAbs += vecY[it->first] * vecY[it->first];
        }

        double Cos =
            innerProduct / (sqrt(lengthXAbs) * sqrt(lengthYAbs));

        sortCos.insert(std::make_pair(Cos, id));
    }

    vector<PageID> result;

    for (auto &id : sortCos)
    {
        result.push_back(id.second);
    }

    return result;
}

/**
 * Generate summary snippets for the candidate documents.
 */
void WebPageSearcher::setSummarys(
    vector<PageID> &sortedIDs,
    WebPage &pageX)
{
    const size_t STEP = 40;

    unordered_map<string, int> &wordsMapX = pageX.getWordsMap();
    vector<PageID> tmpIDs = sortedIDs;

    for (auto &ID : tmpIDs)
    {
        static int n = 0;
        cout << ++n << endl;

        if (n == 1548)
        {
            cout << "got it!" << endl;
        }

        const string content = _pageList[ID].getContent();

        size_t first_pos = SIZE_MAX;

        for (auto &wordPair : wordsMapX)
        {
            string word = wordPair.first;
            size_t pos = content.find(word);

            if (pos != content.npos && pos < first_pos)
                first_pos = pos;
        }

        if (first_pos == SIZE_MAX)
        {
            remove(sortedIDs.begin(), sortedIDs.end(), ID);
            continue;
        }

        size_t first_to_end = content.substr(first_pos).size();

        size_t right_pos =
            first_pos +
            howManyBytesWithNCharacter(
                &content[first_pos],
                first_to_end,
                STEP);

        vector<size_t> ppc =
            getPosPerCharactor(content, first_pos);

        size_t left_pos =
            ppc.size() >= STEP
                ? ppc[ppc.size() - STEP]
                : 0;

        string summary = "";

        if (left_pos != 0)
            summary += " ... ji";

        summary += content.substr(left_pos, right_pos - left_pos);

        if (right_pos < content.size())
            summary += " ... ";

        _pageList[ID].setPageSummary(summary);
    }
}

/**
 * Serialize the response for a failed search.
 */
string WebPageSearcher::serializeForNoting()
{
    Json root;

    root["msgID"] = 404;
    root["msg"] = json('No relevant articles found.');

    return root.dump(4);
}

/**
 * Serialize the search results into JSON.
 */
string WebPageSearcher::serialize(const vector<PageID> &sortedIDs)
{
    const PageID N =
        stol(Configuration::getInstance()->getConfigMap()["maxpagenum"]);

    Json root;
    root["msgID"] = 200;

    Json msg;
    PageID cnt = 0;

    for (auto &ID : sortedIDs)
    {
        if (++cnt > N)
            break;

        WebPage &page = _pageList[ID];

        Json file;
        file["title"] = page.getTitle();
        file["url"] = page.getUrl();
        file["summary"] = page.getSummary();

        msg.push_back(file);
    }

    root["msg"] = msg;

    return root.dump(4);
}

}; // namespace wdcpp
