cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-rtti -fno-exceptions -Wall")

set(glm_dir ${CMAKE_SOURCE_DIR}/glm)
set(gtc_dir ${CMAKE_SOURCE_DIR}/glm/gtc)

set(utils ${CMAKE_SOURCE_DIR}/Utils)
set(exif_dir ${CMAKE_SOURCE_DIR}/exif)
AUX_SOURCE_DIRECTORY(./RenderControl SRCLIST)
AUX_SOURCE_DIRECTORY(./Animation ANI_SRC_LIST)
AUX_SOURCE_DIRECTORY(./GLM GLMLIST)

include_directories(${glm_dir} ${utils} ${gtc_dir} ${exif_dir})

add_library(glrender SHARED
RenderView.cpp GLProgram.cpp Utils/ImageUtil.cpp exif/exif.c exif/exifUtils.c
${SRCLIST} ${GLMLIST} ${ANI_SRC_LIST})

add_library(encryption SHARED
encryption/Encryption.cpp encryption/md5.cpp)

target_link_libraries(glrender  GLESv3 android EGL log m
-ljnigraphics)
