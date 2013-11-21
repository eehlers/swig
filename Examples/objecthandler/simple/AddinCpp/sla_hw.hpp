
#ifndef sla_hpp
#define sla_hpp

#include <string>

namespace SimpleLibAddin {
    std::string slFunc();
    std::string slAdder(const std::string &objectID, int x);
    int slAdderAdd(const std::string &objectID, int y);
}

#endif

