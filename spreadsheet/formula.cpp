#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << "#DIV/0!";
}

namespace {
class Formula : public FormulaInterface {
public:
    explicit Formula(std::string expression);
    Value Evaluate(const SheetInterface& sheet) const override;
    std::string GetExpression() const override;

    std::vector<Position> GetReferencedCells() const override;

private:
    FormulaAST ast_;
};
Formula::Formula(std::string expression)
: ast_{ParseFormulaAST(expression)}{
}
   
FormulaInterface::Value Formula::Evaluate(const SheetInterface& sheet) const {
    try {
        return ast_.Execute(sheet);
    } catch (const FormulaError& e) {
        return e;
    }
}
    
std::string Formula::GetExpression() const {
    std::stringstream ret_val;
    ast_.PrintFormula(ret_val);
    return ret_val.str();
}

std::vector<Position> Formula::GetReferencedCells() const {
	
	std::set<Position> temp_set(ast_.GetCells().begin(), ast_.GetCells().end());
	
    return std::vector<Position>(temp_set.begin(), temp_set.end());
}
    
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    } catch (const ParsingError& e) {
        throw FormulaException("Parsing error");
    } catch (const std::exception& e) {
		throw FormulaException(e.what());
	} catch(...) {
		throw FormulaException("Unknown error");
	}
}