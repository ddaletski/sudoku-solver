#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <iostream>
#include "board.hpp"


TEST_CASE( "catch2 works", "[dummy]" ) {
    REQUIRE( 1 == 1 );
}


TEST_CASE("board tests", "[board]") {
    Board b;

    SECTION("can't solve empty board") {
        std::cerr << b << std::endl;
        REQUIRE(!b.solved());
    }

    SECTION("can solve solved board") {
        auto solvedBoard = Board::trivial();
        std::cerr << solvedBoard << std::endl;
        REQUIRE(solvedBoard.solved());
    }

    SECTION("trivial board is valid") {
        auto solvedBoard = Board::trivial();
        REQUIRE(solvedBoard.valid());
    }
}