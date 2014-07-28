
#ifndef simplelib_adder2_hpp
#define simplelib_adder2_hpp

#include <string>

namespace FullLib {

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
        Adder2(FullLib::Long x);
        long add(FullLib::Long y);
    };

};

#endif

