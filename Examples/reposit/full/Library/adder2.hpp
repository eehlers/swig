
#ifndef simplelib_adder2_hpp
#define simplelib_adder2_hpp

#include <string>

namespace SimpleLib {

    class Long {
    private:
        long x_;
    public:
        Long(long x=0);
        Long(std::string x);
        long x();
    };

    class Adder2 {
    private:
        long x_;
    public:
        Adder2(SimpleLib::Long x);
        long add(SimpleLib::Long y);
    };

};

#endif

