#include "../include/hanoi.h"

// Main entry point that chooses the algorithm
void HanoiSolver::solve(int n, int from, int to, int aux, bool recordMoves, bool useIterative) {
    moveHistory.clear();
    // Reserve memory if recording to prevent reallocation stutter
    if (recordMoves && n < 20) { 
        // 2^n - 1 can get very large, careful with reserve on high n
        size_t predictedMoves = (size_t)pow(2, n) - 1;
        moveHistory.reserve(predictedMoves);
    }

    if (useIterative) {
        solveIterative(n, from, to, aux, recordMoves);
    } else {
        solveRecursive(n, from, to, aux, recordMoves);
    }
}

// ---------------- RECURSIVE IMPLEMENTATION ----------------
void HanoiSolver::solveRecursive(int n, int from, int to, int aux, bool recordMoves) {
    if (n == 0) return;
    
    solveRecursive(n - 1, from, aux, to, recordMoves);
    
    if (recordMoves) {
        moveHistory.push_back({from, to});
    }
    
    solveRecursive(n - 1, aux, to, from, recordMoves);
}

// ---------------- ITERATIVE IMPLEMENTATION ----------------
// Adapted from your snippet
void HanoiSolver::solveIterative(int n, int from, int to, int aux, bool recordMoves) {
    long long totalMoves = (1LL << n) - 1;
    
    // We need 3 stacks representing the pegs
    std::stack<int> sFrom, sAux, sTo;
    
    // We need an array of pointers to access them by index (0, 1, 2)
    // IMPORTANT: The Iterative algo in the snippet assumes specific roles based on index.
    // We map 0->from, 1->aux, 2->to for standard logic.
    std::stack<int>* pegs[3];
    pegs[0] = &sFrom;
    pegs[1] = &sAux; 
    pegs[2] = &sTo;

    // Actual Peg Indices for Move recording
    int pegIndices[3] = {from, aux, to};

    // Initialize source peg (Push disks N down to 1)
    for (int i = n; i >= 1; i--) {
        sFrom.push(i);
    }

    // Determine move directions based on parity of N
    // Logic from snippet:
    // If N is even: Swap Aux and Dest roles in the cyclical move pattern
    int i_src = 0; 
    int i_dest = 2; // Default Dest
    int i_aux = 1;  // Default Aux

    if (n % 2 == 0) {
        i_dest = 1; // Dest is actually at index 1
        i_aux = 2;  // Aux is at index 2
    }

    // Iterative Execution
    for (long long move = 1; move <= totalMoves; move++) {
        if (move % 3 == 1) {
            // Move between Source and Dest
            moveDiskIterative(pegs[i_src], pegs[i_dest], pegIndices[i_src], pegIndices[i_dest], recordMoves);
        }
        else if (move % 3 == 2) {
            // Move between Source and Aux
            moveDiskIterative(pegs[i_src], pegs[i_aux], pegIndices[i_src], pegIndices[i_aux], recordMoves);
        }
        else {
            // Move between Aux and Dest
            moveDiskIterative(pegs[i_aux], pegs[i_dest], pegIndices[i_aux], pegIndices[i_dest], recordMoves);
        }
    }
}

void HanoiSolver::moveDiskIterative(std::stack<int>* fromStack, std::stack<int>* toStack, int fromIdx, int toIdx, bool recordMoves) {
    int fromTop = fromStack->empty() ? INT_MAX : fromStack->top();
    int toTop = toStack->empty() ? INT_MAX : toStack->top();

    // Logic: Always move smaller disk onto larger disk (or empty peg)
    if (fromTop < toTop) {
        toStack->push(fromTop);
        fromStack->pop();
        if (recordMoves) moveHistory.push_back({fromIdx, toIdx});
    } else {
        fromStack->push(toTop);
        toStack->pop();
        if (recordMoves) moveHistory.push_back({toIdx, fromIdx});
    }
}

bool HanoiSolver::saveToCSV(const std::vector<BenchmarkResult>& results, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return false;

    file << "N,Time(ms)\n";
    for (const auto& row : results) {
        file << row.n << "," << row.timeMs << "\n";
    }
    file.close();
    return true;
}
