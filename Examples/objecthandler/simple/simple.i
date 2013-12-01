
%module SimpleLibAddin
%{
#include <Library/simplelib.hpp>
%}
%feature("oh:group", "adder");
namespace SimpleLib {
    std::string func();
    class Adder {
    public:
        Adder(long x);
        long add(long y);
    };
}
