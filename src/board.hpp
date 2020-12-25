#pragma once
#include <algorithm>
#include <array>
#include <bitset>
#include <iostream>
#include <vector>

class Board;
class Range;

class Cell {
 public:
    bool remove(size_t value);
    bool certain() const;
    size_t value() const;

    void value(size_t val);

    bool operator == (const Cell& other) const;

 private:
    std::bitset<9> bits_;
};

struct Format {
    std::string emptySign = ".";
    std::string cellDelimiter = "";
    std::string blockDelimiter = "";
    std::string rowDelimiter = "";
    std::string end = "";
};

class Board {
 public:
    Board();

    template <typename DataSource>
    Board(DataSource src) : Board()
    {
        static_assert(std::is_integral_v<decltype(*std::begin(src))>);
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

    static Board fromString(const std::string& str);

    Cell& cell(size_t row, size_t col) { return cells_[row * 9 + col]; }
    const Cell& cell(size_t row, size_t col) const { return cells_[row * 9 + col]; }

    bool solved() const;

    size_t filledCount() const;

    bool valid() const;

    std::string toString(const Format& f=Format{}) const;

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
