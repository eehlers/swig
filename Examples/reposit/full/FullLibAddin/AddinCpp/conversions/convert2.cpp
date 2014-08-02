
// The template instantiations below already appear in FullLibAddin.
// GNU requires us to define them again here in AddinCpp, otherwise the linker fails with undefined symbols.
// MSVC requires us not to define them again here, otherwise the linker fails with multiply defined symbols.

#ifdef __GNUC__

#include "flo/conversions/convert2.hpp"
#include "flo/conversions/cnv_tmpl.hpp"

namespace ObjectHandler {

    template<>
    FullLib::Long convert2<FullLib::Long, property_t>(const property_t& c) {
        return convertLong(c);
    }
}

#endif

