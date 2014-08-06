
%feature("rp:group", "inheritance");
%feature("rp:include") %{
#include <cl/inheritance.hpp>
%}

namespace ComplexLib {

    // A

    class A {
    public:
        A();
        virtual std::string func();
    };

    // B

    class BA {
    public:
        virtual std::string func();
    };

    class BB : public BA {
    public:
        BB();
        virtual std::string func();
    };

    // C

    class CA {
    public:
        virtual std::string func();
    };

    class CB : public CA {
    public:
        virtual std::string func();
    };

    class CC : public CB {
    public:
        CC();
        virtual std::string func();
    };
}

