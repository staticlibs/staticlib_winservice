/*
 * Copyright 2016, alex at staticlibs.net
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * File:   winservice_exception.hpp
 * Author: alex
 *
 * Created on August 8, 2016, 2:48 PM
 */

#ifndef STATICLIB_WINSERVICE_WINSERVICEEXCEPTION_HPP
#define	STATICLIB_WINSERVICE_WINSERVICEEXCEPTION_HPP

#include "staticlib/support/exception.hpp"

namespace staticlib {
namespace winservice {

/**
 * Module specific exception
 */
class winservice_exception : public sl::support::exception {
public:
    /**
     * Default constructor
     */
    winservice_exception() = default;

    /**
     * Constructor with message
     * 
     * @param msg error message
     */
    winservice_exception(const std::string& msg) :
    sl::support::exception(msg) { }

};

} // namespace
} 

#endif	/* STATICLIB_WINSERVICE_WINSERVICEEXCEPTION_HPP */

