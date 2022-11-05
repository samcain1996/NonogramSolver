#include <vector>
#include <iostream>
#include <optional>
#include <string>
#include <chrono>
// #include <set>

using CellList = std::vector<bool>;
using Clues = std::vector<int>;

using Nonogram = std::vector<CellList>;
using CluesList = std::vector<Clues>;

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
        os << "No solution found" << "\n";
    }

    return os;
}

// Returns whether the size of the nanogram matches the clues
bool validDimensions(const Nonogram& nonogram, const CluesList& topClues, const CluesList& sideClues) {

    const int expectedSize = topClues.size() * sideClues.size();
    const int actualSize = nonogram.size() <= 0 ? 0 : nonogram.size() * nonogram[0].size();

    return expectedSize == actualSize;

}

// Returns whether an individual row or column is valid
bool isListValid(const CellList& list, const Clues& listClues) {

    Clues clues(listClues);
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

bool isValid(const CluesList& topClues, const CluesList& sideClues, const Nonogram& nonogram) {

    // If the dimensions don't match, it cannot be valid
    if (!validDimensions(nonogram, topClues, sideClues)) { return false; }

    // Number of rows and columns expected in the nonogram
    const int cols = topClues.size();
    const int rows = sideClues.size();

    // Check rows
    for (int rowIndex = 0; rowIndex < rows; ++rowIndex) {

        const CellList& row = nonogram[rowIndex];
        const Clues clues(sideClues[rowIndex]);

        if (!isListValid(row, clues)) { return false; }

    }
    
    // Check columns
    for (int colIndex = 0; colIndex < cols; ++colIndex) {

        CellList col(rows, false);
        for (int rowIndex = 0; rowIndex < rows; rowIndex++) {
            col[rowIndex] = nonogram[rowIndex][colIndex];
        }

        if (!isListValid(col, topClues[colIndex])) { return false; }

    }

    return true;
}

PossibleSolution solve(const CluesList& topClues, const CluesList& sideClues, int index, Nonogram nonogram) {

    // static int boardsGenerated = 0;

    if (isValid(topClues, sideClues, nonogram)) {
        return PossibleSolution{ nonogram };
    }
    else if (index >= topClues.size() * sideClues.size()) { return std::nullopt; }

    // Convert index to X,Y coordinates
    int xIndex = index % sideClues.size();
    int yIndex = index / sideClues.size();

    PossibleSolution option1 = solve(topClues, sideClues, index + 1, nonogram);

    nonogram[xIndex][yIndex] = true;
    PossibleSolution option2 = solve(topClues, sideClues, index + 1, nonogram);
    // boardsGenerated++;

    return option1.has_value() ? option1 : option2;

}

PossibleSolution solve(const CluesList& topClues, const CluesList& sideClues) {
    
    Nonogram nonogramData(sideClues.size(), CellList(topClues.size(), false));

    return solve(topClues, sideClues, 0, nonogramData);

}

static std::pair<CluesList, CluesList>  create3x3Puzzle() {

    // Solution
    // 
    // OXO
    // XXO
    // XOX

    CluesList topClues = { { 2 }, { 2 }, { 1 } };
    CluesList sideClues = { { 1 }, { 2 }, { { 1, 1 } } };

    return { topClues, sideClues };

}

static std::pair<CluesList, CluesList>  create5x5Puzzle() {

    // Solution
    // 
    // OOXOO
    // OXXXO
    // XOXOX
    // OOXOO
    // OOXOO

    CluesList topClues = { { 1 }, { 1 }, { 5 }, { 1 }, { 1 } };
    CluesList sideClues = { { 1 }, { 3 }, { { 1, 1, 1 } }, { 1 }, { 1 } };

    return { topClues, sideClues };

}

int main() {

    auto [ topCluesSmall, sideCluesSmall] = create3x3Puzzle();
    auto [ topCluesMed, sideCluesMed] = create5x5Puzzle();

    auto begin = std::chrono::high_resolution_clock::now();

    auto solution = solve(topCluesSmall, sideCluesSmall);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    std::cout << "3x3 Solution:\n" << solution << "Time: " << std::to_string(delta) << " microseconds\n\n";

    begin = std::chrono::high_resolution_clock::now();
    solution = solve(topCluesMed, sideCluesMed);
    end = std::chrono::high_resolution_clock::now();
    delta = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    std::cout << "5x5 Solution:\n" << solution << "Time: " << std::to_string(delta) << " microseconds\n\n";
    return 0;
}