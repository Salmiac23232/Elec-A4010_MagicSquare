#include <stdio.h>
#include <stdbool.h>

#define X 1  // X on 1
#define O 2  // O on 2
#define EMPTY 0  // Tyhjä on 0

// Laskee käytettävissä olevat siirrot (tyhjät ruudut)
int actions(int array[]) {
    int availableMoves = 0;
    for (int i = 0; i < 9; i++) {
        if (array[i] == EMPTY) {
            availableMoves++;
        }
    }
    return availableMoves;
}

// Määrittelee vuorossa olevan pelaajan (X tai O)
char player(int array[]) {
    int num = 0;
    for (int i = 0; i < 9; i++) 
      if (array[i] != EMPTY) num++;
    return (num % 2) ? O : X;  // X menee aina ensin
}

// Arvioi pelilaudan tilanteen voittotilanteen perusteella
int evaluate(int array[]) {
    int winning_combinations[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // Rivit
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // Sarakkeet
        {0, 4, 8}, {2, 4, 6}             // Diagonaalit
    };

    for (int i = 0; i < 8; i++) {
        if (array[winning_combinations[i][0]] == array[winning_combinations[i][1]] &&
            array[winning_combinations[i][1]] == array[winning_combinations[i][2]] &&
            array[winning_combinations[i][0]] != EMPTY) {
            return (array[winning_combinations[i][0]] == X) ? 1 : -1;  // X voittaa: 1, O voittaa: -1
        }
    }
    return 0;  // Ei voittajaa
}

// Minimax-algoritmi alpha-beta leikkuulla
int minimax(int array[], int depth, int alpha, int beta, bool isMaximizing) {
    int score = evaluate(array);
    if (score != 0) return score;  // Jos peli on päättynyt (voitto tai tasapeli)

    if (actions(array) == 0) return 0;  // Jos ei ole enää siirtoja, on tasapeli

    if (isMaximizing) {
        int maxEval = -1000;
        for (int i = 0; i < 9; i++) {
            if (array[i] == EMPTY) {
                array[i] = X;  // Maksimoiva pelaaja (X)
                int eval = minimax(array, depth + 1, alpha, beta, false);
                array[i] = EMPTY;
                maxEval = (eval > maxEval) ? eval : maxEval;
                alpha = (alpha > eval) ? alpha : eval;
                if (beta <= alpha) break;  // Beta-leikkaus
            }
        }
        return maxEval;
    } else {
        int minEval = 1000;
        for (int i = 0; i < 9; i++) {
            if (array[i] == EMPTY) {
                array[i] = O;  // Minimoiva pelaaja (O)
                int eval = minimax(array, depth + 1, alpha, beta, true);
                array[i] = EMPTY;
                minEval = (eval < minEval) ? eval : minEval;
                beta = (beta < eval) ? beta : eval;
                if (beta <= alpha) break;  // Alpha-leikkaus
            }
        }
        return minEval;
    }
}

// Etsi paras siirto nykyiselle pelaajalle
int bestMove(int array[]) {
    int bestVal = -1000;
    int move = -1;

    for (int i = 0; i < 9; i++) {
        if (array[i] == EMPTY) {
            array[i] = X;  // AI tekee siirron (X)
            int moveVal = minimax(array, 0, -1000, 1000, false);  // Tarkistetaan X:n siirron arvo
            array[i] = EMPTY;
            if (moveVal > bestVal) {
                move = i;
                bestVal = moveVal;
            }
        }
    }
    return move;
}

// Tarkista, onko peli päättynyt (voitto tai tasapeli)
bool terminal(int array[]) {
    return (evaluate(array) != 0 || actions(array) == 0);
}


