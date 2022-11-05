#include <vector>
#include <iostream>
#include <optional>
#include <string>
#include <chrono>
#include <algorithm>
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

    int expectedTruths = 0;
    std::for_each(listClues.begin(), listClues.end(), [&expectedTruths](const int& clue) { expectedTruths += clue; });

    if (std::count(list.begin(), list.end(), true) != expectedTruths) { return false; }

    int actualGroups = list[0];
    for (CellList::const_iterator iter = list.begin(), bool startingVal = *iter; iter != list.end(); actualGroups++) {
        iter = std::adjacent_find(iter, list.end(), [](const bool& curr, const bool& next) { 
            return !curr && next;  
        });
        actualGroups += 0;
    }
    if (actualGroups > listClues.size()) { return false; }

    Clues clues(listClues);
    int clueIndex = -1;

    bool prev = false;

    for (const bool& element : list) {

        // Group starts when element goes from false to true
        if (element) {

            if (!prev) {

                // If there are still elements left in current group OR 
                // the next group index is out of bounds, return false
                if ((clueIndex != -1 && clues[clueIndex] > 0) || ++clueIndex >= clues.size()) {
                    return false;
                }
            }

            clues[clueIndex]--;
        }
        else if (!element) { goto setPrev; }

        if (clues[clueIndex] < 0) { return false; }

        setPrev:
        prev = element;

    }

    return std::none_of(clues.begin(), clues.end(), [](const int& clue) { return clue != 0; });
}

bool isValid(const CluesList& topClues, const CluesList& sideClues, const Nonogram& nonogram) {

    // If the dimensions don't match, it cannot be valid
    if (!validDimensions(nonogram, topClues, sideClues)) { return false; }

    // Number of rows and columns expected in the nonogram
    const int& cols = topClues.size();
    const int& rows = sideClues.size();

    // Check rows
    for (int x = 0; x < rows; ++x) {

        const CellList& row = nonogram[x];
        const Clues& clues(sideClues[x]);


        if (!isListValid(row, clues)) { return false; }

    }
    
    // Check columns
    for (int y = 0; y < cols; ++y) {

        CellList col(rows, false);
        for (int i = 0; i < rows; i++) {
            col[i] = nonogram[i][y];
        }

        if (!isListValid(col, topClues[y])) { return false; }

    }

    return true;
}

PossibleSolution solve(const CluesList& topClues, const CluesList& sideClues, const int index, Nonogram nonogram) {

    // static int boardsGenerated = 0;

    if (isValid(topClues, sideClues, nonogram)) {
        return PossibleSolution{ nonogram };
    }
    else if (index >= topClues.size() * sideClues.size()) { return std::nullopt; }

    // Convert index to X,Y coordinates
    const int x = index % sideClues.size();
    const int y = index / sideClues.size();

    PossibleSolution option1 = solve(topClues, sideClues, index + 1, nonogram);

    nonogram[x][y] = true;
    PossibleSolution option2 = solve(topClues, sideClues, index + 1, nonogram);
    // boardsGenerated++;

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