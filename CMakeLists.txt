project(BLPCONVERTER)
cmake_minimum_required(VERSION 2.6)


##########################################################################################
# Options

option(WITH_LIBRARY "Compile library" OFF)


##########################################################################################
# CMake-related settings

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${BLPCONVERTER_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${BLPCONVERTER_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${BLPCONVERTER_BINARY_DIR}/bin")


##########################################################################################
# Dependencies

add_subdirectory(dependencies)

include_directories("${BLPCONVERTER_SOURCE_DIR}/dependencies/include/"
                    "${BLPCONVERTER_SOURCE_DIR}/dependencies/FreeImage/"
                    "${BLPCONVERTER_SOURCE_DIR}/dependencies/squish/"
)


set(EXECUTABLE_SRCS main.cpp)
set(LIBRARY_SRCS    blp.cpp)
set(LIBRARY_HEADERS blp.h blp_internal.h)


##########################################################################################
# libblp shared library

if (WITH_LIBRARY)
    add_library(blp SHARED ${LIBRARY_SRCS} ${LIBRARY_HEADERS})
    target_link_libraries(blp freeimage squish)

    set_target_properties(blp PROPERTIES COMPILE_DEFINITIONS "FREEIMAGE_LIB"
                                         COMPILE_FLAGS "-fPIC"
                                         BUILD_WITH_INSTALL_RPATH ON
                                         INSTALL_NAME_DIR "@rpath"
    )

    install(TARGETS blp RUNTIME DESTINATION bin
                        LIBRARY DESTINATION lib
                        ARCHIVE DESTINATION lib)
    install(FILES blp.h DESTINATION include)
endif()


##########################################################################################
# BLPConverter executable

if (WITH_LIBRARY)
    add_executable(BLPConverter ${EXECUTABLE_SRCS})
    target_link_libraries(BLPConverter blp)

    if (APPLE)
        set_target_properties(BLPConverter PROPERTIES LINK_FLAGS "-Wl,-rpath,@loader_path/.")
    else()
        set_target_properties(BLPConverter PROPERTIES INSTALL_RPATH ".")
    endif()
else()
    add_executable(BLPConverter ${EXECUTABLE_SRCS} ${LIBRARY_SRCS} ${LIBRARY_HEADERS})
    target_link_libraries(BLPConverter freeimage squish)
endif()

set_target_properties(BLPConverter PROPERTIES COMPILE_DEFINITIONS "FREEIMAGE_LIB")

install(TARGETS BLPConverter RUNTIME DESTINATION bin)
