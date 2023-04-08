#pragma once

#include "common.h"
#include "formula.h"


#include <functional>
#include <unordered_set>
#include <optional>

class Sheet;

class Cell : public CellInterface {
public:
    Cell(Sheet& sheet);
    ~Cell();

    void Set(std::string text, Position position);
    void Clear(Position position);

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    //bool IsReferenced() const;

private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;
	class NumberImpl;

	Sheet& sheet_;
	std::unique_ptr<Impl> impl_;

	std::unordered_set<Position> this_depends_from{};//от каких ячеек зависит эта ячейка
	std::unordered_set<Position> other_depends_this{};//какие ячейки зависят от этой

	mutable std::optional<Value> cash_;

	void ResetCash();//рекурсивно сбросит кэш этой ячейке и всем other_depends_this

	void DependsFrom(Position position);
	void NotDependsFrom(Position position);

	void InsertCellDependsFromThis(Position position);
	void RemoveCellDependsFromThis(Position position);


	void InsertThisDepends(Position this_position, Position other_position);
	void InsertThisDepends(Position this_position, const std::unordered_set<Position>& other_positions);
	void RemoveThisDepends(Position this_position, Position other_position);
	void RemoveThisDepends(Position this_position);

	void CheckCircularReference(std::unordered_set<Position>& positions, std::unordered_set<Position>& already_check, Position throw_position);
	void CheckCircularReference(std::unordered_set<Position>& positions, Position throw_position);
	
	void SetFormula(std::string text, Position position);
	bool ContainsInDependsFrom(const std::unordered_set<Position>& positions) const;
};