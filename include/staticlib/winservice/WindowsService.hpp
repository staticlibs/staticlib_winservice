/* 
 * File:   WindowsService.hpp
 * Author: alex
 *
 * Created on August 8, 2016, 2:46 PM
 */

#ifndef STATICLIB_WINSERVICE_WINDOWSSERVICE_HPP
#define	STATICLIB_WINSERVICE_WINDOWSSERVICE_HPP

#include <functional>
#include <string>

#include "staticlib/pimpl.hpp"

#include "staticlib/winservice/WindowsServiceException.hpp"

namespace staticlib {
namespace winservice {

/**
 * Context object for one or more HTTP requests. Caches TCP connections.
 */
class WindowsService : public staticlib::pimpl::PimplObject {
    /**
     * Implementation class
     */
    class Impl;

public:
    /**
     * PIMPL-specific constructor
     * 
     * @param pimpl impl object
     */
    PIMPL_CONSTRUCTOR(WindowsService)
            
    WindowsService(std::function<void()> starter, std::function<void()> stopper);
            
    static void install(const std::string& service_name, const std::string& display_name,
            const std::string& start_type, const std::string& dependencies, 
            const std::string& account, const std::string& password);

    static void uninstall(const std::string& service_name);
    
    void start();
            
};

} // namespace
}

#endif	/* STATICLIB_WINSERVICE_WINDOWSSERVICE_HPP */

