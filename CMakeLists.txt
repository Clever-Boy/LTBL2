# This CMakeLists.txt is based on the CMakeLists.txt of the "light" project
# of Zack Hovatter
cmake_minimum_required(VERSION 2.8)

project(ltbl)

# Set options
set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
set(BUILD_SHARED_LIBS TRUE CACHE BOOL "TRUE to build ltbl as shared libraries, FALSE to build it as static libraries")
set(SFML_STATIC_LIBS FALSE CACHE BOOL "Choose whether SFML is linked statically or not.")
set(ltbl_STATIC_STD_LIBS FALSE CACHE BOOL "Use statically linked standard/runtime libraries? This option must match the one used for SFML.")
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# Compile ltbl with C++11 features
if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++11 -Wall -Wextra")
endif()

# Make sure that the runtime library gets linked statically
if (ltbl_STATIC_STD_LIBS)
    if (NOT SFML_STATIC_LIBS)
        message("\n-> If you checked ltbl_STATIC_STD_LIBS, you also need to check SFML_STATIC_LIBRARIES.")
        message("\n-> It would lead to multiple runtime environments which would result in undefined behavior.\n")
    elseif (WIN32 AND MSVC)
        # Change all MSVC compiler flags to /MT
        foreach (flag CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
            if (${flag} MATCHES "/MD")
                string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
            endif()
    elseif (CMAKE_COMPILER_IS_GNUCXX)
        # Doesn't work for the TDM compiler, since it compiles the runtime libs statically by default
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")
    endif()
endif()

# Make sure that FindSFML.cmake searches for the static libraries
if (SFML_STATIC_LIBS)
    set(SFML_STATIC_LIBRARIES TRUE)
endif()

# Find SFML
find_package(SFML 2 COMPONENTS graphics window system)

# Output an error if SFML wasn't found
if (SFML_FOUND)
    include_directories(${SFML_INCLUDE_DIR})
else()
    set(SFML_ROOT "" CACHE PATH "SFML top-level directory")
    message("\n-> SFML directory not found. Set SFML_ROOT to SFML's top level path (containing \"include\" and \"lib\" directories).")
    message("-> Make sure the SFML libraries with the same configuration (Release/Debug, Static/Dynamic) exist.\n")
endif()

# ltbl project
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")
set(ltbl_src
    src/lighting/LightDirectionEmission.cpp
    src/lighting/LightPointEmission.cpp
    src/lighting/LightSystem.cpp
    src/tools/Math.cpp
    src/quadtree/DynamicQuadtree.cpp
    src/quadtree/Quadtree.cpp
    src/quadtree/QuadtreeNode.cpp
    src/quadtree/QuadtreeOccupant.cpp
    src/quadtree/StaticQuadtree.cpp)
include_directories("inc")

add_library(ltbl ${ltbl_src})

# Adjust the output file prefix/suffix to match our conventions
if (BUILD_SHARED_LIBS)
    if (WIN32)
        set_target_properties(ltbl PROPERTIES DEBUG_POSTFIX -d)
        if (CMAKE_COMPILER_IS_GNUCXX)
            # On WIndows/GCC, get rid of "lib" prefix for shared libraries
            # and transform the ".dll.a" suffix into ".a" for import libraries
            set_target_properties(ltbl PROPERTIES PREFIX "")
            set_target_properties(ltbl PROPERTIES IMPORT_SUFFIX ".a")
        endif()
    endif()
else()
    set_target_properties(ltbl PROPERTIES DEBUG_POSTFIX -s-d)
    set_target_properties(ltbl PROPERTIES RELEASE POSTFIX -s)
    set_target_properties(ltbl PROPERTIES MINSIZEREL_POSTFIX -s)
endif()

install(
    TARGETS ltbl
    RUNTIME DESTINATION bin COMPONENT bin
    LIBRARY DESTINATION lib COMPONENT bin
    ARCHIVE DESTINATION lib COMPONENT dev
)
install(DIRECTORY include DESTINATION .)

# Compile the examples
add_subdirectory(examples bin)
