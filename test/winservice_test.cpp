/*
* File:   winservice_test.cpp
* Author: alex
*
* Created on August 8, 2016, 3:07 PM
*/

#include "staticlib/winservice.hpp"

#include <cstdio>
#include <iostream>
#include <string>

#include "staticlib/config/assert.hpp"
#include "staticlib/utils.hpp"

namespace su = staticlib::utils;
namespace sw = staticlib::winservice;

const std::string logfile = "c:/tmp/winservice_test.log";

void log(su::FileDescriptor& fd, const std::string& msg) {
    fd.write(msg.c_str(), msg.length());
    fd.write("\n", 1);
}

int main(int argc, char* argv[]) {
    // note: switch me for manual testing
    bool skip = true;
    if (skip) return 0;
    // end note
    std::remove(logfile.c_str());
    auto fd = su::FileDescriptor(logfile, 'w');
    try {
        if (1 == argc) {
            log(fd, "enter");
            sw::start_service_and_wait("foo",
                [&fd]{
                    log(fd, "start");
                },
                [&fd]{
                    log(fd, "stop");
                },
                [&fd](const std::string& msg) {
                    log(fd, msg);
                }
            );
            log(fd, "exit");
        } else if (2 != argc) {
            std::string msg = "Invalid arguments, use single argument: '-i' or '-u'";
            std::cout << msg << std::endl;
            log(fd, msg);
            exit(1);
        }    
        std::string arg = std::string(argv[1]);
        if ("-i" == arg) {
            // http://stackoverflow.com/q/31538050/314015
            sw::install_service("foo", "foo_test", "LocalSystem");
        } else if ("-u" == arg) {
            sw::uninstall_service("foo");           
        } else {
            std::string msg = "Invalid argument: [" + arg + "], use single argument: '-i' or '-u'";
            log(fd, msg);
            std::cout << msg << std::endl;
            exit(1);
        }
    }
    catch (const std::exception& e) {
        std::string msg{e.what()};
        log(fd, msg);
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

