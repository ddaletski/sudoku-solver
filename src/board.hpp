#include <algorithm>
#include <array>
#include <bitset>
#include <iostream>
#include <vector>

class Board;
class Range;

class Cell {
 public:
    void remove(size_t value) { bits_[value] = false; }
    size_t value() const;

    void value(size_t val);

 private:
    std::bitset<9> bits_;
};

class Board {
 public:
    Board();

    template <typename DataSource>
    Board(DataSource src) : Board()
    {
        size_t idx = 0;
        for (auto& val : src) {
            assert(val >= 0 && val <= 9);
            cells_[idx].value(val);

            if (idx == 81) {
                break;
            }
            ++idx;
        }
    }

    static Board empty() { return Board(); }

    static Board trivial();

    Cell& cell(size_t row, size_t col) { return cells_[row * 9 + col]; }
    const Cell& cell(size_t row, size_t col) const { return cells_[row * 9 + col]; }

    bool solved() const;

    bool valid() const;

    friend std::ostream& operator<<(std::ostream& stream, const Board& board);

 private:
    std::array<Cell, 9 * 9> cells_;
};

class Range {
    friend class Board;

 public:
    std::vector<size_t> values() const;
    bool unique() const;

 private:
    Range(const Board& board, size_t y, size_t height, size_t x, size_t width);

    size_t x_;
    size_t y_;
    size_t w_;
    size_t h_;
    std::vector<const Cell*> cells_;
};
