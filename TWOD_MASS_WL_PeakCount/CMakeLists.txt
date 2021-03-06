CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(TWOD_MASS_WL_PeakCount)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(TWOD_MASS_WL_MassMapping)
elements_depends_on_subdirs(TWOD_MASS_WL_MapMaker)

#===============================================================================
# Add the find_package macro (a pure CMake command) here to locate the
# libraries.
# Examples:
#          find_package(CppUnit)
#===============================================================================

#===============================================================================
# Declare the library dependencies here
# Example:
#         elements_add_library(ElementsExamples src/Lib/*.cpp
#                     LINK_LIBRARIES Boost ElementsKernel
#                     INCLUDE_DIRS Boost ElementsKernel
#                     PUBLIC_HEADERS ElementsExamples)
#===============================================================================
elements_add_library(TWOD_MASS_WL_PeakCount src/lib/*.cpp
                     LINK_LIBRARIES ElementsKernel TWOD_MASS_WL_MassMapping TWOD_MASS_WL_MapMaker
                     PUBLIC_HEADERS TWOD_MASS_WL_PeakCount)

#===============================================================================
# Declare the executables here
# Example:
# elements_add_executable(ElementsProgramExample src/Program/ProgramExample.cpp
#                        LINK_LIBRARIES Boost ElementsExamples
#                        INCLUDE_DIRS Boost ElementsExamples)
#===============================================================================
elements_add_executable(PeakCount src/program/PeakCount.cpp
                     LINK_LIBRARIES ElementsKernel TWOD_MASS_WL_PeakCount)

#===============================================================================
# Declare the Boost tests here
# Example:
# elements_add_unit_test(BoostClassExample tests/src/Boost/ClassExample_test.cpp
#                       EXECUTABLE BoostClassExample_test
#                       INCLUDE_DIRS ElementsExamples
#                       LINK_LIBRARIES ElementsExamples TYPE Boost)
#===============================================================================
elements_add_unit_test(PeakCountAlgo_test tests/src/PeakCountAlgo_test.cpp 
                     LINK_LIBRARIES TWOD_MASS_WL_PeakCount
                     TYPE Boost)
elements_add_unit_test(PeakCountParser_test tests/src/PeakCountParser_test.cpp 
                     LINK_LIBRARIES TWOD_MASS_WL_PeakCount
                     TYPE Boost)

#===============================================================================
# Declare the Python programs here
# Examples :
# elements_add_python_program(PythonProgramExample
#                             ElementsExamples.PythonProgramExample)
#===============================================================================

#===============================================================================
# Use the following macro for python modules, scripts and aux files:
#  elements_install_python_modules()
#  elements_install_scripts()
#===============================================================================

#===============================================================================
# Add the elements_install_conf_files macro
# Examples:
#          elements_install_conf_files()
#===============================================================================
elements_install_conf_files()
