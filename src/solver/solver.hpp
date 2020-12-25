#pragma once
#include "../board.hpp"
#include <set>
#include <bitset>


class Solver {
 public:
    Solver(Board& board)
    : board_(board)
    {
    }

    bool lastInRange(size_t row, size_t col, size_t y, size_t height, size_t x, size_t width) {
        size_t value = board_.cell(row, col).value();
        if(value != 0) {
            return false;
        }

        std::bitset<9> foundValues;
        for (size_t i = y; i < y + height; ++i) {
            for (size_t j = x; j < x + width; ++j) {
                if (i == row && j == col) {
                    continue;
                }
                auto valInCell = board_.cell(i, j).value();
                if (valInCell == 0) {
                    return false;
                }
                foundValues[valInCell - 1] = true;
            }
        }

        size_t lastValue = 0;
        for(size_t idx = 0; idx < 9; ++idx) {
            if(!foundValues[idx]) {
                lastValue = idx + 1;
                break;
            }
        }

        assert(lastValue != 0);

        board_.cell(row, col).value(lastValue);

        return true;
    }

    bool lastInRow(size_t row, size_t col) {
        return lastInRange(row, col, row, 1, 0, 9);
    }

    bool lastInCol(size_t row, size_t col) {
        return lastInRange(row, col, 0, 9, col, 1);
    }

    bool lastInBlock(size_t row, size_t col) {
        size_t blockStartRow = row / 3 * 3;
        size_t blockStartCol = col / 3 * 3;
        return lastInRange(row, col, blockStartRow, 3, blockStartCol, 3);
    }

    bool adjustRange(size_t row, size_t col, size_t y, size_t height, size_t x, size_t width) {
        bool changed = false;

        size_t value = board_.cell(row, col).value();
        if(value == 0) {
            return changed;
        }

        for (size_t i = y; i < y + height; ++i) {
            for (size_t j = x; j < x + width; ++j) {
                if (i == row && j == col) {
                    continue;
                }
                auto &otherCell = board_.cell(i, j);
                auto otherVal = otherCell.value();
                if(otherVal != 0) {
                    continue;
                }
                if(otherVal == value) {
                    throw std::runtime_error("CAN'T BE!");
                }
                changed |= otherCell.remove(value);
            }
        }

        return changed;
    }

    bool adjustRow(size_t row, size_t col) {
        return adjustRange(row, col, row, 1, 0, 9);
    }

    bool adjustCol(size_t row, size_t col) {
        return adjustRange(row, col, 0, 9, col, 1);
    }

    bool adjustBlock(size_t row, size_t col) {
        size_t blockStartRow = row / 3 * 3;
        size_t blockStartCol = col / 3 * 3;
        return adjustRange(row, col, blockStartRow, 3, blockStartCol, 3);
    }


    bool solve() {
        bool somethingChanged = true;
        while(somethingChanged) {
            somethingChanged = false;

            for(size_t i = 0; i < 9; ++i) {
                for(size_t j = 0; j < 9; ++j) {
                    somethingChanged |= adjustRow(i, j);
                    somethingChanged |= adjustCol(i, j);
                    somethingChanged |= adjustBlock(i, j);
                    somethingChanged |= lastInRow(i, j);
                    somethingChanged |= lastInCol(i, j);
                    somethingChanged |= lastInBlock(i, j);
                }
            }
        }

        return board_.solved();
    }


 private:
    Board& board_;
};
