#include "WebPage.h"
#include "RssParser.h"
#include "SplitTool.h"

#include <sstream>
using std::stringstream;

namespace wdcpp
{

WebPage::WebPage(const RssItem &item)
    : _docID(0)
{
    _docTitle = item.title;
    _docURL = item.link;
    _docContent = item.description;

    // stringstream ss(str);
    // getline(ss, _docTitle);
    // getline(ss, _docURL);
    // getline(ss, _docContent);
}

WebPage::WebPage(const string &doc)
    : _doc(doc)
{
    size_t beg = 0, end = 0;

    beg = doc.find("<docid>"); 
    end = doc.find("</docid>");
    istringstream tmp(doc.substr(beg + 8, end - beg - 9));
    tmp >> _docID;

    beg = doc.find("<title>");
    end = doc.find("</title>");
    _docTitle = doc.substr(beg + 8, end - beg - 9);

    beg = doc.find("<url>");
    end = doc.find("</url>");
    _docURL = doc.substr(beg + 6, end - beg - 7);

    beg = doc.find("<content>");
    end = doc.find("</content>");
    _docContent = doc.substr(beg + 10, end - beg - 11);

}

WebPage::WebPage()
    : _docID(0)
{
}

string WebPage::getDoc() const
{
    return _doc;
}

PageID WebPage::getDocId() const
{
    return _docID;
}

string WebPage::getTitle() const
{
    return _docTitle;
}

string WebPage::getUrl() const
{
    return _docURL;
}

string WebPage::getContent() const
{
    return _docContent;
}

string WebPage::getSummary() const
{
    return _docSummary;
}

unordered_map<string, int> &WebPage::getWordsMap()
{
    return _wordsMap;
}

void WebPage::setPageID(PageID ID)
{
    _docID = ID;
}

void WebPage::setPageDoc()
{
    _doc = "<doc>\n\t<docid> " + to_string(_docID) +
           " </docid>\n\t<title> " + _docTitle +
           " </title>\n\t<url> " + _docURL +
           " </url>\n\t<content> " + _docContent +
           " </content>\n</doc>\n";
}

void WebPage::setPageContent(const string &content)
{
    _docContent = content;
}

void WebPage::setPageSummary(const string &summary)
{
    _docSummary = summary;
}

void WebPage::splitWord(SplitTool &tool, const vector<string> &stopWords)
{
    auto words = tool.cut(_docTitle + _docContent); 
    for (auto &word : words)                      
    {
        if (word != " " && find(stopWords.begin(), stopWords.end(), word) == stopWords.end()) 
            ++_wordsMap[word];
    }

    // printWordsMap();
}

void WebPage::printWordsMap() const
{
    using namespace std;
    cout << "words num of No." << _docID << " = " << _wordsMap.size() << endl;
    for (auto &pair : _wordsMap)
    {
        cout << pair.first << " " << pair.second << endl;
    }
}
}; // namespace wdcpp
