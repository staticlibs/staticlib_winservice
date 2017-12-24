Windows Services integration library for Staticlibs
===================================================

[![appveyor](https://ci.appveyor.com/api/projects/status/github/staticlibs/staticlib_winservice?svg=true)](https://ci.appveyor.com/project/staticlibs/staticlib-winservice)

This project is a part of [Staticlibs](http://staticlibs.net/).

This library provides a high-level wrapper for [Windows Services API](https://msdn.microsoft.com/en-us/library/windows/desktop/ms685942%28v=vs.85%29.aspx).

Link to the [API documentation](http://staticlibs.github.io/staticlib_winservice/docs/html/namespacestaticlib_1_1winservice.html).

Usage example
-------------

Install current executable as a service:

    sl::winservice::install_service("foo", "Foo Service Description", "LocalSystem");

Uninstall service by name:

    sl::winservice::uninstall_service("foo");

Start application as a service when service for this executable is already installed and being run by
Service Control Manager. This call will block until the service will be stopped:

    sl::winservice::start_service_and_wait(
            "foo", // service name
            [&]{ ... }, // start callback                
            [&]{ ... }, // stop callback
            [&](const std::string& msg) { ... } // logger callback
    );

Service arguments are currently not supported (config file may be used instead).

How to build
------------

[CMake](http://cmake.org/) is required for building.

[pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/) utility is used for dependency management.
For Windows users ready-to-use binary version of `pkg-config` can be obtained from [tools_windows_pkgconfig](https://github.com/staticlibs/tools_windows_pkgconfig) repository.
See [StaticlibsPkgConfig](https://github.com/staticlibs/wiki/wiki/StaticlibsPkgConfig) for Staticlibs-specific details about `pkg-config` usage.

To build the library on Windows using Visual Studio 2013 Express run the following commands using
Visual Studio development command prompt 
(`C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\Shortcuts\VS2013 x86 Native Tools Command Prompt`):

    git clone https://github.com/staticlibs/staticlib_config.git
    git clone https://github.com/staticlibs/staticlib_support.git
    git clone https://github.com/staticlibs/staticlib_utils.git
    git clone https://github.com/staticlibs/staticlib_winservice.git
    cd staticlib_winservice
    mkdir build
    cd build
    cmake ..
    msbuild staticlib_winservice.sln


See [StaticlibsToolchains](https://github.com/staticlibs/wiki/wiki/StaticlibsToolchains) for 
more information about the toolchain setup and cross-compilation.

License information
-------------------

This project is released under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).

Changelog
---------

**2017-12-24**

 * version 1.0.2
 * vs2017 support

**2017-05-22**

 * version 1.0.1
 * deps update

**2016-08-09**

 * version 1.0
 * initial public version
