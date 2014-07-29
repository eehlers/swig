
#ifndef full_lib_adder_hpp
#define full_lib_adder_hpp

#include <string>

namespace FullLib {

    std::string func();

    class Adder {
    private:
        long x_;
    public:
        Adder(long x);
        long add(long y);
    };

};

#endif

