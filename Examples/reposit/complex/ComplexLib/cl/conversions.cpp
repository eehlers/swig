
#include <cl/conversions.hpp>
#include <sstream>

ComplexLib::Grade::Grade(long score)
    : score_(score) {}

ComplexLib::Grade::operator char() {
    if (score_ > 90)
        return 'A';
    else if (score_ > 80)
        return 'B';
    else if (score_ > 70)
        return 'C';
    else if (score_ > 60)
        return 'D';
    else
        return 'F';
}

std::string ComplexLib::showGrade(Grade g) {
    std::ostringstream ret;
    ret << "grade=" << g;
    return ret.str();
}

