#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>
#include <numeric>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::ThrowOnInvalidPosition(Position pos) const {
    if(!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }
}

Cell& Sheet::CreateCellIfNotCreated(Position pos) {
	ThrowOnInvalidPosition(pos);
    if(cells_.size() <= static_cast<size_t>(pos.row)) {
        cells_.resize(pos.row + 1);
        cells_[pos.row].resize(pos.col + 1);
    } else if(cells_[pos.row].size() <= static_cast<size_t>(pos.col)) {
        cells_[pos.row].resize(pos.col + 1);
    }
    auto& cell = cells_[pos.row][pos.col];
    if(!cell) {
        cell = std::make_unique<Cell>(*this);
    }
	return *cell;
}

void Sheet::SetCell(Position pos, std::string text) {
    Cell& cell = CreateCellIfNotCreated(pos);
    cell.Set(std::move(text), pos);
    
    size_.rows = std::max(size_.rows, pos.row + 1);
    size_.cols = std::max(size_.cols, pos.col + 1);
}

const Cell* Sheet::GetCell(Position pos) const {
    ThrowOnInvalidPosition(pos);
    if(cells_.size() <= static_cast<size_t>(pos.row)) {
        return nullptr;
    }
    if(cells_[pos.row].size() <= static_cast<size_t>(pos.col)) {
        return nullptr;
    }
    return cells_[pos.row][pos.col].get();
}
Cell* Sheet::GetCell(Position pos) {
    const Cell* p = static_cast<const Sheet*>(this)->GetCell(pos);
    return const_cast<Cell*>(p);
}

size_t Sheet::GetNullptrCountInEnd(size_t row) {
    size_t null_count = 0;
    auto end = cells_[row].rend();
    for(auto it = cells_[row].rbegin(); it != end; ++it) {
        if(*it) {
            break;
        }
        ++null_count;
    }
    return null_count;
}

size_t Sheet::GetEmptyRowsCountInEnd() {
    size_t zero_count = 0;
    for(auto it = cells_.rbegin(); it != cells_.rend(); ++it) {
        if(it->size()) {
            break;
        }
        ++zero_count;
    }
    return zero_count;
}

void Sheet::RecalculateSize() {
    size_.rows = cells_.size();
    size_.cols = 
        std::transform_reduce( cells_.begin(), cells_.end(),
                    static_cast<size_t>(0),
                    [](size_t lhs, size_t rhs) {return std::max(lhs, rhs);},
                    [](auto& row){ return row.size();} 
        );
}

void Sheet::ClearCell(Position pos) {
    ThrowOnInvalidPosition(pos);
    if(GetCell(pos) == nullptr) {
        return;
    }
    auto& cell = cells_[pos.row][pos.col];
    cell = nullptr;
    if(&cell != &cells_[pos.row].back()) {
        return;
    }
    //уберем размер с конца этой строки
    size_t del_count = GetNullptrCountInEnd(pos.row);
    size_t count = cells_[pos.row].size() - del_count;
    bool recalculate = static_cast<size_t>(size_.cols) == cells_[pos.row].size();
    cells_[pos.row].resize(count);
    if(count) {
        if(recalculate) {
            RecalculateSize();
        }
        return;
    }
    size_t del_rows = GetEmptyRowsCountInEnd();
    count = cells_.size() - del_rows;
    recalculate = recalculate || (static_cast<size_t>(size_.rows) == cells_.size());
    cells_.resize(count);
    if(recalculate) {
        RecalculateSize();
    }
    return;
}

Size Sheet::GetPrintableSize() const {
    return size_;
}

template <typename GetFunc>
void Sheet::PrintRow(std::ostream& output, size_t row_index, GetFunc getter) const {
	auto& row = cells_[row_index];
    size_t cols = size_.cols;
    size_t i = 1;
    if(row.size() != 0 && row[0]) {
        output << getter(row[0]);
    }
    for(; i < row.size(); ++i) {
        output << "\t";
        if(row[i]) {
            output << getter(row[i]);
        }
    }
    
    for(; i < cols; ++i) {
        output << "\t";
    }
}

template <typename GetFunc>
void Sheet::PrintCells(std::ostream& output, GetFunc getter) const {
    for(size_t row = 0; row < cells_.size(); ++row) {
        PrintRow(output, row, getter);
        output << "\n";
    }
}

void Sheet::PrintValues(std::ostream& output) const {
	PrintCells(output, [](auto& cell) {
			return cell->GetValue();
		}
	);
}
void Sheet::PrintTexts(std::ostream& output) const {
	PrintCells(output, [](auto& cell) {
			return cell->GetText();
		}
	);
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}