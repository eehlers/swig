
#include <clo/enumerations/constructors/classes.hpp>

boost::shared_ptr<ComplexLib::TimeZone> ComplexLibAddin::TimeZone_EST() {
    return boost::shared_ptr<ComplexLib::TimeZone>(
        new ComplexLib::TimeZoneEst);
}

boost::shared_ptr<ComplexLib::TimeZone> ComplexLibAddin::TimeZone_UTC() {
    return boost::shared_ptr<ComplexLib::TimeZone>(
        new ComplexLib::TimeZoneUtc);
}

boost::shared_ptr<ComplexLib::TimeZone> ComplexLibAddin::TimeZone_CST() {
    return boost::shared_ptr<ComplexLib::TimeZone>(
        new ComplexLib::TimeZoneCst);
}

