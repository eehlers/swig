
#ifndef vo_hw_adder2_hpp
#define vo_hw_adder2_hpp

#include <string>
#include <set>
#include <oh/valueobject.hpp>

namespace ValueObjects {

    class slLong : public ObjectHandler::ValueObject {
        friend class boost::serialization::access;
    public:
        slLong() {}
        slLong(
            const std::string& ObjectId,
            bool Permanent,
            long x);

        const std::set<std::string>& getSystemPropertyNames() const;
        std::vector<std::string> getPropertyNamesVector() const;
        ObjectHandler::property_t getSystemProperty(const std::string&) const;
        void setSystemProperty(const std::string& name, const ObjectHandler::property_t& value);

    protected:
        static const char* mPropertyNames[];
        static std::set<std::string> mSystemPropertyNames;
        bool Permanent_;
        long x_;

        template<class Archive>
        void serialize(Archive& ar, const unsigned long) {
        boost::serialization::void_cast_register<slLong, ObjectHandler::ValueObject>(this, this);
            ar  & boost::serialization::make_nvp("ObjectId", objectId_)
                & boost::serialization::make_nvp("Permanent", Permanent_)
                & boost::serialization::make_nvp("UserProperties", userProperties);
        }
    };

    class slAdder2 : public ObjectHandler::ValueObject {
        friend class boost::serialization::access;
    public:
        slAdder2() {}
        slAdder2(
            const std::string& ObjectId,
            bool Permanent,
            long x);

        const std::set<std::string>& getSystemPropertyNames() const;
        std::vector<std::string> getPropertyNamesVector() const;
        ObjectHandler::property_t getSystemProperty(const std::string&) const;
        void setSystemProperty(const std::string& name, const ObjectHandler::property_t& value);

    protected:
        static const char* mPropertyNames[];
        static std::set<std::string> mSystemPropertyNames;
        bool Permanent_;
        long x_;

        template<class Archive>
        void serialize(Archive& ar, const unsigned long) {
        boost::serialization::void_cast_register<slAdder2, ObjectHandler::ValueObject>(this, this);
            ar  & boost::serialization::make_nvp("ObjectId", objectId_)
                & boost::serialization::make_nvp("Permanent", Permanent_)
                & boost::serialization::make_nvp("UserProperties", userProperties);
        }
    };
}

#endif

