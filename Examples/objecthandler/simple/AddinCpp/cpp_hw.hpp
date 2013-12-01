
#ifndef cpp_hw_hpp
#define cpp_hw_hpp

#include <string>

namespace SimpleLibAddin {
    std::string slFunc();
    std::string slAdder(const std::string &objectID, long x);
    long slAdderAdd(const std::string &objectID, long y);
}

#endif

