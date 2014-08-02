
#ifndef addinobjects_enum_factories_hpp
#define addinobjects_enum_factories_hpp

#include <oh/enumerations/typefactory.hpp>
#include <cl/color.hpp>

namespace ObjectHandler {

    typedef boost::shared_ptr<ComplexLib::Color>(*ColorConstructor)();

    template<>
    class Create<boost::shared_ptr<ComplexLib::Color> > :
        private RegistryManager<ComplexLib::Color, EnumClassRegistry> {
    public:
        boost::shared_ptr<ComplexLib::Color> operator() (
                const std::string& colorName) {
            ColorConstructor colorConstructor =
                reinterpret_cast<ColorConstructor>(getType(colorName));
            return colorConstructor();
        }
        using RegistryManager<ComplexLib::Color, EnumClassRegistry>::registerType;
    };
 }

#endif


