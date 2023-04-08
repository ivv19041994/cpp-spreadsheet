#pragma once

#include "common.h"
#include "formula.h"

class Cell : public CellInterface {
public:
    Cell(SheetInterface& sheet);
    ~Cell();

    void Set(std::string text, Position position);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
	
	
	
private:

	SheetInterface& sheet_;
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;
    std::unique_ptr<Impl> impl_;
	
	std::unique_set<Position> this_depends_from;//от каких ячеек зависит эта ячейка
	std::unique_set<Position> other_depends_this;//какие ячейки зависят от этой
	
	std::optional<Value> cash_;
	
	void ResetCash();//рекурсивно сбросит кэш этой ячейке и всем other_depends_this
	
	void DependsFrom(Position position);
	void NotDependsFrom(Position position);
	
	void InsertCellDependsFromThis(Position position);
	void RemoveCellDependsFromThis(Position position);
	
	void InsertThisDepends(Position position); 
	void RemoveThisDepends(Position position); 
	
	void CheckCircularReference(std::unique_set<Position>& already_check, Position throw_position);
	
	
};