# specify all source files for this test suite
set(TEST_SOURCES
        info_construction_test.cpp
        info_assignment_test.cpp
        info_capacity_test.cpp
        info_access_test.cpp
)

# create google test with MPI support
add_mpi_test(info "${TEST_SOURCES}" 1)