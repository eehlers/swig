
#ifndef vo_hw_hpp
#define vo_hw_hpp

#include <string>
#include <set>
#include <oh/valueobject.hpp>

namespace ValueObjects {

    class slAdder : public ObjectHandler::ValueObject {
        friend class boost::serialization::access;
    public:
        slAdder() {}
        slAdder(
            const std::string& ObjectId,
            bool Permanent,
            int x);

        const std::set<std::string>& getSystemPropertyNames() const;
        std::vector<std::string> getPropertyNamesVector() const;
        ObjectHandler::property_t getSystemProperty(const std::string&) const;
        void setSystemProperty(const std::string& name, const ObjectHandler::property_t& value);

    protected:
        static const char* mPropertyNames[];
        static std::set<std::string> mSystemPropertyNames;
        bool Permanent_;
        int x_;

        template<class Archive>
        void serialize(Archive& ar, const unsigned int) {
        boost::serialization::void_cast_register<slAdder, ObjectHandler::ValueObject>(this, this);
            ar  & boost::serialization::make_nvp("ObjectId", objectId_)
                & boost::serialization::make_nvp("Permanent", Permanent_)
                & boost::serialization::make_nvp("UserProperties", userProperties);
        }
    };
}

#endif

