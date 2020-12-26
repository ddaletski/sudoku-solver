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

    bool adjustRange(size_t row, size_t col, Board::Range rng) {
        bool changed = false;

        size_t value = board_.cell(row, col).value();
        if(value == 0) {
            return changed;
        }

        for(auto entry: rng) {
            auto pos = entry.pos;
            if (pos.y == row && pos.x == col) {
                continue;
            }

            auto &otherCell = entry.cell;
            auto otherVal = otherCell.value();
            if(otherVal != 0) {
                continue;
            }
            if(otherVal == value) {
                throw std::runtime_error("CAN'T BE!");
            }
            changed |= otherCell.remove(value);
        }

        return changed;
    }

    bool adjustRow(size_t row, size_t col) {
        return adjustRange(row, col, board_.range(row, 0, 1, 9));
    }

    bool adjustCol(size_t row, size_t col) {
        return adjustRange(row, col, board_.range(0, col, 9, 1));
    }

    bool adjustBlock(size_t row, size_t col) {
        size_t blockStartRow = row / 3 * 3;
        size_t blockStartCol = col / 3 * 3;
        return adjustRange(row, col, board_.range(blockStartRow, blockStartCol, 3, 3));
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
                }
            }
        }

        return board_.solved();
    }


 private:
    Board& board_;
};
