cmake_minimum_required(VERSION 3.5)

project(Slicer3D)

set(EXTENSION_HOMEPAGE "https://github.com/KitwareMedical/SlicerVirtualReality")
set(EXTENSION_CATEGORY "Stereo Display")
set(EXTENSION_CONTRIBUTORS "Jean-Baptiste Vimort (Kitware), Jean-Christophe Fillion-Robin (Kitware), Csaba Pinter (PerkLab, Queen's University), Adam Rankin (VASST Lab, Robarts Research Institute, Western University)")
set(EXTENSION_DESCRIPTION "Dual projection stereo 3D display for medical imaging. Supports polarized projection systems with separate left/right eye rendering.")
set(EXTENSION_ICONURL "https://raw.githubusercontent.com/KitwareMedical/SlicerVirtualReality/master/SlicerVirtualReality.png")
set(EXTENSION_SCREENSHOTURLS "https://www.slicer.org/slicerWiki/images/4/49/SlicerVirtualReality_Screenshot1.png")
set(EXTENSION_DEPENDS "NA")

set(EXTENSION_BUILD_SUBDIRECTORY inner-build)

set(SUPERBUILD_TOPLEVEL_PROJECT inner)

option(Slicer3D_SUPERBUILD "Build Slicer3D and the projects it depends on." ON)
mark_as_advanced(Slicer3D_SUPERBUILD)
if(Slicer3D_SUPERBUILD)
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuildPrerequisites.cmake")
  include("${CMAKE_CURRENT_SOURCE_DIR}/SuperBuild.cmake")
  return()
endif()

find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

find_package(Git REQUIRED)

add_subdirectory(VirtualReality)

set(Slicer3D_CUSTOM_CONFIG "####### Expanded from \@Slicer3D_CUSTOM_CONFIG\@ #######

##################################################
")
include(${Slicer_EXTENSION_GENERATE_CONFIG})

set(EXTENSION_CPACK_INSTALL_CMAKE_PROJECTS)

set(${EXTENSION_NAME}_CPACK_INSTALL_CMAKE_PROJECTS "${EXTENSION_CPACK_INSTALL_CMAKE_PROJECTS}" CACHE STRING "List of external projects to install" FORCE)

list(APPEND CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_BINARY_DIR};${EXTENSION_NAME};ALL;/")
list(APPEND CPACK_INSTALL_CMAKE_PROJECTS "${${EXTENSION_NAME}_CPACK_INSTALL_CMAKE_PROJECTS}")
include(${Slicer_EXTENSION_CPACK})
