
#ifndef addinobjects_enum_factories_hpp
#define addinobjects_enum_factories_hpp

#include <oh/enumerations/typefactory.hpp>
#include <Library/color.hpp>

namespace ObjectHandler {

    typedef boost::shared_ptr<FullLib::Color>(*ColorConstructor)();

    template<>
    class Create<boost::shared_ptr<FullLib::Color> > :
        private RegistryManager<FullLib::Color, EnumClassRegistry> {
    public:
        boost::shared_ptr<FullLib::Color> operator() (
                const std::string& colorName) {
            ColorConstructor colorConstructor =
                reinterpret_cast<ColorConstructor>(getType(colorName));
            return colorConstructor();
        }
        using RegistryManager<FullLib::Color, EnumClassRegistry>::registerType;
    };
 }

#endif


