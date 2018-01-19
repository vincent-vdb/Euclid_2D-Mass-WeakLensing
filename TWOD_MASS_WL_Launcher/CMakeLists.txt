CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(TWOD_MASS_WL_Launcher)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(TWOD_MASS_WL_CatalogSplitter)
elements_depends_on_subdirs(TWOD_MASS_WL_MassMapping)
elements_depends_on_subdirs(TWOD_MASS_WL_MapMaker)
elements_depends_on_subdirs(TWOD_MASS_WL_PeakCount)

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
elements_add_library(TWOD_MASS_WL_Launcher src/lib/*.cpp
                     LINK_LIBRARIES xerces-c ElementsKernel TWOD_MASS_WL_CatalogSplitter TWOD_MASS_WL_MassMapping TWOD_MASS_WL_MapMaker TWOD_MASS_WL_PeakCount
                     INCLUDE_DIRS xerces-c
                     PUBLIC_HEADERS TWOD_MASS_WL_Launcher)

#===============================================================================
# Declare the executables here
# Example:
# elements_add_executable(ElementsProgramExample src/Program/ProgramExample.cpp
#                        LINK_LIBRARIES Boost ElementsExamples
#                        INCLUDE_DIRS Boost ElementsExamples)
#===============================================================================
elements_add_executable(PFLauncher src/program/PFLauncher.cpp
                     LINK_LIBRARIES ElementsKernel TWOD_MASS_WL_Launcher)

#===============================================================================
# Declare the Boost tests here
# Example:
# elements_add_unit_test(BoostClassExample tests/src/Boost/ClassExample_test.cpp
#                       EXECUTABLE BoostClassExample_test
#                       INCLUDE_DIRS ElementsExamples
#                       LINK_LIBRARIES ElementsExamples TYPE Boost)
#===============================================================================
elements_add_unit_test(XMLParser_test tests/src/XMLParser_test.cpp 
                     LINK_LIBRARIES TWOD_MASS_WL_Launcher
                     TYPE Boost)
elements_add_unit_test(PFAlgo_test tests/src/PFAlgo_test.cpp 
                     LINK_LIBRARIES TWOD_MASS_WL_Launcher
                     TYPE Boost)
elements_add_unit_test(PFLauncherParser_test tests/src/PFLauncherParser_test.cpp 
                     LINK_LIBRARIES TWOD_MASS_WL_Launcher
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
