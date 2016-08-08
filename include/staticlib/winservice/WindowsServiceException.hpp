/* 
 * File:   WindowsServiceException.hpp
 * Author: alex
 *
 * Created on August 8, 2016, 2:48 PM
 */

#ifndef STATICLIB_WINSERVICE_WINDOWSSERVICEEXCEPTION_HPP
#define	STATICLIB_WINSERVICE_WINDOWSSERVICEEXCEPTION_HPP

#include "staticlib/config.hpp"

namespace staticlib {
namespace winservice {

/**
 * Module specific exception
 */
class WindowsServiceException : public staticlib::config::BaseException {
public:
    /**
     * Default constructor
     */
    WindowsServiceException() = default;

    /**
     * Constructor with message
     * 
     * @param msg error message
     */
    WindowsServiceException(const std::string& msg) :
    staticlib::config::BaseException(msg) { }

};

} // namespace
} 

#endif	/* STATICLIB_WINSERVICE_WINDOWSSERVICEEXCEPTION_HPP */

