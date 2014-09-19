
#ifndef objmanual_overrides_hpp
#define objmanual_overrides_hpp

#include <string>
#include <oh/libraryobject.hpp>
#include <oh/valueobject.hpp>
#include <boost/shared_ptr.hpp>
#include <cl/overrides.hpp>

using namespace ComplexLib;

namespace ComplexLibAddin {

    class Test2 : 
        public ObjectHandler::LibraryObject<ComplexLib::Test2> {
    public:
        Test2(
            const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
            // BEGIN typemap rp_tm_default
            // END   typemap rp_tm_default
            bool permanent)
        : ObjectHandler::LibraryObject<ComplexLib::Test2>(properties, permanent) {
            libraryObject_ = boost::shared_ptr<ComplexLib::Test2>(new ComplexLib::Test2(
                // BEGIN typemap rp_tm_default
                // END   typemap rp_tm_default
            ));
        }
        std::string f();
    };

} // namespace ComplexLibAddin

#endif

