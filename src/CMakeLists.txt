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
)

target_compile_features(nekrt_lib PUBLIC cxx_std_20)
target_include_directories(nekrt_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)

# The dependencies are PUBLIC so that the executable and the tests get them
# through nekrt_lib.
# NOTE(parallel): MPI::MPI_CXX goes here once MFEM is built with MPI.
target_link_libraries(nekrt_lib PUBLIC fmt::fmt ryml::ryml mfem)

add_executable(nekrt main.cc)
target_link_libraries(nekrt PRIVATE nekrt_lib)
