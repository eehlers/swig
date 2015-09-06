
#ifndef obj_overrides_hpp
#define obj_overrides_hpp

#include <string>
#include <rp/libraryobject.hpp>
#include <rp/valueobject.hpp>
#include <boost/shared_ptr.hpp>
#include <cl/overrides.hpp>

using namespace ComplexLib;

namespace ComplexLibAddin {

    class Test2 : 
        public reposit::LibraryObject<ComplexLib::Test2> {
    public:
        Test2(
            const boost::shared_ptr<reposit::ValueObject>& properties,
            // BEGIN typemap rp_tm_default
            // END   typemap rp_tm_default
            bool permanent)
        : reposit::LibraryObject<ComplexLib::Test2>(properties, permanent) {
            libraryObject_ = boost::shared_ptr<ComplexLib::Test2>(new ComplexLib::Test2(
                // BEGIN typemap rp_tm_default
                // END   typemap rp_tm_default
            ));
        }
        std::string f();
    };

} // namespace ComplexLibAddin

#endif

