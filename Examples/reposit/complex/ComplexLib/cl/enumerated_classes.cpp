
#include <cl/enumerated_classes.hpp>
#include <ctime>
#include <sstream>
#include <iomanip>

std::string ComplexLib::timeString(boost::shared_ptr<TimeZone> timeZone) {
    time_t rawtime;
    time( &rawtime);
    struct tm *ptm = gmtime(&rawtime);
    std::ostringstream ret;
    ret << std::setw(2) << std::setfill('0') << (ptm->tm_hour+timeZone->utcOffset())%24 << ":" << std::setw(2) << std::setfill('0') << ptm->tm_min;
    return ret.str();
}

