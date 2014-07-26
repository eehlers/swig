
#ifndef addinobjects_enum_factories_hpp
#define addinobjects_enum_factories_hpp

#include <oh/enumerations/typefactory.hpp>
#include <Library/color.hpp>

namespace ObjectHandler {

    typedef boost::shared_ptr<SimpleLib::Color>(*ColorConstructor)();

    template<>
    class Create<boost::shared_ptr<SimpleLib::Color> > :
        private RegistryManager<SimpleLib::Color, EnumClassRegistry> {
    public:
        boost::shared_ptr<SimpleLib::Color> operator() (
                const std::string& colorName) {
            ColorConstructor colorConstructor =
                reinterpret_cast<ColorConstructor>(getType(colorName));
            return colorConstructor();
        }
        using RegistryManager<SimpleLib::Color, EnumClassRegistry>::registerType;
    };
 }

#endif


