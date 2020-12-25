#include <bitset>
#include <iostream>
#include <array>

struct Cell {
    std::bitset<9> bits;

    void remove(size_t value) {
        bits[value] = false;
    }
    size_t value() {
        if(bits.count() == 1) {
            for(size_t i = 0; i < 9; ++i) {
                if(bits[i]) {
                    return i;
                }
            }
        }
        return 0;
    }

    void value(size_t val) {
        bits.reset();
        bits.set(val, true);
    }
};

class Board {
 public:
    Board() {
        for(auto& cell: cells_) {
            cell.bits.set();
        }
    }

    Cell& cell(size_t row, size_t col) { return cells_[row * 9 + col]; }

    bool solved() {
        for(auto& cell: cells_) {
            if(cell.value() == 0) {
                return false;
            }
        }
        return true;
    }
 private:
    std::array<Cell, 9*9> cells_;
};