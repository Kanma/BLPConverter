# List the source files
set(SRCS alpha.cpp
         clusterfit.cpp
         colourblock.cpp
         colourfit.cpp
         colourset.cpp
         maths.cpp
         rangefit.cpp
         singlecolourfit.cpp
         squish.cpp
)

# List the include paths
include_directories(.)

# Declaration of the library
add_library(squish STATIC ${SRCS})

# Compilation settings
if (NOT WIN32)
    set_target_properties(freeimage PROPERTIES COMPILE_FLAGS "-w -fPIC")
else()
    set_target_properties(freeimage PROPERTIES COMPILE_FLAGS "/W0")
endif()
