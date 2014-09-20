
#include <cl/coercions.hpp>
#include <sstream>

ComplexLib::Grade2::Grade2(double score)
    : score_(score) {}

ComplexLib::Grade2::operator char() {
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

std::string ComplexLib::showGrade2(Grade2 g) {
    std::ostringstream ret;
    ret << "grade=" << g;
    return ret.str();
}

