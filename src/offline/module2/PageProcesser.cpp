#include "PageProcesser.h"
#include "WebPage.h"
#include "RssParser.h"
#include "Configuration.h"

#include <sys/time.h>
#include <ErrorCheck>
#include <algorithm>
#include <fstream>
using std::ifstream;
using std::sort;

namespace wdcpp
{
PageProcesser::PageProcesser(vector<string> &filePathList, vector<WebPage> &pageList)
    : _filePathList(filePathList),
      _nonRepetivepageList(pageList)
{
    loadStopWords();
}

void PageProcesser::loadStopWords()
{
    ifstream ifs(Configuration::getInstance()->getConfigMap()["stopwords"]);
    if (!ifs)
    {
        ERROR_PRINT("can not open stop_words.utf8");
    }
    string word;
    while (ifs >> word)
        _stopWords.push_back(word);
}


void PageProcesser::process()
{
    {
        struct timeval begTime, endTime;
        gettimeofday(&begTime, NULL);
        loadPageFromXML(); 
        gettimeofday(&endTime, NULL);
        printf("loadPageFromXML take total %ld microsecends\n",
               (endTime.tv_sec - begTime.tv_sec) * 1000000 + (endTime.tv_usec - begTime.tv_usec));
    }

    {
        struct timeval begTime, endTime;
        gettimeofday(&begTime, NULL);
        cutRedundantPage();
        gettimeofday(&endTime, NULL);
        printf("cutRedundantPage take total %ld microsecends\n",
               (endTime.tv_sec - begTime.tv_sec) * 1000000 + (endTime.tv_usec - begTime.tv_usec));

        // Deduplication complete; the _docID and _doc for each article are now determined
        for (size_t idx = 0; idx < _nonRepetivepageList.size(); ++idx)
        {
            _nonRepetivepageList[idx].setPageID(idx);
            _nonRepetivepageList[idx].setPageDoc();
        }
    }

    // for (auto &page : _pageList)
    //     _nonRepetivepageList.push_back(page);

    {
        struct timeval begTime, endTime;
        gettimeofday(&begTime, NULL);
        // time_t now = time(NULL);
        // printf("beg countFrequence at %s", ctime(&now));
        countFrequence(); 
        // now = time(NULL);
        // printf("end countFrequence at %s", ctime(&now));
        gettimeofday(&endTime, NULL);
        printf("countFrequence take total %ld microsecends\n",
               (endTime.tv_sec - begTime.tv_sec) * 1000000 + (endTime.tv_usec - begTime.tv_usec));
    }
}

void PageProcesser::loadPageFromXML()
{
    PageID ID = 0;
    for (auto &filePath : _filePathList)
    {
        RssPraser rssPraser(filePath.c_str());
        for (auto &item : rssPraser.getRssItems())
        {
            WebPage page(item);
            page.setPageID(ID++);
            _pageList.push_back(std::move(page));
           
        }
    }

    // _isDelete.resize(_pageList.size(), false);
}

bool cmp(const WebPage &lhs, const WebPage &rhs)
{
    size_t lhs_size = lhs.getContent().size();
    size_t rhs_size = rhs.getContent().size();
    if (lhs_size != rhs_size)
        return lhs_size > rhs_size;
    else
        return lhs.getDocId() < rhs.getDocId(); 
}
void PageProcesser::cutRedundantPage()
{
    sort(_pageList.begin(), _pageList.end(), cmp);

    for (auto &page : _pageList)
    {
        if (!_comparePages.cut(page)) // If the page does not need to be excluded, save it to _nonRepetivepageList.
            _nonRepetivepageList.push_back(page);
    }

void PageProcesser::countFrequence()
{
    for (auto &page : _nonRepetivepageList)
        page.splitWord(_splitTool, _stopWords);
}

void PageProcesser::printPageList()
{
    using namespace std;
    cout << "_pageList.size = " << _pageList.size() << endl;
    for (auto &page : _pageList)
    {
        cout << "doc = " << page.getDoc() << endl;
        cout << "docId = " << page.getDocId() << endl;
        cout << "docTitle = " << page.getTitle() << endl;
        cout << "docURL = " << page.getUrl() << endl;
        cout << "docContent = " << page.getContent() << endl;
    }
}

void PageProcesser::printStopWords()
{
    using namespace std;
    cout << "PageProcesser::printStopWords()" << endl;
    for (auto &word : _stopWords)
        cout << word << endl;
}
}; // namespace wdcpp
