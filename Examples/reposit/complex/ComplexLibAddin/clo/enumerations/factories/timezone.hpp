
#ifndef factories_timezone_hpp
#define factories_timezone_hpp

#include <oh/enumerations/typefactory.hpp>
#include <cl/enumerated_classes.hpp>

namespace ObjectHandler {

    typedef boost::shared_ptr<ComplexLib::TimeZone>(*TimeZoneConstructor)();

    template<>
    class Create<boost::shared_ptr<ComplexLib::TimeZone> > :
        private RegistryManager<ComplexLib::TimeZone, EnumClassRegistry> {
    public:
        boost::shared_ptr<ComplexLib::TimeZone> operator() (
                const std::string& timeZoneName) {
            TimeZoneConstructor timeZoneConstructor =
                reinterpret_cast<TimeZoneConstructor>(getType(timeZoneName));
            return timeZoneConstructor();
        }
        using RegistryManager<ComplexLib::TimeZone, EnumClassRegistry>::registerType;
    };
 }

#endif


