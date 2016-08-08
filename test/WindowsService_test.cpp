/* 
 * File:   WindowsService_test.cpp
 * Author: alex
 *
 * Created on August 8, 2016, 3:07 PM
 */

#include "staticlib/winservice/WindowsService.hpp"

#include <iostream>

#include "staticlib/config/assert.hpp"

int main() {
    try {
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

