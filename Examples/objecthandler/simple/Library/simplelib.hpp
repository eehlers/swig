
#ifndef simplelib_hpp
#define simplelib_hpp

namespace SimpleLib {

    void func();

    class Adder {
    private:
        int x_;
    public:
        Adder(int x);
        int add(int y);
    };

};

#endif

