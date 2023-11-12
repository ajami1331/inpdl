if(CMAKE_GENERATOR MATCHES "Visual Studio")
    message(FATAL_ERROR "Visual Studio generator not supported, use: cmake -G Ninja")
endif()
set(CMAKE_SYSTEM_NAME "Linux")
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR "X86")
set(CMAKE_C_COMPILER "/opt/cosmo/bin/cosmocc")
set(CMAKE_CXX_COMPILER "/opt/cosmo/bin/cosmoc++")