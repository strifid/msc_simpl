project(msc_simpl)

cmake_minimum_required(VERSION 2.6)
cmake_policy(SET CMP0015 OLD)

add_definitions(-D_LINUX)
if(QUAD)
	add_definitions(-DQUAD)
endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wno-deprecated")

if(NOT MTLIB_PATH)
    set(MTLIB_PATH "/prjcts/mtlib/install")
endif()

if(NOT OPENCV_PATH)
    set(OPENCV_PATH "/prjcts/opencv/install")
endif()

if(NOT CCFITS_PATH)
    set(CCFITS_PATH "/prjcts/CCfits/install")
endif()

if(NOT CFITSIO_PATH)
    set(CFITSIO_PATH "/prjcts/cfitsio/install")
endif()


include_directories(./ ${CFITSIO_PATH}/include ${CCFITS_PATH}/include ${OPENCV_PATH}/include ${MTLIB_PATH}/include /usr/include/libxml2 /usr/include/paraview /usr/include/vtk-5.8 ./src)
link_directories(${CFITSIO_PATH}/lib ${CCFITS_PATH}/lib ${MTLIB_PATH}/lib ${OPENCV_PATH}/lib /usr/lib/paraview/ )

aux_source_directory(src/ BASE_SOURCE)
aux_source_directory(src/persist_pair PPAIR_SOURCE)
aux_source_directory(src/main MAIN_SOURCE)

aux_source_directory(src/tests TESTS_SOURCE)


set(LIBS pthread xml2 mt m opencv_core opencv_highgui  vtkHybrid vtkIO vtkFiltering vtkCommon vtkGraphics vtkImaging vtkRendering CCfits) 

add_executable (gradient_msc ${BASE_SOURCE} ${MAIN_SOURCE} ${PPAIR_SOURCE})
target_link_libraries(gradient_msc ${LIBS})

add_executable (create_test src/utils/generate_image.cpp)
target_link_libraries(create_test ${LIBS})

# Compile tests

add_executable (tests ${BASE_SOURCE} ${TESTS_SOURCE} ${PPAIR_SOURCE})
target_link_libraries(tests ${LIBS})

