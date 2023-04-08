#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <deque>
#include <memory>

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

	// Можете дополнить ваш класс нужными полями и методами


private:
	// Можете дополнить ваш класс нужными полями и методами
    std::deque<std::deque<std::unique_ptr<Cell>>> cells_;//[row][col]
    Size size_;
    
    size_t GetNullptrCountInEnd(size_t row);
    size_t GetEmptyRowsCountInEnd();
    
    void RecalculateSize();
    
    void PrintValues(std::ostream& output, size_t row) const;
    void PrintTexts(std::ostream& output, size_t row) const;
    
    void ThrowOnInvalidPosition(Position pos) const;
};