/**
 * @file swap.cpp
 * @author Marcel Breyer
 * @date 2019-12-16
 *
 * @brief Test cases for the @ref mpicxx::info implementation.
 *
 * This file provides test cases for the `swap` non-member function of the mpicxx::info class.
 */

#include <gtest/gtest.h>
#include <mpi.h>

#include <mpicxx/info/info.hpp>


TEST(NonMemberFunctionTest, SwapValidAndValid) {
    // create two info objects and add [key, value]-pairs
    mpicxx::info info_1;
    MPI_Info_set(info_1.get(), "key1", "value1");
    mpicxx::info info_2;
    MPI_Info_set(info_2.get(), "key2", "value2");
    MPI_Info_set(info_2.get(), "key3", "value3");

    // swap both info objects
    std::swap(info_1, info_2);

    int nkeys, flag;
    char value[MPI_MAX_INFO_VAL];
    // check info_1 object
    MPI_Info_get_nkeys(info_1.get(), &nkeys);
    EXPECT_EQ(nkeys, 2);
    MPI_Info_get(info_1.get(), "key2", 6, value, &flag);
    EXPECT_TRUE(static_cast<bool>(flag));
    EXPECT_STREQ(value, "value2");
    MPI_Info_get(info_1.get(), "key3", 6, value, &flag);
    EXPECT_TRUE(static_cast<bool>(flag));
    EXPECT_STREQ(value, "value3");

    // check info_2 object
    MPI_Info_get_nkeys(info_2.get(), &nkeys);
    EXPECT_EQ(nkeys, 1);
    MPI_Info_get(info_2.get(), "key1", 6, value, &flag);
    EXPECT_TRUE(static_cast<bool>(flag));
    EXPECT_STREQ(value, "value1");
}

TEST(NonMemberFunctionTest, SwapValidAndMovedFrom) {
    // create two info objects and add [key, value]-pairs to one and set the other to the "moved-from" state
    mpicxx::info info_1;
    MPI_Info_set(info_1.get(), "key", "value");
    mpicxx::info info_2(std::move(info_1));

    // swap both info objects
    std::swap(info_1, info_2);

    int nkeys, flag;
    char value[MPI_MAX_INFO_VAL];

    // check info_2 object -> now in the "moved-from" state
    EXPECT_EQ(info_2.get(), MPI_INFO_NULL);
    EXPECT_FALSE(info_2.freeable());

    // check info_1 object
    MPI_Info_get_nkeys(info_1.get(), &nkeys);
    EXPECT_EQ(nkeys, 1);
    MPI_Info_get(info_1.get(), "key", 6, value, &flag);
    EXPECT_TRUE(static_cast<bool>(flag));
    EXPECT_STREQ(value, "value");
    EXPECT_TRUE(info_1.freeable());

    // swap both info objects back
    std::swap(info_1, info_2);

    // check info_1 object -> now in the "moved-from" state
    EXPECT_EQ(info_1.get(), MPI_INFO_NULL);
    EXPECT_FALSE(info_1.freeable());

    // check info_2 object
    MPI_Info_get_nkeys(info_2.get(), &nkeys);
    EXPECT_EQ(nkeys, 1);
    MPI_Info_get(info_2.get(), "key", 6, value, &flag);
    EXPECT_TRUE(static_cast<bool>(flag));
    EXPECT_STREQ(value, "value");
    EXPECT_TRUE(info_2.freeable());
}

TEST(NonMemberFunctionTest, SwapMovedFromAndMovedFrom) {
    // create two empty info objects and set them to the "moved-from" state
    mpicxx::info info_1;
    mpicxx::info dummy_1(std::move(info_1));
    mpicxx::info info_2;
    mpicxx::info dummy_2(std::move(info_2));

    // swap both "moved-from" info objects
    std::swap(info_1, info_2);

    // both are still in the "moved-from" state
    EXPECT_EQ(info_1.get(), MPI_INFO_NULL);
    EXPECT_FALSE(info_1.freeable());
    EXPECT_EQ(info_2.get(), MPI_INFO_NULL);
    EXPECT_FALSE(info_2.freeable());
}