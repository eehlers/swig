
#ifndef complex_lib_adder_hpp
#define complex_lib_adder_hpp

#include <string>

namespace ComplexLib {

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

