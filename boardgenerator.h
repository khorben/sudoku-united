/**
    This file is part of Sudoku United.

    Sudoku United is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Sudoku United is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Sudoku United.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BOARDGENERATOR_H
#define BOARDGENERATOR_H

#include <vector>
#include <QObject>
#include "sudoku.h"

class LogItem;
class Board;

using namespace std;

/**
 * The board containing all the memory structures and
 * methods for solving or generating sudoku puzzles.
 */
class BoardGenerator {
public:
    enum Difficulty {
        UNKNOWN,
        SIMPLE,
        EASY,
        INTERMEDIATE,
        EXPERT,
    };
    BoardGenerator();
    bool solve();
    int countSolutions();
    bool isSolved();
    void setRecordHistory(bool recHistory);
    void setLogHistory(bool logHist);
    void setBoard(const Board &board);
    bool generatePuzzle(Sudoku::Difficulty difficulty);
    int getGivenCount();
    int getSingleCount();
    int getHiddenSingleCount();
    int getNakedPairCount();
    int getHiddenPairCount();
    int getBoxLineReductionCount();
    int getPointingPairTripleCount();
    int getGuessCount();
    int getBacktrackCount();
    vector<LogItem*>* getSolveInstructions() { return solveInstructions; };
    Board *toBoard() const;
    BoardGenerator::Difficulty getDifficulty();
    ~BoardGenerator();
private:
    /**
         * The 81 integers that make up a sudoku puzzle.
         * Givens are 1-9, unknows are 0.
         * Once initialized, this puzzle remains as is.
         * The answer is worked out in "solution".
         */
    int* puzzle;

    /**
         * The 81 integers that make up a sudoku puzzle.
         * The solution is built here, after completion
         * all will be 1-9.
         */
    int* solution;

    /**
         * Recursion depth at which each of the numbers
         * in the solution were placed.  Useful for backing
         * out solve branches that don't lead to a solution.
         */
    int* solutionRound;

    /**
         * The 729 integers that make up a the possible
         * values for a suduko puzzle. (9 possibilities
         * for each of 81 squares).  If possibilities[i]
         * is zero, then the possibility could still be
         * filled in according to the sudoku rules.  When
         * a possibility is eliminated, possibilities[i]
         * is assigned the round (recursion level) at
         * which it was determined that it could not be
         * a possibility.
         */
    int* possibilities;

    /**
         * An array the size of the board (81) containing each
         * of the numbers 0-n exactly once.  This array may
         * be shuffled so that operations that need to
         * look at each cell can do so in a random order.
         */
    int* randomBoardArray;

    /**
         * An array with one element for each position (9), in
         * some random order to be used when trying each
         * position in turn during guesses.
         */
    int* randomPossibilityArray;

    /**
         * Whether or not to record history
         */
    bool recordHistory;

    /**
         * A list of moves used to solve the puzzle.
         * This list contains all moves, even on solve
         * branches that did not lead to a solution.
         */
    vector<LogItem*>* solveHistory;

    /**
         * A list of moves used to solve the puzzle.
         * This list contains only the moves needed
         * to solve the puzzle, but doesn't contain
         * information about bad guesses.
         */
    vector<LogItem*>* solveInstructions;

    /**
      * Difficulty of the generated board.
      */
    Sudoku::Difficulty m_difficulty;

    /**
         * The last round of solving
         */
    int lastSolveRound;
    bool reset();
    bool singleSolveMove(int round);
    bool onlyPossibilityForCell(int round);
    bool onlyValueInRow(int round);
    bool onlyValueInColumn(int round);
    bool onlyValueInSection(int round);
    bool solve(int round);
    int countSolutions(int round, bool limitToTwo);
    bool guess(int round, int guessNumber);
    bool isImpossible();
    void rollbackRound(int round);
    bool pointingRowReduction(int round);
    bool rowBoxReduction(int round);
    bool colBoxReduction(int round);
    bool pointingColumnReduction(int round);
    bool hiddenPairInRow(int round);
    bool hiddenPairInColumn(int round);
    bool hiddenPairInSection(int round);
    void mark(int position, int round, int value);
    int findPositionWithFewestPossibilities();
    bool handleNakedPairs(int round);
    int countPossibilities(int position);
    bool arePossibilitiesSame(int position1, int position2);
    void addHistoryItem(LogItem* l);
    void markRandomPossibility(int round);
    void shuffleRandomArrays();
    void rollbackNonGuesses();
    void clearPuzzle();
    bool removePossibilitiesInOneFromTwo(int position1, int position2, int round);

};

/**
 * While solving the puzzle, log steps taken in a log item.
 * This is useful for later printing out the solve history
 * or gathering statistics about how hard the puzzle was to
 * solve.
 */
class LogItem {
public:
    enum LogType {
        GIVEN,
        SINGLE,
        HIDDEN_SINGLE_ROW,
        HIDDEN_SINGLE_COLUMN,
        HIDDEN_SINGLE_SECTION,
        GUESS,
        ROLLBACK,
        NAKED_PAIR_ROW,
        NAKED_PAIR_COLUMN,
        NAKED_PAIR_SECTION,
        POINTING_PAIR_TRIPLE_ROW,
        POINTING_PAIR_TRIPLE_COLUMN,
        ROW_BOX,
        COLUMN_BOX,
        HIDDEN_PAIR_ROW,
        HIDDEN_PAIR_COLUMN,
        HIDDEN_PAIR_SECTION,
    };
    LogItem(int round, LogType type);
    LogItem(int round, LogType type, int value, int position);
    int getRound();
    LogType getType();
    int getValue() { return value; }
    int getPosition() { return position; }
    ~LogItem();
private:
    void init(int round, LogType type, int value, int position);
    /**
         * The recursion level at which this item was gathered.
         * Used for backing out log items solve branches that
         * don't lead to a solution.
         */
    int round;

    /**
         * The type of log message that will determine the
         * message printed.
         */
    LogType type;

    /**
         * Value that was set by the operation (or zero for no value)
         */
    int value;

    /**
         * position on the board at which the value (if any) was set.
         */
    int position;
};

void shuffleArray(int* array, int size);
int getLogCount(vector<LogItem*>* v, LogItem::LogType type);

#define GRID_SIZE 3
#define ROW_LENGTH          (GRID_SIZE*GRID_SIZE)
#define COL_HEIGHT          (GRID_SIZE*GRID_SIZE)
#define SEC_SIZE            (GRID_SIZE*GRID_SIZE)
#define SEC_COUNT           (GRID_SIZE*GRID_SIZE)
#define SEC_GROUP_SIZE      (SEC_SIZE*GRID_SIZE)
#define NUM_POSS            (GRID_SIZE*GRID_SIZE)
#define BOARD_SIZE          (ROW_LENGTH*COL_HEIGHT)
#define POSSIBILITY_SIZE    (BOARD_SIZE*NUM_POSS)

class BoardGeneratorWrapper : public QObject {
    Q_OBJECT
public:
    BoardGeneratorWrapper(Sudoku::Difficulty difficulty = Sudoku::EASY, QObject *parent = 0);
    ~BoardGeneratorWrapper();

    Board *board() const { return m_board; }

public slots:
    void startGeneration();
private:
    Board *m_board;
    Sudoku::Difficulty m_difficulty;
};

class HintGenerator : public QObject {
    Q_OBJECT
public:
    HintGenerator(Board* board);

    const BoardGenerator boardGenerator() const { return m_boardGenerator; }

public slots:
    void startHintGeneration();

private:
    BoardGenerator m_boardGenerator;
};

#endif // BOARDGENERATOR_H
