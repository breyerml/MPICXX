/**
 * @file default_construction.cpp
 * @author Marcel Breyer
 * @date 2019-12-15
 *
 * @brief Test cases for the @ref mpicxx::info implementation.
 *
 * This file provides test cases for the default constructor of the mpicxx::info class.
 */

#include <gtest/gtest.h>
#include <mpi.h>

#include <mpicxx/info/info.hpp>


TEST(DefaultConstructionTest, DefaultConstruction) {
    // default construct an info object
    mpicxx::info info;

    // a default constructed info object should not be in the "moved-from" state
    ASSERT_NE(info.get(), MPI_INFO_NULL);

    // a default constructed info object should be emtpy
    int nkeys;
    MPI_Info_get_nkeys(info.get(), &nkeys);
    EXPECT_EQ(nkeys, 0);

    // a default constructed info object is always freeable
    EXPECT_TRUE(info.freeable());
}