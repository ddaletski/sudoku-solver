#include "board.hpp"
#include <cassert>
#include <sstream>
#include <stdexcept>

bool Cell::remove(size_t value)
{
    bool wasTrue = bits_[value - 1];
    bits_[value - 1] = false;
    return wasTrue;
}

bool Cell::certain() const
{
    return bits_.count() == 1;
}

size_t Cell::value() const
{
    if (bits_.count() == 1) {
        for (size_t i = 0; i < 9; ++i) {
            if (bits_[i]) {
                return i + 1;
            }
        }
    }
    return 0;
}

void Cell::value(size_t val)
{
    bits_.reset();
    if (val != 0) {
        bits_.set(val - 1, true);
    } else {
        bits_.set();
    }
}

bool Cell::operator ==(const Cell &other) const
{
    return bits_ == other.bits_;
}

Board::Board()
{
    for (auto& cell : cells_) {
        cell.value(0);
    }
}

Board Board::trivial()
{
    auto board = Board::empty();
    for (size_t i = 0; i < 9; ++i) {
        for (size_t j = 0; j < 9; ++j) {
            board.cell(i, j).value((i * 3 + (i / 3) + j) % 9 + 1);
        }
    }

    return board;
}

Board Board::fromString(const std::string& str)
{
    Board board;

    std::istringstream ss(str);
    assert(str.size() >= 81);
    char chr;
    for(int i = 0; i < 81; ++i) {
        ss >> chr;
        if(chr >= '0' && chr <= '9') {
            board.cell(i / 9, i % 9).value(size_t(chr - '0'));
        } else {
            board.cell(i / 9, i % 9).value(0);
        }
    }

    return board;
}

Board::Range Board::range(size_t y, size_t x, size_t height, size_t width)
{
    std::array<Cell*, 81> arr;
    for(int i = 0; i < 81; ++i) {
        arr[i] = &cells_[i];
    }
    return Range(arr, y, x, height, width);
}

Board::ConstRange Board::range(size_t y, size_t x, size_t height, size_t width) const
{
    std::array<const Cell*, 81> arr;
    for(int i = 0; i < 81; ++i) {
        arr[i] = &cells_[i];
    }
    return ConstRange(arr, y, x, height, width);
}

bool Board::solved() const
{
    for (auto& cell : cells_) {
        if (cell.value() == 0) {
            return false;
        }
    }
    return true;
}


size_t Board::filledCount() const
{
    return std::count_if(cells_.begin(), cells_.end(), std::mem_fn(&Cell::certain));
}

bool Board::valid() const
{
    for (int i = 0; i < 9; ++i) {
        auto rowRng = range(i, 0, 1, 9);
        if(!rowRng.unique()) {
            return false;
        }
    }

    for (int i = 0; i < 9; ++i) {
        auto colRng = range(0, i, 9, 1);
        if(!colRng.unique()) {
            return false;
        }
    }

    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            auto blockRng = range(i*3, j*3, 3, 3);
            if (!blockRng.unique()) {
                return false;
            }
        }
    }

    return true;
}

std::string Board::toString(const Format &f) const
{
    std::ostringstream ss;
    for(int i = 0; i < 9; ++i) {
        for(int j = 0; j < 9; ++j) {
            auto val = cell(i, j).value();
            if(val == 0) {
                ss << f.emptySign;
            } else {
                ss << val;
            }
            if(j < 8) {
                if(j % 3 == 2) {
                    ss << f.blockDelimiter;
                } else {
                    ss << f.cellDelimiter;
                }
            }
        }
        if(i < 8) {
            ss << f.rowDelimiter;
        } else {
            ss << f.end;
        }
    }


    return ss.str();
}

std::ostream& operator<<(std::ostream& stream, const Board& board)
{
    Format format;
    format.rowDelimiter = "\n";
    format.cellDelimiter = " ";
    format.blockDelimiter = " ";
    format.end = "\n";
    stream << board.toString(format);

    return stream;
}
