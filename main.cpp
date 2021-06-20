#include <iostream>
#include <string>
#include <optional>
#include <tuple>
#include <vector>
#include <limits.h>

using namespace std;

enum Player
{
    Naught,
    Cross
};

Player opponent(Player player)
{
    return (player == Player::Naught) ? Player::Cross : Player::Naught;
}

class Game
{
private:
    int board[3][3];

    char repr_to_symbol(int repr)
    {
        switch (repr)
        {
        case 1:
            return 'O';
        case 2:
            return 'X';
        default:
            return ' ';
        }
    }
    int player_to_repr(Player player)
    {
        switch (player)
        {
        case Player::Naught:
            return 1;
        case Player::Cross:
            return 2;
        default:
            return 0;
        }
    }
    int num_on_row(Player player, int row)
    {
        int player_repr = player_to_repr(player);
        int count = 0;
        for (int i = 0; i < 3; i++)
        {
            if (board[row][i] == player_repr)
            {
                count += 1;
            }
        }
        return count;
    }
    int num_on_column(Player player, int column)
    {
        int player_repr = player_to_repr(player);
        int count = 0;
        for (int i = 0; i < 3; i++)
        {
            if (board[i][column] == player_repr)
            {
                count += 1;
            }
        }
        return count;
    }
    int num_on_diagonals(Player player)
    {
        int player_repr = player_to_repr(player);
        int count = 0;
        if (board[1][1] == player_repr)
        {
            count += 1;
        }
        if (board[0][0] == player_repr)
        {
            count += 1;
            if (board[2][2] == player_repr)
            {
                count += 1;
            }
        }
        else if (board[0][2] == player_repr)
        {
            count += 1;
            if (board[2][0] == player_repr)
            {
                count += 1;
            }
        }
        return count;
    }

public:
    Player current_player = Player::Cross;
    void reset()
    {
        fill_n(board[0], 9, 0);
    }
    bool is_valid_pos(int x, int y)
    {
        return x < 3 && x >= 0 && y < 3 && y >= 0 && board[y][x] == 0;
    }
    optional<Game> play(int x, int y)
    {

        auto new_game = Game();
        copy(&board[0][0], &board[2][2] + 1, &new_game.board[0][0]);
        if (!is_valid_pos(x, y))
        {
            return {};
        }
        new_game.board[y][x] = player_to_repr(current_player);
        new_game.current_player = opponent(current_player);
        return new_game;
    }
    vector<Game> children()
    {
        vector<Game> children = {};
        if (end())
        {
            return children;
        }
        for (auto pos : possible_pos())
        {
            int x, y;
            tie(x, y) = pos;
            children.push_back(play(x, y).value());
        }
        return children;
    }
    string to_string()
    {
        string output = "";
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                output += repr_to_symbol(board[i][j]);
                if (j != 2)
                {
                    output += "|";
                }
            }
            if (i != 2)
            {
                output += "\n-----";
            }
            output += "\n";
        }
        return output;
    }
    bool won(Player player)
    {
        // Check rows
        for (int i = 0; i < 3; i++)
        {
            if (num_on_row(player, i) == 3)
            {
                return true;
            }
        }
        // Check rows
        for (int i = 0; i < 3; i++)
        {
            if (num_on_column(player, i) == 3)
            {
                return true;
            }
        }
        return num_on_diagonals(player) == 3;
    }
    bool board_filled()
    {
        int count = 0;
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if (board[i][j] != 0)
                {
                    count += 1;
                }
            }
        }
        return count == 9;
    }
    bool end()
    {
        return won(Player::Naught) || won(Player::Cross) || board_filled();
    }
    vector<tuple<int, int>> possible_pos()
    {
        vector<tuple<int, int>> pos = {};
        if (end())
        {
            return pos;
        }
        for (int x = 0; x < 3; x++)
        {
            for (int y = 0; y < 3; y++)
            {
                if (is_valid_pos(x, y))
                {
                    pos.push_back(make_tuple(x, y));
                }
            }
        }
        return pos;
    }
};

int alphabeta(Game node, int depth, int alpha, int beta, Player maximizing_player)
{
    if (depth == 0 || node.end())
    {
        if (node.won(maximizing_player))
        {
            return 1;
        }
        else if (node.won(opponent(maximizing_player)))
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
    if (maximizing_player == node.current_player)
    {
        int value = INT_MIN;
        for (auto child : node.children())
        {
            value = max(value, alphabeta(child, depth - 1, alpha, beta, maximizing_player));
            if (value >= beta)
            {
                break;
            }
            alpha = max(alpha, value);
        }
        return value;
    }
    else
    {
        int value = INT_MAX;
        for (auto child : node.children())
        {
            value = min(value, alphabeta(child, depth - 1, alpha, beta, maximizing_player));
            if (value <= alpha)
            {
                break;
            }
            beta = min(beta, value);
        }
        return value;
    }
}

const int DEPTH = 9;

tuple<int, int> search(Game game)
{
    int max_value = INT_MIN;
    int best_x, best_y;
    for (auto pos : game.possible_pos())
    {
        int x, y;
        tie(x, y) = pos;
        int value = alphabeta(game.play(x, y).value(), DEPTH, INT_MIN, INT_MAX, game.current_player);
        if (value > max_value)
        {
            max_value = value;
            best_x = x;
            best_y = y;
        }
    }
    return make_tuple(best_x, best_y);
}

int number_input(string text)
{
    int a;
    cout << text;
    cin >> a;
    while (1)
    {
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout
                << "You have entered the wrong input\n";
            cin >> a;
        }
        if (!cin.fail())
            break;
    }
    return a;
}

int main()
{
    int user_choice = number_input("Choose first or second: ");
    while (user_choice != 1 && user_choice != 2)
    {
        user_choice = number_input("Choose first or second: ");
    }
    Game game = Game();
    Player first_player = game.current_player;
    Player user_player = (user_choice == 1) ? first_player : opponent(first_player);

    while (!game.end())
    {
        if (game.current_player == user_player)
        {
            cout << game.to_string();
            int x = number_input("x: ");
            int y = number_input("y: ");
            auto result = game.play(x, y);
            while (!result.has_value())
            {
                x = number_input("x: ");
                y = number_input("y: ");
                result = game.play(x, y);
            }
            game = result.value();
        }
        else
        {
            int x, y;
            tie(x, y) = search(game);
            game = game.play(x, y).value();
        }
    }
    cout << game.to_string();
    string player_name;
    if (game.won(Player::Naught))
    {
        player_name = "Naught player";
    }
    else if (game.won(Player::Cross))
    {
        player_name = "Cross player";
    }
    else
    {
        player_name = "No one";
    }
    cout << player_name + " is victorious!\n";
    return 0;
}