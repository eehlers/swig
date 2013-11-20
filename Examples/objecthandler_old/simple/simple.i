
%module SimpleAddin
%{
//wazoo
#include "simple.hpp"
%}
namespace Simple {
void f2();
    class Foo {
        public:
            void f();
            Foo();
    };
}
