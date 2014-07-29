
/*
 Copyright (C) 2014 Eric Ehlers

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

//#if defined(HAVE_CONFIG_H)     // Dynamically created by configure
//    #include <oh/config.hpp>
//#endif

#include <oh/property.hpp>
#include "creators.hpp"
#include <AddinObjects/obj_adder.hpp>

namespace FullLibAddin {

    boost::shared_ptr<ObjectHandler::Object> createAdder(
        const boost::shared_ptr<ObjectHandler::ValueObject> &valueObject) {

        //long x = ObjectHandler::convert2<long>(valueObject->getProperty("Number"));
        //bool permanent = ObjectHandler::convert2<bool>(valueObject->getProperty("Permanent"));

        boost::shared_ptr<ObjectHandler::Object> object(
            new Adder(valueObject, 0, false));
        return object;
    }
}

