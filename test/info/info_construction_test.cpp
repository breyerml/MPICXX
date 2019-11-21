/**
 * @file info_construction_test.cpp
 * @author Marcel Breyer
 * @date 2019-11-20
 *
 * @brief Test cases for the @ref mpicxx::info implementation.
 *
 * This file provides test cases for the constructoion of a mpicxx::info object.
 */

#include <gtest/gtest.h>
#include <mpi.h>

#include <mpicxx/info/info.hpp>

TEST(InfoTests, DefaultConstruction) {

    // default construct a mpicxx::info object
    mpicxx::info info;

    int nkeys;
    MPI_Info_get_nkeys(info.get(), &nkeys);

    // a newly created MPI_Info object should be emtpy
    EXPECT_EQ(nkeys, 0);

}

TEST(InfoTests, CopyConstruction) {

    // default construct a mpicxx::info object
    mpicxx::info info;

    // add an element to the info object
    MPI_Info_set(info.get(), "key", "value");

    // be sure the key was successfully added
    int nkeys;
    MPI_Info_get_nkeys(info.get(), &nkeys);
    EXPECT_EQ(nkeys, 1);

    // create an new info object from a copy
    mpicxx::info info_copy(info);

    // be sure that the copied key and value are present
    int flag_copy;
    char value_copy[6];
    MPI_Info_get(info_copy.get(), "key", 5, value_copy, &flag_copy);
    // check if the key exists
    EXPECT_TRUE(static_cast<bool>(flag_copy));
    // check if the value associated with the key is correct
    EXPECT_STREQ(value_copy, "value");

    // add an element to the copied info object
    MPI_Info_set(info_copy.get(), "key2", "value2");

    // be sure the key was successfully added
    int nkeys_copy;
    MPI_Info_get_nkeys(info_copy.get(), &nkeys_copy);
    EXPECT_EQ(nkeys_copy, 2);

    // be sure the copied from object wasn't changed
    MPI_Info_get_nkeys(info.get(), &nkeys);
    EXPECT_EQ(nkeys, 1);

}

TEST(InfoTests, MoveConstruction) {

    // default construct a mpicxx::info object
    mpicxx::info info;

    // add an element to the info object
    MPI_Info_set(info.get(), "key", "value");

    // be sure the key was successfully added
    int nkeys;
    MPI_Info_get_nkeys(info.get(), &nkeys);
    EXPECT_EQ(nkeys, 1);

    // create an new info object from a std::move
    mpicxx::info info_move(std::move(info));

    // be sure that the moved key and value are present
    int flag_move;
    char value_move[6];
    MPI_Info_get(info_move.get(), "key", 5, value_move, &flag_move);
    // check if the key exists
    EXPECT_TRUE(static_cast<bool>(flag_move));
    // check if the value associated with the key is correct
    EXPECT_STREQ(value_move, "value");

    // add an element to the info object
    MPI_Info_set(info_move.get(), "key2", "value2");

    // be sure the key was successfully added
    int nkeys_move;
    MPI_Info_get_nkeys(info_move.get(), &nkeys_move);
    EXPECT_EQ(nkeys_move, 2);

    // be sure the moved from object has released it's state
    EXPECT_EQ(info.get(), nullptr);

}