
#include <cl/typedefs.hpp>
#include <sstream>

std::string ComplexLib::doubleToString(const Double &d) {
    std::ostringstream ret;
    ret << "d=" << d;
    return ret.str();
}

std::string ComplexLib::longDoubleToString(const LongDouble &d) {
    std::ostringstream ret;
    ret << "d=" << d;
    return ret.str();
}

