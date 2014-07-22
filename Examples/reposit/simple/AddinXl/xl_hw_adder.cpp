
/*
 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

#include <ohxl/objecthandlerxl.hpp>
#include <ohxl/register/register_all.hpp>
#include <ohxl/functions/export.hpp>
#include <ohxl/utilities/xlutilities.hpp>
#include <ohxl/objectwrapperxl.hpp>
#include "ValueObjects/vo_hw.hpp"
#include "AddinObjects/obj_hw.hpp"

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
            TempStrNoSize("\x06""slFunc"),              // function code name
            TempStrNoSize("\x02""C#"),                  // parameter codes
            TempStrNoSize("\x06""slFunc"),              // function display name
            TempStrNoSize("\x00"""),                    // comma-delimited list of parameters
            TempStrNoSize("\x01""1"),                   // function type (0 = hidden function, 1 = worksheet function, 2 = command macro)
            TempStrNoSize("\x07""Example"));            // function category

        Excel(xlfRegister, 0, 7, &xDll,
            TempStrNoSize("\x07""slAdder"),             // function code name
            TempStrNoSize("\x04""CCN#"),                // parameter codes
            TempStrNoSize("\x07""slAdder"),             // function display name
            TempStrNoSize("\x0A""ObjectID,x"),          // comma-delimited list of parameters
            TempStrNoSize("\x01""1"),                   // function type (0 = hidden function, 1 = worksheet function, 2 = command macro)
            TempStrNoSize("\x07""Example"));            // function category

        Excel(xlfRegister, 0, 7, &xDll,
            TempStrNoSize("\x0A""slAdderAdd"),          // function code name
            TempStrNoSize("\x04""NCN#"),                // parameter codes
            TempStrNoSize("\x0A""slAdderAdd"),          // function display name
            TempStrNoSize("\x0A""ObjectID,x"),          // comma-delimited list of parameters
            TempStrNoSize("\x01""1"),                   // function type (0 = hidden function, 1 = worksheet function, 2 = command macro)
            TempStrNoSize("\x07""Example"));            // function category

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

        //AccountExample::unregisterEnumeratedTypes();
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

DLLEXPORT char *slFunc() {

    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;

    try {

        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>
            (new ObjectHandler::FunctionCall("slFunc"));

        std::string returnValue = 
            SimpleLibAddin::func();

        static char ret[XL_MAX_STR_LEN];
        ObjectHandler::stringToChar(returnValue, ret);
        return ret;

    } catch (const std::exception &e) {

        ObjectHandler::RepositoryXL::instance().logError(e.what(), functionCall);
        return 0;

    }
}

DLLEXPORT char *slAdder(
        char *objectID,
        long *x) {

    boost::shared_ptr<ObjectHandler::FunctionCall> functionCall;

    try {

        functionCall = boost::shared_ptr<ObjectHandler::FunctionCall>
            (new ObjectHandler::FunctionCall("slAdder"));

        boost::shared_ptr<ObjectHandler::ValueObject> valueObject(
            new ValueObjects::slAdder(objectID, false, *x));

        boost::shared_ptr<ObjectHandler::Object> object(
            new SimpleLibAddin::Adder(valueObject, false, *x));
    
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

DLLEXPORT long *slAdderAdd(char *objectID, long *y) {

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


