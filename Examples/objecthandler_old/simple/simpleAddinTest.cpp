
#include "simple.hpp"
#include "simpleAddinTest.hpp"
#include <oh/objecthandler.hpp>

namespace SimpleAddin {

class Foo : 
public ObjectHandler::LibraryObject<Simple::Foo> {
public:
Foo(
const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
bool permanent)
: ObjectHandler::LibraryObject<Simple::Foo>(properties, permanent) {
    libraryObject_ = boost::shared_ptr<Simple::Foo>(new Simple::Foo());
}
};

    namespace ValueObjects {

    class rpFoo : public ObjectHandler::ValueObject {
        friend class boost::serialization::access;
    public:
        rpFoo() {}
        rpFoo(
            const std::string& ObjectId,
            bool Permanent);

        const std::set<std::string>& getSystemPropertyNames() const;
        std::vector<std::string> getPropertyNamesVector() const;
        ObjectHandler::property_t getSystemProperty(const std::string&) const;
        void setSystemProperty(const std::string& name, const ObjectHandler::property_t& value);

    protected:
        static const char* mPropertyNames[];
        static std::set<std::string> mSystemPropertyNames;
        bool Permanent_;

        template<class Archive>
        void serialize(Archive& ar, const unsigned int) {
        boost::serialization::void_cast_register<rpFoo, ObjectHandler::ValueObject>(this, this);
            ar  & boost::serialization::make_nvp("ObjectId", objectId_)
                & boost::serialization::make_nvp("Permanent", Permanent_)
                & boost::serialization::make_nvp("UserProperties", userProperties);
        }
    };

    const char* rpFoo::mPropertyNames[] = {
        "Permanent"
    };

    std::set<std::string> rpFoo::mSystemPropertyNames(
        mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));

    const std::set<std::string>& rpFoo::getSystemPropertyNames() const {
        return mSystemPropertyNames;
    }

    std::vector<std::string> rpFoo::getPropertyNamesVector() const {
        std::vector<std::string> ret(
            mPropertyNames, mPropertyNames + sizeof(mPropertyNames) / sizeof(const char*));
        for (std::map<std::string, ObjectHandler::property_t>::const_iterator i = userProperties.begin();
            i != userProperties.end(); ++i)
            ret.push_back(i->first);
        return ret;
    }

    ObjectHandler::property_t rpFoo::getSystemProperty(const std::string& name) const {
        std::string nameUpper = boost::algorithm::to_upper_copy(name);
        if(strcmp(nameUpper.c_str(), "OBJECTID")==0)
            return objectId_;
        else if(strcmp(nameUpper.c_str(), "CLASSNAME")==0)
            return className_;
        else if(strcmp(nameUpper.c_str(), "PERMANENT")==0)
            return Permanent_;
        else
            OH_FAIL("Error: attempt to retrieve non-existent Property: '" + name + "'");
    }

    void rpFoo::setSystemProperty(const std::string& name, const ObjectHandler::property_t& value) {
        std::string nameUpper = boost::algorithm::to_upper_copy(name);
        if(strcmp(nameUpper.c_str(), "OBJECTID")==0)
            objectId_ = boost::get<std::string>(value);
        else if(strcmp(nameUpper.c_str(), "CLASSNAME")==0)
            className_ = boost::get<std::string>(value);
        else if(strcmp(nameUpper.c_str(), "PERMANENT")==0)
            Permanent_ = ObjectHandler::convert2<bool>(value);
        else
            OH_FAIL("Error: attempt to set non-existent Property: '" + name + "'");
    }

    rpFoo::rpFoo(
            const std::string& ObjectId,
            bool Permanent) :
        ObjectHandler::ValueObject(ObjectId, "rpFoo", Permanent),
        Permanent_(Permanent) {
    }
} }

static ObjectHandler::Repository repository;
std::string SimpleAddin::rpFoo(const std::string &objectID) {
        boost::shared_ptr<ObjectHandler::ValueObject> valueObject(
            new SimpleAddin::ValueObjects::rpFoo(
                objectID, false));
        boost::shared_ptr<ObjectHandler::Object> object(
            new SimpleAddin::Foo(
                valueObject, false));
        std::string returnValue =
            ObjectHandler::Repository::instance().storeObject(
                objectID, object, false, valueObject);
        return returnValue;
}

void SimpleAddin::rpFooF(const std::string &objectID) {
    OH_GET_REFERENCE(foo, objectID, SimpleAddin::Foo, Simple::Foo);
    foo->f();
}

