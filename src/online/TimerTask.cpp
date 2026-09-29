#include "TimerTask.h"
#include "CacheManager.h"

namespace wdcpp
{
void TimerTask::process()
{
    CacheManager::getInstance()->sync(); 
}
}; // namespace wdcpp
