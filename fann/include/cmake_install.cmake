# Install script for directory: /home/mandary/Documents/FANN/src/include

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/mandary/Documents/FANN/src/include/fann.h"
    "/home/mandary/Documents/FANN/src/include/doublefann.h"
    "/home/mandary/Documents/FANN/src/include/fann_internal.h"
    "/home/mandary/Documents/FANN/src/include/floatfann.h"
    "/home/mandary/Documents/FANN/src/include/fann_data.h"
    "/home/mandary/Documents/FANN/src/include/fixedfann.h"
    "/home/mandary/Documents/FANN/src/include/compat_time.h"
    "/home/mandary/Documents/FANN/src/include/fann_activation.h"
    "/home/mandary/Documents/FANN/src/include/fann_cascade.h"
    "/home/mandary/Documents/FANN/src/include/fann_error.h"
    "/home/mandary/Documents/FANN/src/include/fann_train.h"
    "/home/mandary/Documents/FANN/src/include/fann_io.h"
    "/home/mandary/Documents/FANN/src/include/fann_cpp.h"
    )
endif()

