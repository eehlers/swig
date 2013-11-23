
%module SimpleLibAddin
%{
#include <Library/simplelib.hpp>
%}
namespace SimpleLib {
    std::string func();
    class Adder {
    public:
        Adder(int x);
        int add(int y);
    };
}
