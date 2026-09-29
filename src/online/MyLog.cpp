#include "MyLog.h"

namespace wdcpp
{
MyLog *MyLog::_pInstance = MyLog::getInstance();

MyLog::MyLog()
    : _root(Category::getRoot().getInstance("wd"))
{
    auto ptnLayout = new PatternLayout();
    ptnLayout->setConversionPattern("%d %c [%p] %m%n");

    auto pFileapp = new FileAppender("fileapp", "../log/wd.log");
    pFileapp->setLayout(ptnLayout);

    _root.setPriority(Priority::DEBUG);
    _root.addAppender(pFileapp);
}

MyLog *MyLog::getInstance()
{
    if (nullptr == _pInstance)
    {
        _pInstance = new MyLog();
        ::atexit(destroy);
    }
    return _pInstance;
}

void MyLog::destroy()
{
    if (_pInstance)
    {
        delete _pInstance;
        _pInstance = nullptr;
    }
}

MyLog::~MyLog()
{
    Category::shutdown();
}
};
