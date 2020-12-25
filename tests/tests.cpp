#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "board.hpp"


TEST_CASE( "catch2 works", "[dummy]" ) {
    REQUIRE( 1 == 1 );
}


TEST_CASE("board tests", "[board]") {
    Board b;
    REQUIRE(b.solved() == false);
}