#pragma once
#include <string>

namespace wdcpp
{
class SocketIO
{
public:
    explicit SocketIO(int);

    size_t readn(void *, size_t);
    size_t writen(const void *, size_t);
    size_t readline(char *, size_t);

private:
    int _fd;
};
};
