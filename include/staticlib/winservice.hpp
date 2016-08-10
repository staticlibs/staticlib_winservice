/* 
 * File:   winservice.hpp
 * Author: alex
 *
 * Created on August 8, 2016, 2:17 PM
 */

#ifndef STATICLIB_WINSERVICE_HPP
#define	STATICLIB_WINSERVICE_HPP

#include <functional>
#include <string>

#include "staticlib/winservice/WindowsServiceException.hpp"

namespace staticlib {
namespace winservice {

void install_service(const std::string& service_name, const std::string& display_name,
        const std::string& account = "NT AUTHORITY\\LocalService", const std::string& password = "",
        const std::string& start_type = "SERVICE_AUTO_START", const std::string& dependencies = "");

void uninstall_service(const std::string& service_name);

void start_service_and_wait(const std::string& service_name, std::function<void()> starter,
        std::function<void()> stopper, std::function<void(const std::string&)> logger);

} // namespace
}

#endif	/* STATICLIB_WINSERVICE_HPP */
