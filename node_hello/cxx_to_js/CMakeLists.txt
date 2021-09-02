cmake_minimum_required(VERSION 3.10)
project(HelloWorld)
set(io_components ITKImageIO)
if(EMSCRIPTEN)
  set(io_components BridgeJavaScript)
endif()
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

set(itkfiltering_SRCS itk_filtering.cxx)
if(EMSCRIPTEN)
  include(ITKBridgeJavaScript)
  web_add_executable(HelloWorld HelloWorld.cxx)

else()
  add_executable(HelloWorld HelloWorld.cxx)
endif()

target_link_libraries(HelloWorld ${ITK_LIBRARIES})