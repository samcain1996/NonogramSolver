#include <vector>
#include <iostream>
#include <optional>
#include <string>
#include <chrono>
#include <algorithm>
#include <numeric>

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

bool validNumberOfGroups(const CellList& list, const int expected) {

    CellList::const_iterator iter = list.begin();
    int actualGroups = (int)*iter;  // If the first cell is true, then mark that as a group

    while (iter != list.end() && actualGroups < expected) {
        // Find next group ( false (blank) cell preceding true (filled) cell )
        iter = std::adjacent_find(iter, list.end(), [](const bool& curr, const bool& next) { return !curr && next; });

        if (iter != list.end()) { actualGroups++; iter++; }
    }

    return actualGroups == expected;
}

// Returns whether an individual row or column is valid
bool isListValid(const CellList& list, const Clues& clues) {

    const int actualTruths = std::count(list.begin(), list.end(), true);
    const int expectedTruths = std::accumulate(clues.begin(), clues.end(), 0);

    if (actualTruths != expectedTruths) { return false; }
    if (!validNumberOfGroups(list, clues.size())) { return false; }

    return true;
}

 CellList getColumn(const Nonogram& nonogram, const int rows, const int columnIndex) {
    
    CellList col(rows, false);

    for (int rowIndex = 0; rowIndex < rows; ++rowIndex) {
        col[rowIndex] = nonogram[rowIndex][columnIndex];
    }

    return col;
}

bool isValid(const CluesList& topClues, const CluesList& sideClues, const Nonogram& nonogram) {

    // If the dimensions don't match, it cannot be valid
    if (!validDimensions(nonogram, topClues, sideClues)) { return false; }

    // Number of rows and columns expected in the nonogram
    const int& cols = topClues.size();
    const int& rows = sideClues.size();

    // Check rows
    for (int x = 0; x < rows; ++x) {
        if (!isListValid(nonogram[x], sideClues[x])) { return false; }
    }
    
    // Check columns
    for (int y = 0; y < cols; ++y) {
        if (!isListValid(getColumn(nonogram, rows, y), topClues[y])) { return false; }
    }

    return true;
}

PossibleSolution solve(const CluesList& topClues, const CluesList& sideClues, const int index, const Nonogram& nonogram) {

    if (isValid(topClues, sideClues, nonogram)) {
        return PossibleSolution{ nonogram };
    }
    else if (index >= topClues.size() * sideClues.size()) { return std::nullopt; }

    // Convert index to X,Y coordinates
    const int x = index % sideClues.size();
    const int y = index / sideClues.size();

    Nonogram copy(nonogram);

    PossibleSolution option1 = solve(topClues, sideClues, index + 1, nonogram);

    copy[x][y] = true;
    PossibleSolution option2 = solve(topClues, sideClues, index + 1, copy);

    return option1.has_value() ? option1 : option2;

}

PossibleSolution solve(const CluesList& topClues, const CluesList& sideClues) {
    
    const Nonogram nonogramData(sideClues.size(), CellList(topClues.size(), false));

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

static std::pair<CluesList, CluesList>  create4x4Puzzle() {

    // Solution
    //
    // XOXO
    // XXXX
    // OOXX
    // OOXO

    CluesList topClues = { 
        { 2 }, { 1 }, { 4 }, { 2 } 
    };
    CluesList sideClues = { 
        { { 1, 1 } }, { 4 }, { 2 }, { 1 } 
    };

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

    auto [ topCluesTiny, sideCluesTiny] = create3x3Puzzle();
    auto [ topCluesSmall, sideCluesSmall] = create4x4Puzzle();
    auto [ topCluesMed, sideCluesMed] = create5x5Puzzle();

    auto begin = std::chrono::high_resolution_clock::now();
    auto solution = solve(topCluesTiny, sideCluesTiny);
    auto end = std::chrono::high_resolution_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    std::cout << "3x3 Solution:\n" << solution << "Time: " << std::to_string(delta) << " microseconds\n\n";

    begin = std::chrono::high_resolution_clock::now();
    solution = solve(topCluesSmall, sideCluesSmall);
    end = std::chrono::high_resolution_clock::now();
    delta = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    std::cout << "4x4 Solution:\n" << solution << "Time: " << std::to_string(delta) << " microseconds\n\n";

    begin = std::chrono::high_resolution_clock::now();
    solution = solve(topCluesMed, sideCluesMed);
    end = std::chrono::high_resolution_clock::now();
    delta = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

    std::cout << "5x5 Solution:\n" << solution << "Time: " << std::to_string(delta) << " microseconds\n\n";

    return 0;
}