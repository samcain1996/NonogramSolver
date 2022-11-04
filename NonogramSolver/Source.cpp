#include <vector>
#include <iostream>
#include <optional>
#include <string>
// #include <set>

using NonogramRow = std::vector<bool>;
using RowClues    = std::vector<int>;

using Nonogram = std::vector<NonogramRow>;
using Clues = std::vector<RowClues>;

using PossibleSolution = std::optional<Nonogram>;

// Defines how to print a solution
std::ostream& operator<<(std::ostream& os, const PossibleSolution& possibleSolution) {

    if (possibleSolution.has_value()) {
        
        const Nonogram& solution = possibleSolution.value();
        const int numberOfYUnits = solution.size();
        const int numberofXUnits = solution[0].size();

        for (int x = 0; x < numberofXUnits; ++x) {

            std::string rowStr = "";

            for (int y = 0; y < numberOfYUnits; ++y) {
                rowStr += solution[x][y] ? "X" : "O";
            }
            os << rowStr << "\n";
        }

        os << "\n";

    }

    // No solution
    else {
        os << "No solution found" << std::endl;
    }

    return os;
}

// Returns whether the size of the nanogram matches the clues
bool validDimensions(const Nonogram& nonogram, const Clues& topClues, const Clues& sideClues) {

    const int expectedSize = topClues.size() * sideClues.size();
    const int actualSize = nonogram.size() <= 0 ? 0 : nonogram.size() * nonogram[0].size();

    return expectedSize == actualSize;

}

// Returns whether an individual row or column is valid
bool isListValid(const NonogramRow& list, const RowClues& listClues) {

    RowClues clues(listClues);
    int clueIndex = -1;

    bool element = list[0];
    bool prev = false;

    for (int i = 0; i < list.size(); i++) {

        element = list[i];

        // Group starts when element goes from false to true
        if (element && !prev) {

            // If there are still elements left in current group OR 
            // the next group index is out of bounds, return false
            if ( (clueIndex != -1 && clues[clueIndex] > 0) || ++clueIndex >= clues.size()) { 
                return false; 
            }
        }
        else if (!element) { prev = element; continue; }

        if (element) {
            clues[clueIndex]--;
        }

        if (clues[clueIndex] < 0) { return false; }

        prev = element;

    }

    for (int i = 0; i < clues.size(); ++i) {
        if (clues[i] != 0) { return false; }
    }

    return true;
}

bool isValid(const Clues& topClues, const Clues& sideClues, const Nonogram& nonogram) {

    // If the dimensions don't match, it cannot be valid
    if (!validDimensions(nonogram, topClues, sideClues)) { return false; }

    // Number of rows and columns expected in the nonogram
    const int cols = topClues.size();
    const int rows = sideClues.size();

    Nonogram gridCols(cols, NonogramRow(rows));  // Same as grid but flipped 90 degrees

    for (int rowIndex = 0; rowIndex < rows; ++rowIndex) {

        const NonogramRow& row = nonogram[rowIndex];
        const RowClues clues(sideClues[rowIndex]);

        if (!isListValid(row, clues)) { return false; }

        // Populate column of gridCols
        for (int columnIndex = 0; columnIndex < row.size(); ++columnIndex) {
            gridCols[columnIndex][rowIndex] = row[columnIndex];
        }
    }
    
    for (int colIndex = 0; colIndex < cols; ++colIndex) {

        if (!isListValid(gridCols[colIndex], topClues[colIndex])) { return false; }

    }

    return true;
}

PossibleSolution solve(const Clues& topClues, const Clues& sideClues, int index, Nonogram nonogram) {

    static int boardsGenerated = 0;


    if (index >= topClues.size() * sideClues.size()) {
        if (isValid(topClues, sideClues, nonogram)) {
            std::cout << std::to_string(boardsGenerated) << "\n";
            return PossibleSolution{ nonogram };
        }
        else { return std::nullopt; }

    }
    else if (isValid(topClues, sideClues, nonogram)) { 
        std::cout << std::to_string(boardsGenerated) << "\n"; 
        return PossibleSolution{ nonogram }; 
    }

    // Convert index to X,Y coordinates
    int xIndex = index % sideClues.size();
    int yIndex = index / sideClues.size();

    auto option1 = solve(topClues, sideClues, index + 1, nonogram);

    nonogram[xIndex][yIndex] = true;
    auto option2 = solve(topClues, sideClues, index + 1, nonogram);
    boardsGenerated++;

    return option1.has_value() ? option1 : option2;

}

PossibleSolution solve(const Clues& topClues, const Clues& sideClues) {
    
    Nonogram nonogramData;

    // Init with all false
    for (int col = 0; col < sideClues.size(); ++col) {
        nonogramData.push_back(NonogramRow(topClues.size(), false));
    }

    return solve(topClues, sideClues, 0, nonogramData);

}

static std::tuple<Nonogram, Clues, Clues>  createNonogram() {

    Nonogram nonogram;

    nonogram.push_back({ false, false, true, false, false });
    nonogram.push_back({ false, true, true, true, false });
    nonogram.push_back({ true, false, true, false, true });
    nonogram.push_back({ false, false, true, false, false });
    nonogram.push_back({ false, false, true, false, false });

    Clues topClues = { { 1 }, { 1 }, { 5 }, { 1 }, { 1 } };
    Clues sideClues = { { 1 }, { 3 }, { { 1, 1, 1 } }, { 1 }, { 1 } };

    return { nonogram, topClues, sideClues };

}

int main() {

    Clues topClues = { { 2 }, { 2 }, { 1 } };
    Clues sideClues = { { 1 }, { 2 }, { { 1, 1 } } };

    auto [ nonogram, defaultTopClues, defaultSideClues] = createNonogram();
    bool valid = isValid(defaultTopClues, defaultSideClues, nonogram);

    std::cout << solve(topClues, sideClues);

    return 0;
}