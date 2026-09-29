#pragma once
#include "NonCopyable.h"

namespace wdcpp
{
class Socket
    : NonCopyable
{
public:
    Socket();
    explicit Socket(int);
    ~Socket();

    int fd() const;
    void shutDownWrite();
    void setNonBlock();

private:
    int _fd;
};
};
