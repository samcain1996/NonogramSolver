#include <iostream>
#include <exception>
#include "DefsAndConstants.h"

// Defines how to print a nonogram row
std::ostream& operator<<(std::ostream& os, const Row& row) {

    static const char filled = 'X';
    static const char empty = 'O';

    std::string rowStr = "";
    std::for_each(row.begin(), row.end(), [&rowStr](const auto& element) { rowStr += element ? filled : empty; });

    return os << rowStr << "\n";

}

// Defines how to print a solution
std::ostream& operator<<(std::ostream& os, const PossibleSolution& possibleSolution) {

    if (possibleSolution.has_value()) {

        const Nonogram& solution = possibleSolution.value();

        // Print each row in solution
        std::for_each(solution.begin(), solution.end(), [](const Row& row) { std::cout << row; });

        return os << "\n";

    }

    // No solution
    return os << "No solution found" << "\n";

}

Column getColumn(const Nonogram& nonogram, const int rows, const int columnIndex) {
    
    Column col(rows, false);

    for (int rowIndex = 0; rowIndex < rows; ++rowIndex) {
        col[rowIndex] = nonogram[rowIndex][columnIndex];
    }

    return col;
}

// Returns whether the size of the nanogram matches the clues
bool validDimensions(const Nonogram& nonogram, const CluesList& topClues, const CluesList& sideClues) {

    const int expectedSize = topClues.size() * sideClues.size();
    const int actualSize = nonogram.size() <= 0 ? 0 : nonogram.size() * nonogram[0].size();

    return expectedSize == actualSize;

}

// Returns whether the number of groups of 'true' match how many are expected
bool validNumberOfGroups(const CellList& list, const int expected) {

    auto iter = list.begin();
    int actualGroups = list[0];  // If the first cell is true, then mark that as a group

    while (iter != list.end() && actualGroups < expected) {
        // Find next group ( false (blank) cell preceding true (filled) cell )
        iter = std::adjacent_find(iter, list.end(), [](const bool& curr, const bool& next) { return !curr && next; });

        if (iter != list.end()) { actualGroups++; iter++; }
    }

    return actualGroups == expected;
}

// Returns whether an individual row or column is valid
bool isListValid(const CellList& list, const Clues& clues) {

    const int actualFilled = std::count(list.begin(), list.end(), true);
    const int expectedFilled = std::accumulate(clues.begin(), clues.end(), 0);

    if (actualFilled != expectedFilled || 
        !validNumberOfGroups(list, clues.size())) [[likely]] { return false; }

    return true;
}

bool isValid(const CluesList& topClues, const CluesList& sideClues, const Nonogram& nonogram) {

    // If the dimensions don't match, it cannot be valid
    if (!validDimensions(nonogram, topClues, sideClues)) { return false; }

    // Number of rows and columns expected in the nonogram
    const int& cols = topClues.size();
    const int& rows = sideClues.size();

    // Check rows
    for (int x = 0; x < rows; ++x) {

        if (!isListValid(nonogram[x], sideClues[x])) [[likely]] { return false; }

    }
    
    // Check columns
    for (int y = 0; y < cols; ++y) {

        Column col = getColumn(nonogram, rows, y);
        if (!isListValid(col, topClues[y])) [[likely]] { return false; }

    }

    return true;
}

PossibleSolution solve(const CluesList& topClues, const CluesList& sideClues, const int index, const Nonogram& nonogram) {

    if (isValid(topClues, sideClues, nonogram)) [[unlikely]] {
        return PossibleSolution{ nonogram };
    }
    else if (index >= topClues.size() * sideClues.size()) { return std::nullopt; }

    // Convert 1-D index to 2-D X,Y coordinates
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

PossibleSolution smartSolve(const CluesList& topClues, const CluesList& sideClues) { throw std::exception("Function not yet implemented"); }

int main() {

    using namespace std::chrono;

    // 3 Puzzles of size 3x3, 4x4, and 5x5
    auto [ topCluesTiny, sideCluesTiny] = create3x3Puzzle();
    auto [ topCluesSmall, sideCluesSmall] = create4x4Puzzle();
    auto [ topCluesMed, sideCluesMed] = create5x5Puzzle();

    // Tiny 3x3
    auto begin = high_resolution_clock::now();
    auto solution = solve(topCluesTiny, sideCluesTiny);
    auto end = high_resolution_clock::now();
    auto delta = duration_cast<microseconds>(end - begin).count();
    std::cout << "3x3 Solution:\n" << solution << "Time: " << std::to_string(delta) << " microseconds\n\n";

    // Small 4x4
    begin = high_resolution_clock::now();
    solution = solve(topCluesSmall, sideCluesSmall);
    end = high_resolution_clock::now();
    delta = duration_cast<microseconds>(end - begin).count();
    std::cout << "4x4 Solution:\n" << solution << "Time: " << std::to_string(delta) << " microseconds\n\n";

    // Medium 5x5
    begin = high_resolution_clock::now();
    solution = solve(topCluesMed, sideCluesMed);
    end = high_resolution_clock::now();
    delta = duration_cast<microseconds>(end - begin).count();
    std::cout << "5x5 Solution:\n" << solution << "Time: " << std::to_string(delta) << " microseconds\n\n";

    return 0;
}
