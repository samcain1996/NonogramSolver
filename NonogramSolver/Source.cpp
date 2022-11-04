#include <vector>
#include <iostream>
#include <optional>
#include <set>

using NonogramRow = std::vector<bool>;
using RowGroup = std::vector<int>;

using NonogramData = std::vector<NonogramRow>;
using NonogramGroups = std::vector<RowGroup>;

using Nonogram = std::pair<NonogramData, std::pair<NonogramGroups, NonogramGroups>>;

bool validDimensions(const Nonogram& nonogram) {

    const NonogramData& nonogramData = nonogram.first;

    const NonogramGroups& horizontalGroups = nonogram.second.first;
    const NonogramGroups& verticalGroups = nonogram.second.second;

    const int expectedSize = horizontalGroups.size() * verticalGroups.size();
    const int actualSize = nonogramData.size() <= 0 ? 0 : nonogramData.size() * nonogramData[0].size();

    return expectedSize == actualSize;

}

bool isListValid(const NonogramRow& list, const RowGroup& Groups) {

    RowGroup groups(Groups);
    int groupIndex = -1;

    bool element = list[0];
    bool prev = false;

    for (int i = 0; i < list.size(); i++) {

        element = list[i];

        // Group starts when element goes from false to true
        if (element && !prev) {

            // If there are still elements left in current group OR 
            // the next group index is out of bounds, return false
            if ( (groupIndex != -1 && groups[groupIndex] > 0) || ++groupIndex >= groups.size()) { 
                return false; 
            }
        }
        else if (!element) { prev = element; continue; }

        if (element) {
            groups[groupIndex]--;
        }

        if (groups[groupIndex] < 0) { return false; }

        prev = element;

    }

    for (int i = 0; i < groups.size(); ++i) {
        if (groups[i] != 0) { return false; }
    }

    return true;
}

bool isValid(const NonogramGroups& horiNumbers, const NonogramGroups& vertNumbers,
       const NonogramData& grid) {

    // If the dimensions don't match, it cannot be valid
    if (!validDimensions({ grid, {horiNumbers, vertNumbers} })) { return false; }

    // Number of rows and columns expected in the nonogram
    const int cols = horiNumbers.size();
    const int rows = vertNumbers.size();

    NonogramData gridCols(cols, NonogramRow(rows));  // Same as grid but flipped 90 degrees

    for (int rowIndex = 0; rowIndex < rows; ++rowIndex) {

        const NonogramRow& row = grid[rowIndex];
        const RowGroup groups(vertNumbers[rowIndex]);

        if (!isListValid(row, groups)) { return false; }

        // Populate column of gridCols
        for (int x = 0; x < row.size(); ++x) {
            gridCols[x][rowIndex] = row[x];
        }
    }
    
    for (int colIndex = 0; colIndex < cols; ++colIndex) {

        if (!isListValid(gridCols[colIndex], horiNumbers[colIndex])) { return false; }

    }

    return true;
}

bool solve(const NonogramGroups& horiNumbers, const NonogramGroups& vertNumbers, int index, NonogramData curGrid, NonogramData& solution) {

    // Convert index to X,Y coordinates
    int xIndex = index % vertNumbers.size();
    int yIndex = index / vertNumbers.size();

    if (index < vertNumbers.size() * horiNumbers.size()) {

        if (solve(horiNumbers, vertNumbers, index + 1, curGrid, solution)) { return true; }

        // Change value and generate again
        curGrid[xIndex][yIndex] = true;
        if (solve(horiNumbers, vertNumbers, index + 1, curGrid, solution)) { return true; }
        else { return false; }

    }
    else if (index == vertNumbers.size() * horiNumbers.size()) {
        if (isValid(horiNumbers, vertNumbers, curGrid)) { solution = curGrid; return true; }
        return false;
    }
    else { return false; }

}

std::optional<NonogramData> generate(const NonogramGroups& horiNumbers, const NonogramGroups& vertNumbers, int index, NonogramData nonogram) {

    static std::set<NonogramData> alreadyGenerated;

    const int rows = horiNumbers.size();
    const int cols = vertNumbers.size();

    if (index >= rows * cols) { 
        static int boardsGenerated = 0;
        std::cout << ++boardsGenerated << "\n";
        if (!alreadyGenerated.insert(nonogram).second) { std::cout << "Already Generated!\n";  exit(-1); }
        else if (isValid(horiNumbers, vertNumbers, nonogram)) {
          
            return std::optional{ nonogram };
        }

        return std::nullopt; 
    }

    // Convert index to X,Y coordinates
    int xIndex = index % vertNumbers.size();
    int yIndex = index / vertNumbers.size();

    auto option1 = generate(horiNumbers, vertNumbers, index + 1, nonogram);

    nonogram[xIndex][yIndex] = true;
    auto option2 = generate(horiNumbers, vertNumbers, index + 1, nonogram);

    return option1 == std::nullopt ? option2 : option1;

}

std::optional<NonogramData> solve(const NonogramGroups& horiNumbers, const NonogramGroups& vertNumbers) {
    
    NonogramData nonogramData;

    // Init with all false
    for (int col = 0; col < vertNumbers.size(); ++col) {
        nonogramData.push_back(NonogramRow(horiNumbers.size(), false));
    }

    return generate(horiNumbers, vertNumbers, 0, nonogramData);

}

static Nonogram createNonogram() {

    NonogramData nonogram;

    nonogram.push_back({ false, false, true, false, false });
    nonogram.push_back({ false, true, true, true, false });
    nonogram.push_back({ true, false, true, false, true });
    nonogram.push_back({ false, false, true, false, false });
    nonogram.push_back({ false, false, true, false, false });

    NonogramGroups horiGroups = { { 1 }, { 1 }, { 5 }, { 1 }, { 1 } };
    NonogramGroups vertGroups = { { 1 }, { 3 }, { { 1, 1, 1 } }, { 1 }, { 1 } };

    return { nonogram, { horiGroups, vertGroups } };

}

int main() {

    NonogramGroups horiGroups = { { 2 }, { 2 }, { 1 } };
    NonogramGroups vertGroups = { { 1 }, { 2 }, { { 1, 1 } } };

    //NonogramData solved = solve( horiGroups, vertGroups );
    bool valid = isValid(createNonogram().second.first, createNonogram().second.second, createNonogram().first);
    std::optional<NonogramData> solved = solve(horiGroups, vertGroups);

    return solved.has_value() ? 1 : 0;
}