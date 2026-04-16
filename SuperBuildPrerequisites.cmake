if(DEFINED slicersources_SOURCE_DIR AND NOT DEFINED Slicer_SOURCE_DIR)
  set(Slicer_SOURCE_DIR ${slicersources_SOURCE_DIR})
endif()

set(SlicerVirtualReality_HAS_OPENVR_SUPPORT OFF)
set(SlicerVirtualReality_HAS_OPENXR_SUPPORT OFF)
set(SlicerVirtualReality_HAS_OPENXRREMOTING_SUPPORT OFF)

if(DEFINED Slicer_SOURCE_DIR)
  set(SlicerVirtualReality_EXTERNAL_PROJECT_DEPENDENCIES "")
else()
  set(SlicerVirtualReality_EXTERNAL_PROJECT_DEPENDENCIES "")
endif()
message(STATUS "SlicerVirtualReality_EXTERNAL_PROJECT_DEPENDENCIES:${SlicerVirtualReality_EXTERNAL_PROJECT_DEPENDENCIES}")
