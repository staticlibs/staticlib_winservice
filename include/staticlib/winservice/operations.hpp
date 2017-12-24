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
 * File:   operations.hpp
 * Author: alex
 *
 * Created on August 8, 2016, 2:17 PM
 */

#ifndef STATICLIB_WINSERVICE_OPERATIONS_HPP
#define STATICLIB_WINSERVICE_OPERATIONS_HPP

#include <functional>
#include <string>

#include "staticlib/winservice/winservice_exception.hpp"

namespace staticlib {
namespace winservice {

/**
 * Installs current executable as a Windows Service
 * 
 * @param service_name service name
 * @param display_name service name in services list
 * @param account windows account name, 'LocalService' by default
 * @param password windows account password
 * @param start_type service start type, 'SERVICE_AUTO_START' by default
 * @param dependencies service dependencies
 */
void install_service(const std::string& service_name, const std::string& display_name,
        const std::string& account = "NT AUTHORITY\\LocalService", const std::string& password = "",
        const std::string& start_type = "SERVICE_AUTO_START", const std::string& dependencies = "");

/**
 * Uninstalls Windows Service by name
 * 
 * @param service_name service name
 */

void uninstall_service(const std::string& service_name);

/**
 * Starts specified service
 *
 * @param service_name service name
 */
void start_service(const std::string& service_name);

/**
* Sends 'SERVICE_CONTROL_STOP' signal to specified service
*
* @param service_name service name
*/
void stop_service(const std::string& service_name);

/**
 * Entry point for the application called by Service Manager,
 * this call will block until the service will be stopped
 * 
 * @param service_name service name
 * @param starter start callback, will be called on 'START' and 'CONTINUE' events
 * @param stopper stop callback, will be called on 'STOP', 'PAUSE' and 'SHUTDOWN' events
 * @param logger logger callback
 */
void start_service_and_wait(const std::string& service_name, std::function<void()> starter,
        std::function<void()> stopper, std::function<void(const std::string&)> logger);

} // namespace
}

#endif /* STATICLIB_WINSERVICE_OPERATIONS_HPP */
