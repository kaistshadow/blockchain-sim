//
// Created by Yonggon Kim on 25/01/2021.
//
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "utility/ArgsManager.h"

TEST_CASE("example1") {
    REQUIRE( 4*5 == 20 );
}



TEST_CASE("example2-BLEEP") {
    using namespace libBLEEP;

//    REQUIRE(gArgs.GetArg("test") == "");
    REQUIRE("" == "");
    REQUIRE(gArgs.GetArg("-id") == "noid");
    // gArgs.GetArg("test");
}