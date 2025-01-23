import math

X = "X"
O = "O"
EMPTY = None


def initial_state():
    """
    Returns starting state of the board.
    """
    return [EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY,
            EMPTY, EMPTY, EMPTY]


def player(board):
    num_x, num_o = 0, 0
    for x in board:
        if x == X:
            num_x += 1
        elif x == O:
            num_o += 1
    if num_o == num_x:
        return X
    return O
            

def actions(board):
    actions = []
    for x in range(9):
        if board[x] == EMPTY:
            actions.append(x)
    return actions
            

def result(board, action):
    if action not in actions(board):
        raise ValueError("Invalid action")
    
    board2 = board[:]
    board2[action] = player(board)
    return board2


def winner(board):
    # Winning conditions
    winning_combinations = [
        [0, 1, 2], [3, 4, 5], [6, 7, 8],  # Rows
        [0, 3, 6], [1, 4, 7], [2, 5, 8],  # Columns
        [0, 4, 8], [2, 4, 6]  # Diagonals
    ]
    
    for comb in winning_combinations:
        if board[comb[0]] == board[comb[1]] == board[comb[2]] != EMPTY:
            return board[comb[0]]
    
    return None


def terminal(board):
    if winner(board) is not None or len(actions(board)) == 0:
        return True
    return False


def utility(board):
    """
    Returns 1 if X has won the game, -1 if O has won, 0 otherwise.
    """
    if winner(board) == X:
        return 1
    elif winner(board) == O:
        return -1
    return 0


def evaluate(board, alpha, beta):
    if terminal(board):
        return utility(board)
    
    if player(board) == X:
        maxeval = -math.inf
        for choice in actions(board):
            newboard = result(board, choice)
            eval = evaluate(newboard, alpha, beta)                 
            maxeval = max(eval, maxeval)
            alpha = max(alpha, eval)
            if maxeval >= beta:
                break
        return maxeval
    else:
        mineval = math.inf
        for choice in actions(board):
            newboard = result(board, choice)
            eval = evaluate(newboard, alpha, beta)                
            mineval = min(eval, mineval)
            beta = min(beta, eval)
            if mineval <= alpha:
                break
        return mineval
    
    
def minimax(board):
    if terminal(board):
        return None
    
    best_move = None
    best_value = -math.inf if player(board) == X else math.inf
    
    for choice in actions(board):
        newboard = result(board, choice)
        value = evaluate(newboard, -math.inf, math.inf)
        if (player(board) == X and value > best_value) or (player(board) == O and value < best_value):
            best_value = value
            best_move = choice
    
    return best_move

def print_board(board):
    """
    Prints the board in a readable format.
    """
    for i in range(3):
        print(f"{board[i * 3]} | {board[i * 3 + 1]} | {board[i * 3 + 2]}")
        print("")

def main():
    """
    Main function to run the Tic-Tac-Toe game.
    """
    board = initial_state()
    print("Welcome to Tic-Tac-Toe!")
    print_board(board)
    
    while not terminal(board):
        if player(board) == X:
            print("\nPlayer X's turn")
            move = minimax(board)  # AI makes a move
        else:
            print("\nPlayer O's turn")
            move = None
            while move not in actions(board):
                try:
                    move = int(input(f"Choose your move (0-8) for O: "))
                    if move not in actions(board):
                        print("Invalid move. Try again.")
                except ValueError:
                    print("Invalid input. Please enter an integer between 0 and 8.")
        
        # Apply the move
        board = result(board, move)
        print_board(board)
        
    # Game over
    winner_player = winner(board)
    if winner_player:
        print(f"\n{winner_player} wins!")
    else:
        print("\nIt's a tie!")

if __name__ == "__main__":
    main()

            
            
    
                    
                    
                    
                    
                
                
