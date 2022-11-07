#pragma once 

#include <string>
#include <vector>
#include <chrono>
#include <numeric>
#include <optional>
#include <algorithm>

/*----------Aliases--------------*/

using Cell = bool;
using Clue = int;

using CellList = std::vector<Cell>;
using Row = CellList;
using Column = CellList;

using Clues = std::vector<Clue>;

using Nonogram = std::vector<CellList>;
using CluesList = std::vector<Clues>;

using PossibleSolution = std::optional<Nonogram>;

/*-------------------------------------------*/


/*------------Sample Nonograms--------------*/

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

/*-------------------------------------------*/