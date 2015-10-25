
// BEGIN buffer b_lib_grp_hpp


#ifndef obj_diamond_inheritance_hpp
#define obj_diamond_inheritance_hpp

#include <string>
#include <rp/object.hpp>
#include <rp/valueobject.hpp>
#include <boost/shared_ptr.hpp>
#include <cl/diamond_inheritance.hpp>

// FIXME get rid of this:
using namespace ComplexLib;

namespace ComplexLibAddin {


    class Foo2 : 
        virtual public reposit::Object {
    public:
        boost::shared_ptr<ComplexLib::Foo2> foo2_;
        Foo2(
            const boost::shared_ptr<reposit::ValueObject>& properties,
            // BEGIN typemap rp_tm_default
            // END   typemap rp_tm_default
            bool permanent)
        : reposit::Object(properties, permanent) {
            foo2_ = boost::shared_ptr<ComplexLib::Foo2>(new ComplexLib::Foo2(
                // BEGIN typemap rp_tm_default
                // END   typemap rp_tm_default
            ));
        }
        Foo2() {}
        void getLibraryObject(boost::shared_ptr<ComplexLib::Foo2> &ret) const {
            ret = foo2_;
        }
    };


    class Bar2 : 
        virtual public reposit::Object {
    public:
        boost::shared_ptr<ComplexLib::Bar2> bar2_;
        Bar2(
            const boost::shared_ptr<reposit::ValueObject>& properties,
            // BEGIN typemap rp_tm_default
            // END   typemap rp_tm_default
            bool permanent)
        : reposit::Object(properties, permanent) {
            bar2_ = boost::shared_ptr<ComplexLib::Bar2>(new ComplexLib::Bar2(
                // BEGIN typemap rp_tm_default
                // END   typemap rp_tm_default
            ));
        }
        Bar2() {}
        void getLibraryObject(boost::shared_ptr<ComplexLib::Bar2> &ret) const {
            ret = bar2_;
        }
    };


    class FooBar2 : 
        public Foo2, public Bar2 {
        boost::shared_ptr<ComplexLib::FooBar2> foobar2_;
    public:
        FooBar2(
            const boost::shared_ptr<reposit::ValueObject>& properties,
            // BEGIN typemap rp_tm_default
            // END   typemap rp_tm_default
            bool permanent)
        : reposit::Object(properties, permanent) {
            foobar2_ = boost::shared_ptr<ComplexLib::FooBar2>(new ComplexLib::FooBar2(
                // BEGIN typemap rp_tm_default
                // END   typemap rp_tm_default
            ));
            foo2_ = foobar2_;
            bar2_ = foobar2_;
        }
        void getLibraryObject(boost::shared_ptr<ComplexLib::FooBar2> &ret) const {
            ret = foobar2_;
        }
    };

} // namespace ComplexLibAddin

#endif


// END buffer b_lib_grp_hpp

