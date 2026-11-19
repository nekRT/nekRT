# src/CMakeLists.txt
#
# Third-party packages are added in cmake/Dependencies.cmake.

add_library(
	nekrt_lib
	quadrature/trapezoid.cc
	quadrature/gauss_lobatto_legendre.cc
	quadrature/gauss_legendre.cc
	quadrature/quadrature.cc
	utils/errors.cc
	utils/input_parameters.cc
	utils/legendre_polynomials.cc
	utils/output.cc
)

target_compile_features(nekrt_lib PUBLIC cxx_std_20)
target_include_directories(
	nekrt_lib
	PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}/include
)

# Build information printed in the output header. The git hash is captured at
# configure time, so it goes stale until CMake is re-run.
execute_process(
	COMMAND git rev-parse HEAD
	WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
	OUTPUT_VARIABLE NEKRT_GIT_SHA
	OUTPUT_STRIP_TRAILING_WHITESPACE
	ERROR_QUIET
)
if(NOT NEKRT_GIT_SHA)
	set(NEKRT_GIT_SHA "unknown")
endif()
configure_file(
	${PROJECT_SOURCE_DIR}/cmake/build_info.h.in
	${PROJECT_BINARY_DIR}/include/utils/build_info.h
	@ONLY
)

# The dependencies are PUBLIC so that the executable and the tests get them
# through nekrt_lib.
# NOTE(parallel): MPI::MPI_CXX goes here once MFEM is built with MPI.
target_link_libraries(nekrt_lib PUBLIC fmt::fmt ryml::ryml mfem)

add_executable(nekrt main.cc)
target_link_libraries(nekrt PRIVATE nekrt_lib)
