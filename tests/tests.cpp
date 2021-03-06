#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <iostream>
#include "board.hpp"
#include "solver/solver.hpp"
#include <filesystem>
#include <fstream>

namespace std {
    namespace fs = filesystem;
}

struct TestSample {
    std::string puzzle;
    std::string solution;
};

std::vector<TestSample> loadTestSamples(const std::fs::path& csvPath) {
    std::vector<TestSample> samples;

    std::ifstream ifs(csvPath);
    if(!ifs.is_open()) {
        throw std::runtime_error("can't open sample file " + csvPath.string());
    }
    std::string buffer(81, '.');
    while(!ifs.eof()) {
        auto sample = TestSample{};
        ifs.read(&buffer[0], 81);
        if(ifs.eof()) {
            break;
        }
        sample.puzzle = buffer;
        ifs.ignore();
        ifs.read(&buffer[0], 81);
        if(ifs.eof()) {
            break;
        }
        sample.solution = buffer;
        samples.push_back(sample);
        ifs.ignore();
    }

    return samples;
}

TEST_CASE( "catch2 works", "[dummy]" ) {
    REQUIRE( 1 == 1 );
}


void fillRange(Board::Range& rng, std::string& afterExpected) {
    for(auto entry: rng) {
        auto newVal = rand() % 9 + 1;
        entry.cell.value(newVal);
        afterExpected[entry.pos.y * 9 + entry.pos.x] = newVal + '0';
    }
}

TEST_CASE("range tests") {
    Board b = Board::trivial();

    SECTION("row range iterates where needed") {
        for(size_t i = 0; i < 9; ++i) {
            auto rng = b.range(i, 0, 1, 9);
            for(auto entry : rng) {
                auto& cell = entry.cell;
                auto trueCell = b.cell(entry.pos.y, entry.pos.x);
                REQUIRE(cell.value() == trueCell.value());
            }
        }
    }

    SECTION("col range iterates where needed") {
        for(size_t i = 0; i < 9; ++i) {
            auto rng = b.range(0, i, 9, 1);
            for(auto entry : rng) {
                auto& cell = entry.cell;
                auto trueCell = b.cell(entry.pos.y, entry.pos.x);
                REQUIRE(cell.value() == trueCell.value());
            }
        }
    }

    SECTION("block range iterates where needed") {
        for(size_t i = 0; i < 3; ++i) {
            for(size_t j = 0; j < 3; ++j) {
                auto rng = b.range(i * 3, j * 3, 3, 3);
                for(auto entry : rng) {
                    auto& cell = entry.cell;
                    auto trueCell = b.cell(entry.pos.y, entry.pos.x);
                    REQUIRE(cell.value() == trueCell.value());
                }
            }
        }
    }

    SECTION("row range iterator mutates appropriate values") {
        std::string afterExpected = std::string(81, '.');
        for(size_t i = 0; i < 9; ++i) {
            auto rng = b.range(i, 0, 1, 9);
            fillRange(rng, afterExpected);
        }
        REQUIRE(afterExpected == b.toString());
    }

    SECTION("col range iterator mutates appropriate values") {
        std::string afterExpected = std::string(81, '.');
        for(size_t i = 0; i < 9; ++i) {
            auto rng = b.range(0, i, 9, 1);
            fillRange(rng, afterExpected);
        }
        REQUIRE(afterExpected == b.toString());
    }

    SECTION("block range iterator mutates appropriate values") {
        std::string afterExpected = std::string(81, '.');
        for(size_t i = 0; i < 3; ++i) {
            for(size_t j = 0; j < 3; ++j) {
                auto rng = b.range(i * 3, j * 3, 3, 3);
                fillRange(rng, afterExpected);
            }
        }
        REQUIRE(afterExpected == b.toString());
    }
}


TEST_CASE("board tests", "[board]") {
    SECTION("empty board isn't solved") {
        Board b;
        REQUIRE(!b.solved());
    }

    SECTION("can solve solved board") {
        auto solvedBoard = Board::trivial();
        REQUIRE(solvedBoard.solved());
    }

    SECTION("trivial board is valid") {
        auto solvedBoard = Board::trivial();
        REQUIRE(solvedBoard.valid());
    }

    SECTION("real puzzles test") {
        auto testSamples = loadTestSamples(std::fs::path("./testdata/easy_30.csv"));
        REQUIRE(!testSamples.empty());

        SECTION("parsed board has the same count of filled cells") {
            for(auto& sample: testSamples) {
                auto b = Board::fromString(sample.puzzle);
                size_t reallyFilled = std::count_if(sample.puzzle.begin(),
                                                    sample.puzzle.end(),
                                                    [](auto& ch) { return (ch >= '1' && ch <= '9'); });
                REQUIRE(b.filledCount() == reallyFilled);
            }
        }


        SECTION("load+dump returns the same string") {
            Format f;
            f.emptySign = "0";
            for(auto& sample: testSamples) {
                auto b = Board::fromString(sample.puzzle);
                REQUIRE(b.toString(f) == sample.puzzle);
            }
        }

        SECTION("solution trial returns valid board") {
            for(auto& sample : testSamples) {
                auto b = Board::fromString(sample.puzzle);
                auto solver = Solver(b);
                solver.solve();
                REQUIRE(b.valid());
            }
        }

        SECTION("solution trial doesn't change filled cells") {
            for(auto& sample : testSamples) {
                auto b = Board::fromString(sample.puzzle);
                auto before = b;
                auto solver = Solver(b);
                solver.solve();

                for(size_t i = 0; i < 9; ++i) {
                    for(size_t j = 0; j < 9; ++j) {
                        auto& cellBeforeSolution = before.cell(i, j);
                        auto& cellAfterSolution = before.cell(i, j);
                        if(cellBeforeSolution.certain()) {
                            REQUIRE(cellBeforeSolution == cellAfterSolution);
                        }
                    }
                }
            }
        }

        SECTION("solution trial doesn't make worse") {
            for(auto& sample : testSamples) {
                auto b = Board::fromString(sample.puzzle);
                size_t filledBefore = b.filledCount();
                auto solver = Solver(b);
                solver.solve();
                REQUIRE(b.filledCount() >= filledBefore);
            }
        }

        SECTION("can solve all puzzles") {
            for(auto& sample: testSamples) {
                auto board = Board::fromString(sample.puzzle);
                auto solver = Solver(board);
                solver.solve();
                REQUIRE(board.solved());
            }
        }
    }
}
