# Sets the minimum version of CMake required to build the native
# library. You should either keep the default value or only pass a
# value of 3.4.0 or lower.

cmake_minimum_required(VERSION 3.4.1)


#set(CMAKE_VERBOSE_MAKEFILE on)
set(libs3rd "${CMAKE_SOURCE_DIR}/lib3rd")
#include_directories(${CMAKE_SOURCE_DIR}/src/main/cpp/include)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds it for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             native-rtmp

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             # Associated headers in the same location as their source
             # file are automatically included.
             src/main/cpp/jni_onload.cpp
             src/main/cpp/RtmpPusher.cpp
              )

add_library( librtmp
             STATIC
             IMPORTED )

set_target_properties( # Specifies the target library.
                       librtmp
                       # Specifies the parameter you want to define.
                       PROPERTIES IMPORTED_LOCATION
                       # Provides the path to the library you want to import.
                       ${libs3rd}/${ANDROID_ABI}/librtmp.a )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because system libraries are included in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in the
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                        native-rtmp

                       # Links the target library to the log library
                      # included in the NDK.
                       ${log-lib}
                       librtmp )

