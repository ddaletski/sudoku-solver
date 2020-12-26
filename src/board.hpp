#pragma once
#include <algorithm>
#include <array>
#include <bitset>
#include <iostream>
#include <vector>
#include <functional>
#include <set>

class Board;

struct Point {
    size_t y;
    size_t x;

    friend std::ostream& operator << (std::ostream& str, const Point& pt) {
        str << "(y: " << pt.y << ", x: " << pt.x << ")";
        return str;
    }
};

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

    template<typename CellType>
    class _Range {
        friend class Board;

     public:
        template<typename RngType, typename ItCellType>
        class RangeIterator {
            friend class _Range;

        private:
            RangeIterator(RngType* rng, size_t pos)
                : rng_(rng)
                , pos_(pos)
            {};

        public:
            struct RangeEntry {
                Point pos;
                Point blockStart;
                ItCellType& cell;
            };

            RangeIterator& operator++() {
                if(pos_ <= rng_->size()) {
                    ++pos_;
                }
                return *this;
            }
            RangeIterator operator++(int) {
                RangeIterator retval = *this;
                ++(*this);
                return retval;
            }
            bool operator==(const RangeIterator& other) const {
                return rng_ == other.rng_ && pos_ == other.pos_;
            }
            bool operator!=(const RangeIterator& other) const {
                return !(*this == other);
            }
            RangeEntry operator*() {
                auto& cell = *rng_->cells_.at(pos_);
                size_t y = rng_->y() + (pos_ / rng_->width());
                size_t x = rng_->x() + (pos_ % rng_->width());
                size_t blockY = y / 3 * 3;
                size_t blockX = x / 3 * 3;

                return RangeEntry {
                    .pos = Point{.y = y, .x = x},
                            .blockStart = Point{.y = blockY, .x = blockX},
                            .cell = cell
                };
            }

            RngType* rng_;
            size_t pos_;
        };

    public:
        std::vector<size_t> values() const
        {
            std::vector<size_t> vals(cells_.size(), 0);
            std::transform(std::begin(cells_), std::end(cells_), std::begin(vals),
                           [](auto& pCell) { return pCell->value(); });

            return vals;
        }

        CellType& cell(size_t idx) { return *cells_.at(idx); }
        const CellType& cell(size_t idx) const { return *cells_.at(idx); }

        bool unique() const
        {
            std::set<size_t> metValues;

            for (auto& cell : cells_) {
                if(!cell->certain()) {
                    continue;
                }
                auto val = cell->value();
                if (metValues.count(val)) {
                    return false;
                }
                metValues.insert(val);
            }

            return true;
        }

        size_t x() const { return x_; }
        size_t y() const { return y_; }
        size_t width() const { return w_; }
        size_t height() const { return h_; }
        size_t size() const { return w_ * h_; }

        RangeIterator<_Range, Cell> begin()
        {
            return RangeIterator<_Range, Cell>(this, 0);
        }
        RangeIterator<_Range, Cell> end()
        {
            return RangeIterator<_Range, Cell>(this, size());
        }

        RangeIterator<const _Range, const Cell> begin() const
        {
            return RangeIterator<const _Range, const Cell>(this, 0);
        }
        RangeIterator<const _Range, const Cell> end() const
        {
            return RangeIterator<const _Range, const Cell>(this, size());
        }

    private:
        _Range(const std::array<CellType*, 9*9>& cells, size_t y, size_t x, size_t height, size_t width)
            : x_(x), y_(y), w_(width), h_(height)
        {
            cells_.reserve(w_ * h_);
            for (size_t i = y_; i < y + h_; ++i) {
                for (size_t j = x_; j < x_ + w_; ++j) {
                    cells_.push_back(cells[i * 9 + j]);
                }
            }
        }

        size_t x_;
        size_t y_;
        size_t w_;
        size_t h_;
        std::vector<CellType*> cells_;
    };


    using Range = _Range<Cell>;
    using ConstRange = _Range<const Cell>;

    Range range(size_t y, size_t x, size_t height, size_t width);
    ConstRange range(size_t y, size_t x, size_t height, size_t width) const;


 private:
    std::array<Cell, 9 * 9> cells_;
};
