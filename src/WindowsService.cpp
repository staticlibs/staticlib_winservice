/* 
 * File:   WindowsService.cpp
 * Author: alex
 * 
 * Created on August 8, 2016, 2:47 PM
 */

#include "staticlib/winservice/WindowsService.hpp"

#include "staticlib/pimpl/pimpl_forward_macros.hpp"

namespace staticlib {
namespace winservice {

namespace { // anonymous

using callback_type = std::function<void()>;

} // namespace

class WindowsService::Impl : public staticlib::pimpl::PimplObject::Impl {
    callback_type starter;
    callback_type stopper;
    
public:
    Impl(callback_type starter, callback_type stopper) :
    starter(std::move(starter)),
    stopper(std::move(stopper)) { }
    
    ~Impl() STATICLIB_NOEXCEPT { }

    static void install(const std::string& service_name, const std::string& display_name,
            const std::string& start_type, const std::string& dependencies,
            const std::string& account, const std::string& password) {
        (void) service_name;
        (void) display_name;
        (void) start_type;
        (void) dependencies;
        (void) account;
        (void) password;
    }

    static void uninstall(const std::string& service_name) {
        (void) service_name;
    }

    void start(WindowsService&) {
        
    }

};
PIMPL_FORWARD_CONSTRUCTOR(WindowsService, (callback_type)(callback_type), (), WindowsServiceException)
PIMPL_FORWARD_METHOD_STATIC(WindowsService, void, install, (const std::string&)(const std::string&)
        (const std::string&)(const std::string&)(const std::string&)(const std::string&), (), WindowsServiceException)
PIMPL_FORWARD_METHOD_STATIC(WindowsService, void, uninstall, (const std::string&), (), WindowsServiceException)
PIMPL_FORWARD_METHOD(WindowsService, void, start, (), (), WindowsServiceException)

} // namespace
}

