
#ifndef simplelib_hpp
#define simplelib_hpp

#include <string>

namespace SimpleLib {

    std::string func();

    class Adder {
    private:
        int x_;
    public:
        Adder(int x);
        int add(int y);
    };

};

#endif

