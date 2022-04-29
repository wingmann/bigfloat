set(TARGET lib_bigfloat)

file(GLOB PROJECT_SOURCES *.cpp)

add_library(${TARGET} ${PROJECT_SOURCES})
