
#ifndef addinobjects_enum_ctor_classes_hpp
#define addinobjects_enum_ctor_classes_hpp

#include <boost/shared_ptr.hpp>
#include <cl/enumerated_classes.hpp>

namespace ComplexLibAddin {

    boost::shared_ptr<ComplexLib::TimeZone> TimeZone_EST();
    boost::shared_ptr<ComplexLib::TimeZone> TimeZone_UTC();
    boost::shared_ptr<ComplexLib::TimeZone> TimeZone_CST();

}

#endif

