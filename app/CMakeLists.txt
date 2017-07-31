# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.



add_library( media_controller-lib SHARED  src/main/cpp/media_controller-lib.cpp
                                          src/main/cpp/EvoInterface/EvoMediaSource.cpp
                                          src/main/cpp/EvoInterface/EvoVideoConvert.cpp
                                          src/main/cpp/EvoInterface/VideoDecoder.cpp
                                          src/main/cpp/MediaDecode.cpp
                                          src/main/cpp/EvoInterface/sei_packet.cpp

)

add_library( libavcodec-56
             SHARED
             IMPORTED )
set_target_properties( libavcodec-56
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../libs/${ANDROID_ABI}/libavcodec-56.so)
#动态库
add_library( libavdevice-56
             SHARED
             IMPORTED )
set_target_properties( libavdevice-56
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../libs/${ANDROID_ABI}/libavdevice-56.so)

add_library( libavfilter-5
             SHARED
             IMPORTED )
set_target_properties( libavfilter-5
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../libs/${ANDROID_ABI}/libavfilter-5.so)

add_library( libavformat-56
             SHARED
             IMPORTED )
set_target_properties( libavformat-56
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../libs/${ANDROID_ABI}/libavformat-56.so)

add_library( libavutil-54
             SHARED
             IMPORTED )
set_target_properties( libavutil-54
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../libs/${ANDROID_ABI}/libavutil-54.so)

add_library( libpostproc-53
             SHARED
             IMPORTED )
set_target_properties( libpostproc-53
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../libs/${ANDROID_ABI}/libpostproc-53.so)

add_library( libswresample-1
             SHARED
             IMPORTED )
set_target_properties( libswresample-1
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../libs/${ANDROID_ABI}/libswresample-1.so)
add_library( libswscale-3
             SHARED
             IMPORTED )
set_target_properties( libswscale-3
                       PROPERTIES IMPORTED_LOCATION
                       ../../../../libs/${ANDROID_ABI}/libswscale-3.so)

include_directories( libs/include )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( log-lib log )
find_library( android-lib android)


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries(media_controller-lib
                        libavcodec-56
                        libavdevice-56
                        libavfilter-5
                        libavformat-56
                        libavutil-54
                        libpostproc-53
                        libswresample-1
                        libswscale-3
                        ${android-lib}
                        ${log-lib} )
