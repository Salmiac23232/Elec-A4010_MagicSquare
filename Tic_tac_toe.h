int actions(int array[]) {
  int availableMoves = 0;
  for (int i = 0; i < 9; i++) {
    if (array[i] == EMPTY) {
      availableMoves++;
    }
  }
  return availableMoves;
}

char player(int array[]) {
  int num_x = 0, num_o = 0;
  for (int i = 0; i < 9; i++) {
    if (array[i] == X) num_x++;
    if (array[i] == O) num_o++;
  }
  return num_o == num_x ? 'X' : 'O';
}


int minimax(int array[], int depth, int alpha, int beta, bool isMaximizing) {
  int score = evaluate(array);
  if (score != 0) return score;

  if (actions(array) == 0) return 0;

  if (isMaximizing) {
    int maxEval = -1000;
    for (int i = 0; i < 9; i++) {
      if (!array[i]) {
        array[i] = X;
        int eval = minimax(array, depth + 1, alpha, beta, false);
        array[i] = EMPTY;
        maxEval = max(maxEval, eval);
        alpha = max(alpha, eval);
        if (beta <= alpha) break;
      }
    }
    return maxEval;
  } else {
    int minEval = 1000;
    for (int i = 0; i < 9; i++) {
      if (!array[i]) {
        array[i] = O;
        int eval = minimax(array, depth + 1, alpha, beta, true);
        array[i] = EMPTY;
        minEval = min(minEval, eval);
        beta = min(beta, eval);
        if (beta <= alpha) break;
      }
    }
    return minEval;
  }
}

int evaluate(int array[]) {
  int winning_combinations[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, // Rows
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, // Columns
    {0, 4, 8}, {2, 4, 6}            // Diagonals
  };

  for (int i = 0; i < 8; i++) {
    if (array[winning_combinations[i][0]] == array[winning_combinations[i][1]] &&
        array[winning_combinations[i][1]] == array[winning_combinations[i][2]] &&
        array[winning_combinations[i][0]] != EMPTY) {
      return (array[winning_combinations[i][0]] == 1) ? 1 : -1;
    }
  }
  return 0;
}

int bestMove(int array[]) {
  int bestVal = -1000;
  int move = -1;

  for (int i = 0; i < 9; i++) {
    if (array[i] == EMPTY) {
      array[i] = X;
      int moveVal = minimax(array, 0, -1000, 1000, false);
      array[i] = EMPTY;
      if (moveVal > bestVal) {
        move = i;
        bestVal = moveVal;
      }
    }
  }
  return move;
}

bool terminal(int array[]) {
  return evaluate(array) != 0 || actions(array) == 0;
}

