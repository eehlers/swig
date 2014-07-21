
#ifndef obj_hw_adder2_hpp
#define obj_hw_adder2_hpp

#include <string>
#include <oh/libraryobject.hpp>
#include <oh/valueobject.hpp>
#include <boost/shared_ptr.hpp>
#include <Library/simplelib.hpp>

namespace SimpleLibAddin {

    class Long : public ObjectHandler::LibraryObject<SimpleLib::Long> {
    public:
        Long(
            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
            bool permanent,
            long x);
    };

    class Adder2 : public ObjectHandler::LibraryObject<SimpleLib::Adder2> {
    public:
        Adder2(
            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
            bool permanent,
            long x);
    };
}

#endif

