
#include <ohxl/objecthandlerxl.hpp>
#include <ohxl/register/register_all.hpp>
#include <ohxl/functions/export.hpp>
#include <ohxl/utilities/xlutilities.hpp>
#include <ohxl/objectwrapperxl.hpp>
#include "ValueObjects/vo_adder.hpp"
#include "AddinObjects/obj_adder.hpp"

/* Use BOOST_MSVC instead of _MSC_VER since some other vendors (Metrowerks,
   for example) also #define _MSC_VER
*/
#ifdef BOOST_MSVC
#  define BOOST_LIB_DIAGNOSTIC
#  include <oh/auto_link.hpp>
#  undef BOOST_LIB_DIAGNOSTIC
#endif
#include <sstream>

DLLEXPORT int xlAutoOpen() {

    // Instantiate the ObjectHandler Repository
    static ObjectHandler::RepositoryXL repositoryXL;

    static XLOPER xDll;

    try {

        Excel(xlGetName, &xDll, 0);

        ObjectHandler::Configuration::instance().init();

        registerOhFunctions(xDll);

        Excel(xlfRegister, 0, 7, &xDll,
            // function code name
            TempStrNoSize("\x00"""),
            // parameter codes
            TempStrNoSize("\x02""C#"),
            // function display name
            TempStrNoSize("\x00"""),
            // comma-delimited list of parameters
            TempStrNoSize("\x00"""),
            // function type (0 = hidden function, 1 = worksheet function, 2 = command macro)
            TempStrNoSize("\x01""1"),
            // function category
            TempStrNoSize("\x07""Example"));

        Excel(xlfRegister, 0, 7, &xDll,
            // function code name
            TempStrNoSize("\x00"""),
            // parameter codes
            TempStrNoSize("\x02""N#"),
            // function display name
            TempStrNoSize("\x00"""),
            // comma-delimited list of parameters
            TempStrNoSize("\x01""x"),
            // function type (0 = hidden function, 1 = worksheet function, 2 = command macro)
            TempStrNoSize("\x01""1"),
            // function category
            TempStrNoSize("\x07""Example"));

        Excel(xlfRegister, 0, 7, &xDll,
            // function code name
            TempStrNoSize("\x00"""),
            // parameter codes
            TempStrNoSize("\x03""CN#"),
            // function display name
            TempStrNoSize("\x00"""),
            // comma-delimited list of parameters
            TempStrNoSize("\x0a""objectID,y"),
            // function type (0 = hidden function, 1 = worksheet function, 2 = command macro)
            TempStrNoSize("\x01""1"),
            // function category
            TempStrNoSize("\x07""Example"));

        Excel(xlFree, 0, 1, &xDll);
        return 1;

    } catch (const std::exception &e) {

        std::ostringstream err;
        err << "Error loading AddinXlHw: " << e.what();
        Excel(xlcAlert, 0, 1, TempStrStl(err.str()));
        Excel(xlFree, 0, 1, &xDll);
        return 0;

    } catch (...) {

        Excel(xlFree, 0, 1, &xDll);
        return 0;

    }
}

DLLEXPORT int xlAutoClose() {

    static XLOPER xDll;

    try {

        Excel(xlGetName, &xDll, 0);

        unregisterOhFunctions(xDll);

        Excel(xlFree, 0, 1, &xDll);
        return 1;

    } catch (const std::exception &e) {

        std::ostringstream err;
        err << "Error unloading AddinXlHw: " << e.what();
        Excel(xlcAlert, 0, 1, TempStrStl(err.str()));
        Excel(xlFree, 0, 1, &xDll);
        return 0;

    } catch (...) {

        Excel(xlFree, 0, 1, &xDll);
        return 0;

    }

}

DLLEXPORT void xlAutoFree(XLOPER *px) {

    freeOper(px);

}

DLLEXPORT std::string *slFunc() {

    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;

    try {

        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>
            (new ObjectHandler::FunctionCall("slFunc"));

        std::string returnValue =
            SimpleLibAddin::func();
{
        static char ret[XL_MAX_STR_LEN];
        ObjectHandler::stringToChar(returnValue, ret);
        return ret;
}
    } catch (const std::exception &e) {

        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);
        return 0;

    }
}

DLLEXPORT char *slAdder(long *x) {

    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;

    try {

        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>
            (new ObjectHandler::FunctionCall("slAdder"));

        boost::shared_ptr<ObjectHandler::ValueObject> valueObject(
            new SimpleLibAddin::ValueObjects::slAdder(objectID, false));

        boost::shared_ptr<ObjectHandler::Object> object(
            new SimpleLibAddin::Adder(valueObject,*objectID, *x));

        std::string returnValue =
            ObjectHandler::RepositoryXL::instance().storeObject(objectID, object, true);

        static char ret[XL_MAX_STR_LEN];
        ObjectHandler::stringToChar(returnValue, ret);
        return ret;

    } catch (const std::exception &e) {

        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);
        return 0;

    }
}

DLLEXPORT long *slAdderAdd(r.q(const).std::string objectID, long y) {

    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;

    try {

        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>
            (new ObjectHandler::FunctionCall("slAdderAdd"));

        OH_GET_REFERENCE(x, objectID, SimpleLibAddin::Adder, SimpleLib::Adder);

        static long ret;
        ret = x->add(*y);
        return &ret;

    } catch (const std::exception &e) {

        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);
        return 0;

    }
}
