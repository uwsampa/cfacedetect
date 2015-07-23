# Install script for directory: /home/mandary/Documents/FANN/src

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
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libfloatfann.so.2.2.0"
      "$ENV{DESTDIR}/usr/local/lib/libfloatfann.so.2"
      "$ENV{DESTDIR}/usr/local/lib/libfloatfann.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libfloatfann.so.2.2.0;/usr/local/lib/libfloatfann.so.2;/usr/local/lib/libfloatfann.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE SHARED_LIBRARY FILES
    "/home/mandary/Documents/FANN/src/libfloatfann.so.2.2.0"
    "/home/mandary/Documents/FANN/src/libfloatfann.so.2"
    "/home/mandary/Documents/FANN/src/libfloatfann.so"
    )
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libfloatfann.so.2.2.0"
      "$ENV{DESTDIR}/usr/local/lib/libfloatfann.so.2"
      "$ENV{DESTDIR}/usr/local/lib/libfloatfann.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libdoublefann.so.2.2.0"
      "$ENV{DESTDIR}/usr/local/lib/libdoublefann.so.2"
      "$ENV{DESTDIR}/usr/local/lib/libdoublefann.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libdoublefann.so.2.2.0;/usr/local/lib/libdoublefann.so.2;/usr/local/lib/libdoublefann.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE SHARED_LIBRARY FILES
    "/home/mandary/Documents/FANN/src/libdoublefann.so.2.2.0"
    "/home/mandary/Documents/FANN/src/libdoublefann.so.2"
    "/home/mandary/Documents/FANN/src/libdoublefann.so"
    )
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libdoublefann.so.2.2.0"
      "$ENV{DESTDIR}/usr/local/lib/libdoublefann.so.2"
      "$ENV{DESTDIR}/usr/local/lib/libdoublefann.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libfixedfann.so.2.2.0"
      "$ENV{DESTDIR}/usr/local/lib/libfixedfann.so.2"
      "$ENV{DESTDIR}/usr/local/lib/libfixedfann.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libfixedfann.so.2.2.0;/usr/local/lib/libfixedfann.so.2;/usr/local/lib/libfixedfann.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE SHARED_LIBRARY FILES
    "/home/mandary/Documents/FANN/src/libfixedfann.so.2.2.0"
    "/home/mandary/Documents/FANN/src/libfixedfann.so.2"
    "/home/mandary/Documents/FANN/src/libfixedfann.so"
    )
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libfixedfann.so.2.2.0"
      "$ENV{DESTDIR}/usr/local/lib/libfixedfann.so.2"
      "$ENV{DESTDIR}/usr/local/lib/libfixedfann.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libfann.so.2.2.0"
      "$ENV{DESTDIR}/usr/local/lib/libfann.so.2"
      "$ENV{DESTDIR}/usr/local/lib/libfann.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libfann.so.2.2.0;/usr/local/lib/libfann.so.2;/usr/local/lib/libfann.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE SHARED_LIBRARY FILES
    "/home/mandary/Documents/FANN/src/libfann.so.2.2.0"
    "/home/mandary/Documents/FANN/src/libfann.so.2"
    "/home/mandary/Documents/FANN/src/libfann.so"
    )
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libfann.so.2.2.0"
      "$ENV{DESTDIR}/usr/local/lib/libfann.so.2"
      "$ENV{DESTDIR}/usr/local/lib/libfann.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/home/mandary/Documents/FANN/src/include/cmake_install.cmake")

endif()

