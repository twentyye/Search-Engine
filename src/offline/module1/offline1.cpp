#include "SplitTool.h"
#include "DictProducer.h"
#include "Configuration.h"

using namespace std;
using namespace wdcpp;
void test0()
{

    string cnYuliaoPath = Configuration::getInstance()->getConfigMap()["cnDir"];
    string enYuliaoPath = Configuration::getInstance()->getConfigMap()["enDir"];

    // string cnYuliaoPath = Configuration::getInstance()->getConfigMap()["cnTest"];
    // string enYuliaoPath = Configuration::getInstance()->getConfigMap()["enTest"];

    SplitTool tool;
    DictProducer endict1(cnYuliaoPath, &tool);
    DictProducer endict2(enYuliaoPath);
}

int main()
{

    test0();
    return 0;
}