/*
* File:   winservice.cpp
* Author: alex
*
* Created on August 8, 2016, 2:47 PM
*/

#include "staticlib/winservice.hpp"
#ifdef STATICLIB_WINDOWS

#include <memory>
#include <mutex>

#define NOMINMAX
#include <windows.h>

#include "staticlib/config.hpp"
#include "staticlib/support.hpp"
#include "staticlib/utils.hpp"

namespace staticlib {
namespace winservice {

namespace { // anonymous

class service_handle_deleter {
public:
    void operator()(SC_HANDLE handle) {
        CloseServiceHandle(handle);
    }
};

class service_ctx {
    std::string name;
    std::wstring wname;
    std::function<void()> starter = []{};
    std::function<void()> stopper = []{};
    std::function<void(const std::string&)> logger = [](const std::string&){};
    SERVICE_STATUS status;
    SERVICE_STATUS_HANDLE status_handle = nullptr;
    bool initialized = false;

public:

    service_ctx(const service_ctx&) = delete;

    service_ctx& operator=(const service_ctx&) = delete;

    service_ctx(service_ctx&& other) = delete;

    service_ctx& operator=(service_ctx&& other) {
        name = std::move(other.name);
        wname = std::move(other.wname);
        starter = std::move(other.starter);
        stopper = std::move(other.stopper);
        logger = std::move(other.logger);
        status = std::move(other.status);
        status_handle = std::move(other.status_handle);
        initialized = other.initialized;
        return *this;
    }

    service_ctx() { }

    service_ctx(const std::string& name, std::function<void()> starter, 
            std::function<void()> stopper, std::function<void(const std::string&)> logger) :
    name(name.data(), name.length()),
    wname(sl::utils::widen(name)),
    starter(starter),
    stopper(stopper),
    logger(logger) {
        status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
        status.dwCurrentState = SERVICE_START_PENDING;
        status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;
        status.dwWin32ExitCode = NO_ERROR;
        status.dwServiceSpecificExitCode = 0;
        status.dwCheckPoint = 0;
        status.dwWaitHint = 0;

        initialized = true;
    }

    void log(const std::string& msg) {
        logger(msg);
    }

    bool is_initialized() {
        return initialized;
    }

    std::string& get_name() {
        return name;
    }

    std::wstring& get_wname() {
        return wname;
    }

    SERVICE_STATUS& get_status() {
        return status;
    }

    SERVICE_STATUS_HANDLE get_status_handle() {
        return status_handle;
    }

    void set_status_handle(SERVICE_STATUS_HANDLE ha) {
       this->status_handle = ha;
    }

    void start() {
        starter();
    }

    void stop() {
        stopper();
    }

};

DWORD resolve_start_type(const std::string& str) {
    if ("SERVICE_BOOT_START" == str) return SERVICE_BOOT_START;
    else if ("SERVICE_AUTO_START" == str) return SERVICE_AUTO_START;
    else if ("SERVICE_DEMAND_START" == str) return SERVICE_DEMAND_START;
    else if ("SERVICE_DISABLED" == str) return SERVICE_DISABLED;
    else throw winservice_exception(TRACEMSG("Invalid 'start_type' specified: [" + str + "]"));
}

std::mutex& static_mutex() {
    static std::mutex mutex{};
    return mutex;
}

service_ctx& static_ctx() {
    static service_ctx ctx{};
    return ctx;
}

void set_static_context(const std::string& service_name, std::function<void()> starter, 
        std::function<void()> stopper, std::function<void(const std::string&)> logger) {
    std::lock_guard<std::mutex> guard{static_mutex()};
    if (static_ctx().is_initialized()) throw winservice_exception(TRACEMSG(
            "Windows service start attempt was already done in this process"));
    static_ctx() = service_ctx(service_name, starter, stopper, logger);
}

void set_service_status(DWORD status, DWORD error = 0) {
    auto& st = static_ctx().get_status();
    st.dwCurrentState = status;
    st.dwWin32ExitCode = error;
    if (SERVICE_RUNNING == status || SERVICE_STOPPED == status) {
        st.dwCheckPoint = 0;
    } else {
        st.dwCheckPoint += 1;
    }
    auto success = SetServiceStatus(static_ctx().get_status_handle(), std::addressof(st));
    // do not throw if error reporting is in progress
    if (!success && 0 == error) throw winservice_exception(TRACEMSG(
            "Error changing status to: [" + sl::support::to_string(status) + "]," +
            " error: [" + sl::utils::errcode_to_string(GetLastError()) + "]"));
}

void start_service(DWORD pending, DWORD target) STATICLIB_NOEXCEPT{
    try {
        set_service_status(pending);
        static_ctx().start();
        set_service_status(target);
    } catch (const std::exception& e) {
        static_ctx().log(TRACEMSG(e.what() + "\nError starting service," +
                " pending: [" + sl::support::to_string(pending) + "], target: [" + sl::support::to_string(target) + "]"));
        set_service_status(SERVICE_STOPPED, 1);
    } catch (...) {
        static_ctx().log(TRACEMSG("Error starting service," +
            " pending: [" + sl::support::to_string(pending) + "], target: [" + sl::support::to_string(target) + "]"));
        set_service_status(SERVICE_STOPPED, 2);
    }
}

void stop_service(DWORD pending, DWORD target) STATICLIB_NOEXCEPT{
    try {
        set_service_status(pending);
        static_ctx().stop();
        set_service_status(target);
    } catch (const std::exception& e) {
        static_ctx().log(TRACEMSG(e.what() + "\nError stopping service," +
            " pending: [" + sl::support::to_string(pending) + "], target: [" + sl::support::to_string(target) + "]"));
        set_service_status(SERVICE_STOPPED, 1);
    } catch (...) {
        static_ctx().log(TRACEMSG("Error stopping service," +
            " pending: [" + sl::support::to_string(pending) + "], target: [" + sl::support::to_string(target) + "]"));
        set_service_status(SERVICE_STOPPED, 2);
    }
}

void WINAPI service_control_handler(DWORD control_step) STATICLIB_NOEXCEPT {
    std::lock_guard<std::mutex> guard{ static_mutex() };
    switch (control_step) {
    case SERVICE_CONTROL_STOP: stop_service(SERVICE_STOP_PENDING, SERVICE_STOPPED); break;
    case SERVICE_CONTROL_PAUSE: stop_service(SERVICE_PAUSE_PENDING, SERVICE_PAUSED); break;
    case SERVICE_CONTROL_CONTINUE: start_service(SERVICE_CONTINUE_PENDING, SERVICE_RUNNING); break;
    case SERVICE_CONTROL_SHUTDOWN: stop_service(SERVICE_STOP_PENDING, SERVICE_STOPPED); break;
    default: break;
    }
}

void WINAPI service_main(DWORD, LPWSTR*) STATICLIB_NOEXCEPT {
    std::lock_guard<std::mutex> guard{static_mutex()};
    // Register the handler function for the service
    auto ha = RegisterServiceCtrlHandlerW(static_ctx().get_wname().c_str(), service_control_handler);
    if (nullptr == ha) {
        static_ctx().log(TRACEMSG(
                "Fatal error on RegisterServiceCtrlHandlerW: [" + sl::utils::errcode_to_string(GetLastError()) + "]"));
        ::exit(-1);
    }
    static_ctx().set_status_handle(ha);
    start_service(SERVICE_START_PENDING, SERVICE_RUNNING);
}

} // namespace

void install_service(const std::string& service_name, const std::string& display_name,
    const std::string& account, const std::string& password,
    const std::string& start_type, const std::string& dependencies) {
    auto scm = std::unique_ptr<SC_HANDLE__, service_handle_deleter>(
            OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE),
            service_handle_deleter());
    if (nullptr == scm.get()) throw winservice_exception(TRACEMSG(
            "Cannot open SCM, error: [" + sl::utils::errcode_to_string(GetLastError()) + "]"));
    std::string exec_path = sl::utils::current_executable_path();
    auto stype = resolve_start_type(start_type);
    auto service = std::unique_ptr<SC_HANDLE__, service_handle_deleter>(
            CreateServiceW(
                scm.get(),                          // SCManager database
                sl::utils::widen(service_name).c_str(),    // Name of service
                sl::utils::widen(display_name).c_str(),    // Name to display
                SERVICE_QUERY_STATUS,               // Desired access
                SERVICE_WIN32_OWN_PROCESS,          // Service type
                stype,                              // Service start type
                SERVICE_ERROR_NORMAL,               // Error control type
                sl::utils::widen(exec_path).c_str(),       // Service's binary
                nullptr,                            // No load ordering group
                nullptr,                            // No tag identifier
                dependencies.empty() ? nullptr :
                sl::utils::widen(dependencies).c_str(),   // Dependencies
                sl::utils::widen(account).c_str(),         // Service running account
                password.empty() ? nullptr :
                sl::utils::widen(password).c_str()        // Password of the account
            ), service_handle_deleter());
    if (nullptr == service.get()) throw winservice_exception(TRACEMSG(
            "Cannot create service, error: [" + sl::utils::errcode_to_string(GetLastError()) + "]"));
}

void uninstall_service(const std::string& service_name) {
    auto scm = std::unique_ptr<SC_HANDLE__, service_handle_deleter>(
            OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT),
            service_handle_deleter());
    if (nullptr == scm.get()) throw winservice_exception(TRACEMSG(
            "Cannot open SCM, error: [" + sl::utils::errcode_to_string(GetLastError()) + "]"));
    auto service = std::unique_ptr<SC_HANDLE__, service_handle_deleter>(
            OpenServiceW(scm.get(), sl::utils::widen(service_name).c_str(), SERVICE_QUERY_STATUS | DELETE), service_handle_deleter());
    if (nullptr == service.get()) throw winservice_exception(TRACEMSG(
            "Cannot open service, name: [" + service_name + "], error: [" + sl::utils::errcode_to_string(GetLastError()) + "]"));
    SERVICE_STATUS_PROCESS ssp;
    DWORD len;
    auto err_query = QueryServiceStatusEx(service.get(), SC_STATUS_PROCESS_INFO,
            reinterpret_cast<BYTE*> (std::addressof(ssp)), sizeof(SERVICE_STATUS_PROCESS), std::addressof(len));
    if (!err_query) throw winservice_exception(TRACEMSG(
            "Error querying service status, name: [" + service_name + "]," +
            " error: [" + sl::utils::errcode_to_string(GetLastError()) + "]"));
    if (SERVICE_STOPPED != ssp.dwCurrentState) throw winservice_exception(TRACEMSG(
            "Error uninstalling service, name: [" + service_name + "],"
            " service must be stopped before the uninstallation"));
    auto success = DeleteService(service.get());
    if (!success) throw winservice_exception(TRACEMSG(
            "Error uninstalling service, name: [" + service_name + "]," +
            " error: [" + sl::utils::errcode_to_string(GetLastError()) + "]"));
}

void start_service(const std::string& service_name) {
    auto scm = std::unique_ptr<SC_HANDLE__, service_handle_deleter>(
        OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT),
        service_handle_deleter());
    if (nullptr == scm.get()) throw winservice_exception(TRACEMSG(
        "Cannot open SCM, error: [" + sl::utils::errcode_to_string(GetLastError()) + "]"));
    auto service = std::unique_ptr<SC_HANDLE__, service_handle_deleter>(
        OpenServiceW(scm.get(), sl::utils::widen(service_name).c_str(), SERVICE_START), service_handle_deleter());
    if (nullptr == service.get()) throw winservice_exception(TRACEMSG(
        "Cannot open service, name: [" + service_name + "], error: [" + sl::utils::errcode_to_string(GetLastError()) + "]"));
    auto success = StartService(service.get(), 0, nullptr);
    if (!success) throw winservice_exception(TRACEMSG(
            "Error starting service, name: [" + service_name + "]," +
            " error: [" + sl::utils::errcode_to_string(GetLastError()) + "]"));
}

void stop_service(const std::string& service_name) {
    auto scm = std::unique_ptr<SC_HANDLE__, service_handle_deleter>(
        OpenSCManagerW(NULL, NULL, SC_MANAGER_CONNECT),
        service_handle_deleter());
    if (nullptr == scm.get()) throw winservice_exception(TRACEMSG(
        "Cannot open SCM, error: [" + sl::utils::errcode_to_string(GetLastError()) + "]"));
    auto service = std::unique_ptr<SC_HANDLE__, service_handle_deleter>(
        OpenServiceW(scm.get(), sl::utils::widen(service_name).c_str(), SERVICE_STOP), service_handle_deleter());
    if (nullptr == service.get()) throw winservice_exception(TRACEMSG(
        "Cannot open service, name: [" + service_name + "], error: [" + sl::utils::errcode_to_string(GetLastError()) + "]"));
    SERVICE_STATUS ss;
    auto success = ControlService(service.get(), SERVICE_CONTROL_STOP, std::addressof(ss));
    if (!success) throw winservice_exception(TRACEMSG(
            "Error stopping service, name: [" + service_name + "]," +
            " error: [" + sl::utils::errcode_to_string(GetLastError()) + "]"));
}

void start_service_and_wait(const std::string& service_name, std::function<void()> starter,
        std::function<void()> stopper, std::function<void(const std::string&)> logger) {
    set_static_context(service_name, std::move(starter), std::move(stopper), std::move(logger));
    std::wstring name = sl::utils::widen(service_name);
    SERVICE_TABLE_ENTRYW st[] = {
        { std::addressof(name.front()), service_main },
        { nullptr, nullptr }
    };

    // Connects the main thread of a service process to the service control 
    // manager, which causes the thread to be the service control dispatcher 
    // thread for the calling process. This call returns when the service has 
    // stopped. The process should simply terminate when the call returns.
    auto success = StartServiceCtrlDispatcherW(st);
    if (!success) throw winservice_exception(TRACEMSG(
        "Error starting service, name: [" + service_name + "]," +
        " error: [" + sl::utils::errcode_to_string(GetLastError()) + "]"));
}

} // namespace
}

#endif // STATICLIB_WINDOWS
