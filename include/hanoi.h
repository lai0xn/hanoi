#ifndef HANOI_H
#define HANOI_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <stack>
#include <climits> // For INT_MAX
#include <cmath>   // For pow

struct Move {
    int fromPeg;
    int toPeg;
};

struct BenchmarkResult {
    int n;
    double timeMs;
};

class HanoiSolver {
public:
    std::vector<Move> moveHistory;
    double lastExecutionTimeMs = 0.0;

    // Updated solve signature to accept algorithm choice
    void solve(int n, int from, int to, int aux, bool recordMoves, bool useIterative);

    void reset() {
        moveHistory.clear();
        lastExecutionTimeMs = 0.0;
    }

    static bool saveToCSV(const std::vector<BenchmarkResult>& results, const std::string& filename);

private:
    void solveRecursive(int n, int from, int to, int aux, bool recordMoves);
    
    // New Iterative Methods
    void solveIterative(int n, int from, int to, int aux, bool recordMoves);
    void moveDiskIterative(std::stack<int>* fromStack, std::stack<int>* toStack, int fromIdx, int toIdx, bool recordMoves);
};

#endif
