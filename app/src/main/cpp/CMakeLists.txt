cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_STANDARD 14)


include_directories(
        ./
        ${ANDROID_NDK}/sources/android/native_app_glue
        common/include
)

add_library(app_glue STATIC ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

#
# target of LearnGLESv3
#
add_library(LearnGLESv3 STATIC
        common/include/esUtil.h
        common/src/esUtil_Android.cpp
        common/src/esUtil.cpp
        common/src/esShader.cpp
        )

target_link_libraries(LearnGLESv3
        app_glue
        # android native share library
        log android EGL GLESv3
)


#
# target of chap2
#
add_library(chap2 SHARED
        session/chap2_hello_triangle.cpp
        )

target_link_libraries(chap2
        app_glue
        LearnGLESv3
        # android native share library
        log android EGL GLESv3
        )


#
# target of chap3
#
add_library(chap3 SHARED
        session/chap3.cpp
        )

target_link_libraries(chap3
        app_glue
        LearnGLESv3
        # android native share library
        log android EGL GLESv3
        )

