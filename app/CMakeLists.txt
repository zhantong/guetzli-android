# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_definitions(-D__USE_OPENCL__)

set(GUETZLI_ROOT
        src/main/cpp/guetzli
)
include_directories(AFTER
        ${GUETZLI_ROOT}
        ${GUETZLI_ROOT}/third_party/butteraugli
        ${GUETZLI_ROOT}/clguetzli
        ${GUETZLI_ROOT}/clguetzli/include
)

set(GUETZLI_SRC
	${GUETZLI_ROOT}/guetzli/butteraugli_comparator.cc
	${GUETZLI_ROOT}/guetzli/dct_double.cc
	${GUETZLI_ROOT}/guetzli/debug_print.cc
	${GUETZLI_ROOT}/guetzli/entropy_encode.cc
	${GUETZLI_ROOT}/guetzli/fdct.cc
	${GUETZLI_ROOT}/guetzli/gamma_correct.cc
	${GUETZLI_ROOT}/guetzli/guetzli.cc
	${GUETZLI_ROOT}/guetzli/idct.cc
	${GUETZLI_ROOT}/guetzli/jpeg_data.cc
	${GUETZLI_ROOT}/guetzli/jpeg_data_decoder.cc
	${GUETZLI_ROOT}/guetzli/jpeg_data_encoder.cc
	${GUETZLI_ROOT}/guetzli/jpeg_data_reader.cc
	${GUETZLI_ROOT}/guetzli/jpeg_data_writer.cc
	${GUETZLI_ROOT}/guetzli/jpeg_huffman_decode.cc
	${GUETZLI_ROOT}/guetzli/output_image.cc
	${GUETZLI_ROOT}/guetzli/preprocess_downsample.cc
	${GUETZLI_ROOT}/guetzli/processor.cc
	${GUETZLI_ROOT}/guetzli/quality.cc
	${GUETZLI_ROOT}/guetzli/quantize.cc
	${GUETZLI_ROOT}/guetzli/score.cc

	${GUETZLI_ROOT}/third_party/butteraugli/butteraugli/butteraugli.cc

	${GUETZLI_ROOT}/clguetzli/cl.hpp
	${GUETZLI_ROOT}/clguetzli/clbutter_comparator.cpp
	${GUETZLI_ROOT}/clguetzli/clbutter_comparator.h
	${GUETZLI_ROOT}/clguetzli/clguetzli.cl
	${GUETZLI_ROOT}/clguetzli/clguetzli.cl.cpp
	${GUETZLI_ROOT}/clguetzli/clguetzli.cl.h
	${GUETZLI_ROOT}/clguetzli/clguetzli.cpp
	${GUETZLI_ROOT}/clguetzli/clguetzli.cu
	${GUETZLI_ROOT}/clguetzli/clguetzli.h
	${GUETZLI_ROOT}/clguetzli/clguetzli_test.cpp
	${GUETZLI_ROOT}/clguetzli/clguetzli_test.h
	${GUETZLI_ROOT}/clguetzli/ocl.cpp
	${GUETZLI_ROOT}/clguetzli/ocl.h
	${GUETZLI_ROOT}/clguetzli/utils.cpp
	${GUETZLI_ROOT}/clguetzli/utils.h
	${GUETZLI_ROOT}/clguetzli/libopencl.c
)

add_library(guetzli
        STATIC
        ${GUETZLI_SRC}
)

set(PNG_ROOT
        ${GUETZLI_ROOT}/third_party/png
)

include_directories(AFTER
        ${PNG_ROOT}
)

set(PNG_SRC
    ${PNG_ROOT}/png.c
    ${PNG_ROOT}/pngerror.c
    ${PNG_ROOT}/pngget.c
    ${PNG_ROOT}/pngmem.c
    ${PNG_ROOT}/pngpread.c
    ${PNG_ROOT}/pngread.c
    ${PNG_ROOT}/pngrio.c
    ${PNG_ROOT}/pngrtran.c
    ${PNG_ROOT}/pngrutil.c
    ${PNG_ROOT}/pngset.c
    ${PNG_ROOT}/pngtrans.c
    ${PNG_ROOT}/pngwio.c
    ${PNG_ROOT}/pngwrite.c
    ${PNG_ROOT}/pngwtran.c
    ${PNG_ROOT}/pngwutil.c
)

add_library(png
        STATIC
        ${PNG_SRC}
)

add_library(native-lib
        SHARED
        src/main/cpp/native-lib.cpp
)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                        native-lib
                        guetzli
                        # Links the target library to the log library
                        # included in the NDK.
                        ${log-lib}

                        png
                        z)