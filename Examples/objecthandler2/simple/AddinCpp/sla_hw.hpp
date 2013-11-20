
#ifndef sla_hpp
#define sla_hpp

#include <string>

namespace SimpleLibAddin {
    void slFunc();
    std::string slAdder(const std::string &objectID, int x);
    int slAdderAdd(const std::string &objectID, int y);
}

#endif

