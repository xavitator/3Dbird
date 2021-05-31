cmake_minimum_required(VERSION 3.2)

# List the files of the current local project 
#    Default behavior: Automatically add all hpp and cpp files from src/ directory
#    You may want to change this definition in case of specific file structure
file(GLOB_RECURSE src_files ${CMAKE_CURRENT_LIST_DIR}/src/*.[ch]pp)

# Generate the executable_name from the current directory name
get_filename_component(executable_name ${CMAKE_CURRENT_LIST_DIR} NAME)
# Another possibility is to set your own name: set(executable_name your_own_name) 
message(STATUS "Configure steps to build executable file [${executable_name}]")
project(${executable_name})

# Add current src/ directory
include_directories("src")

# Include files from the library (vcl as well as external dependencies)
include("../../../library/CMakeLists.txt")

 


# Add all files to create executable
#  @src_files: the local file for this project
#  @src_files_vcl: all files of the VCL library
#  @src_files_third_party: all third party libraries compiled with the project
add_executable(${executable_name} ${src_files_vcl} ${src_files_third_party} ${src_files})

# Set Compiler for Unix system
if(UNIX)
   set(CMAKE_CXX_COMPILER g++)                      # Can switch to clang++ if prefered
   add_definitions(-g -O2 -std=c++14 -Wall -Wextra) # Can adapt compiler flags if needed
   add_definitions(-Wno-sign-compare -Wno-type-limits) # Remove some warnings
endif()

# Set Compiler for Windows/Visual Studio
if(MSVC)
    add_definitions(/MP /W4 /wd4244 /wd4127 /wd4267)   # Parallel build (/MP)
    source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${src_files})  #Allow to explore source directories as a tree in Visual Studio
endif()



# Link options for Unix
target_link_libraries(${executable_name} ${GLFW_LIBRARIES})
if(UNIX)
   target_link_libraries(${executable_name} dl) #dlopen is required by Glad on Unix
endif()

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_LIST_DIR}/documentation)

    # request to configure the file
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( documentation
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_IN}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

