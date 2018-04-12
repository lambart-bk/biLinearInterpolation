
find_package(OpenCV  REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} )


add_executable(bilinearinterpolation bilinearinterpolation.cpp)
target_link_libraries(bilinearinterpolation ${OpenCV_LIBRARIES} )


