
#ifndef obj_hpp
#define obj_hpp

#include <string>
#include <oh/libraryobject.hpp>
#include <oh/valueobject.hpp>
#include <boost/shared_ptr.hpp>
#include <Library/simplelib.hpp>

namespace SimpleLibAddin {

    std::string func();

    class Adder : public ObjectHandler::LibraryObject<SimpleLib::Adder> {
    public:
        Adder(
            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
            bool permanent,
            long x);
    };
}

#endif

