
#ifndef complex_lib_enumerated_classes_hpp
#define complex_lib_enumerated_classes_hpp

// Test enumerated classes.

#include <string>
#include <boost/shared_ptr.hpp>

namespace ComplexLib {

    class TimeZone {
    private:
        int utcOffset_;
    public:
        virtual int utcOffset() const { return utcOffset_; }
        TimeZone(int utcOffset) : utcOffset_(utcOffset) {}
        virtual ~TimeZone() {}
    };

    class TimeZoneEst : public TimeZone {
    public:
        TimeZoneEst() : TimeZone(-5) {}
    };

    class TimeZoneUtc : public TimeZone {
    public:
        TimeZoneUtc() : TimeZone(0) {}
    };

    class TimeZoneCst : public TimeZone {
    public:
        TimeZoneCst() : TimeZone(8) {}
    };

    std::string timeString(boost::shared_ptr<TimeZone> timeZone);
};

#endif

