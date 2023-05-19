#include <bits/stdc++.h>

using namespace std;

typedef char player_t;

#define NONE -1
#define PLAYER0 0
#define PLAYER1 1

#define OPPONENT(player) 1 - player

#define X_IN_DATA 0xF
#define Y_IN_DATA 0xF0

double BOARD_WEIGHTS[8][8] = {
    {20, -3, 11, 8, 8, 11, -3, 20},
    {-3, -7, -4, 1, 1, -4, -7, -3},
    {11, -4, 2, 2, 2, 2, -4, 11},
    {8, 1, 2, -3, -3, 2, 1, 8},
    {8, 1, 2, -3, -3, 2, 1, 8},
    {11, -4, 2, 2, 2, 2, -4, 11},
    {-3, -7, -4, 1, 1, -4, -7, -3},
    {20, -3, 11, 8, 8, 11, -3, 20},
};

struct pos_t{
public:
    char data;

    pos_t(int x, int y){
        if(x == -1)
            x = 0xF;
        if(y == -1)
            y = 0xF;

        data = (x | (y << 4));
    }

    pos_t()
        : pos_t(-1, -1) {}

    bool isEmpty(){
        return data == -1;
    }

    void set_x(int x){
        if(x == -1)
            x = 0xF;
        data = (data & Y_IN_DATA) | x;
    }

    void set_y(int y){
        if(y == -1)
            y = 0xF;
        data = (data & X_IN_DATA) | (y << 4);
    }

    int get_x() const{
        int x = data & X_IN_DATA;
        if(x == 0xF)
            x = -1;
        return x;
    }

    int get_y() const{
        int y = (data & Y_IN_DATA) >> 4;
        if(y == 0xF)
            y = -1;
        return y;
    }

    friend bool operator==(const pos_t &x, const pos_t &y)
    {
        return x.get_x() == y.get_x() && x.get_y() == y.get_y();
    }
};

vector<pos_t> DIRS = {
    pos_t(-1, -1),
    pos_t(-1, 0),
    pos_t(-1, 1),
    pos_t(0, 1),
    pos_t(1, 1),
    pos_t(1, 0),
    pos_t(1, -1),
    pos_t(0, -1)
};

vector<pos_t> CLOSE_CORNERS = {
    pos_t(0, 1),
    pos_t(1, 0),
    pos_t(1, 1),
    pos_t(6, 0),
    pos_t(7, 1),
    pos_t(6, 1),
    pos_t(0, 6),
    pos_t(1, 7),
    pos_t(1, 6),
    pos_t(6, 6),
    pos_t(6, 7),
    pos_t(7, 6)
};

#define WEST 0
#define NORTHWEST 1
#define NORTH 2
#define NORTHEAST 3
#define EAST 4
#define SOUTHEAST 5
#define SOUTH 6
#define SOUTHWEST 7


class Reversi{
public:
    player_t board[8][8];

    int moves_made = 0;
    pos_t lastPos, lastlastPos;

    Reversi(){
        setup_initial_board();
    }

    void setup_initial_board(){
        for(int i = 0; i<8; i++){
            for(int j = 0; j<8; j++)
                board[i][j] = NONE;
        }
        board[3][3] = PLAYER1;
        board[4][4] = PLAYER1;
        board[3][4] = PLAYER0;
        board[4][3] = PLAYER0;

        moves_made = 0;
        lastPos = pos_t(-1, -1);
        lastlastPos = pos_t(-1, -1);
    }

    player_t get_player(int x, int y){
        if(x < 0 || x > 7)
            return NONE;
        if(y < 0 || y > 7)
            return NONE;
        
        return board[y][x];
    }

    string draw(){
        stringstream resString;

        for(int i = 0; i<8; i++){
            for(int j = 0; j<8; j++){
                player_t p = get_player(j, i);
                if(p == NONE)
                    resString << '.';
                else if(p == PLAYER0)
                    resString << '#';
                else
                    resString << 'o';
            }
            resString << endl;
        }

        return resString.str();
    }

    vector<pos_t> moves(player_t player){

/*
        // direction bitmask
        int dp[8][8];

        vector<pos_t> positions;
        
        //cerr << "Starting" << endl;

        for(int i = 0; i<8; i++){
            for(int j = 0; j<8; j++){
                if(get_player(j, i) == player){
                    dp[i][j] = 0xFF;
                }
                else if(get_player(j, i) == OPPONENT(player)){
                    dp[i][j] = (j > 0 ? dp[i][j-1] & (1 << WEST) : 0) | 
                               (j > 0 && i > 0 ? dp[i-1][j-1] & (1 << NORTHWEST) : 0) | 
                               (i > 0 ? dp[i-1][j] & (1 << NORTH) : 0) | 
                               (i > 0 && j < 7 ? dp[i-1][j+1] & (1 << NORTHEAST) : 0) ;
                }
            }
        }

        for(int i = 7; i>=0; i--){
            for(int j = 7; j>=0; j--){
                if(get_player(j, i) == OPPONENT(player)){
                    dp[i][j] = (j < 7 ? dp[i][j+1] & (1 << EAST) : 0) | 
                               (j < 7 && i < 7 ? dp[i+1][j+1] & (1 << SOUTHEAST) : 0) | 
                               (i < 7 ? dp[i+1][j] & (1 << SOUTH) : 0) | 
                               (i < 7 && j > 0 ? dp[i+1][j-1] & (1 << SOUTHWEST) : 0) ;
                    continue;
                }

                // add all valid none spots
                if(get_player(j, i) == NONE){

                    if( (j > 0 && get_player(j-1, i) == OPPONENT(player) ? dp[i][j-1] & (1 << WEST) : 0) | 
                        (j > 0 && i > 0 && get_player(j-1, i-1) == OPPONENT(player) ? dp[i-1][j-1] & (1 << NORTHWEST) : 0) | 
                        (i > 0 && get_player(j, i-1) == OPPONENT(player) ? dp[i-1][j] & (1 << NORTH) : 0) | 
                        (i > 0 && j < 7 && get_player(j-1, i+1) == OPPONENT(player) ? dp[i-1][j+1] & (1 << NORTHEAST) : 0) |
                        (j < 7 && get_player(j+1, i) == OPPONENT(player) ? dp[i][j+1] & (1 << EAST) : 0) | 
                        (j < 7 && i < 7 && get_player(j+1, i+1) == OPPONENT(player) ? dp[i+1][j+1] & (1 << SOUTHEAST) : 0) | 
                        (i < 7 && get_player(j, i+1) == OPPONENT(player) ? dp[i+1][j] & (1 << SOUTH) : 0) | 
                        (i < 7 && j > 0 && get_player(j-1, i+1) == OPPONENT(player) ? dp[i+1][j-1] & (1 << SOUTHWEST) : 0)) {

                        positions.push_back(pos_t(j, i));
                    }
                }
            }
        }

        return positions;
*/

        vector<pos_t> res;
        for(int i = 0; i<8; i++){
            for(int j = 0; j<8; j++){

                if(get_player(j, i) != NONE)
                    continue;

                for(int dx = -1; dx<=1; dx++){
                    for(int dy = -1; dy<=1; dy++){
                        if(dx == dy && dx == 0)
                            continue;
                        
                        if(can_beat(j, i, dx, dy, player)){
                            res.push_back( pos_t(j, i) );
                            dx = 2;
                            dy = 2;
                        }
                    }
                }
            }
        }

        return res;
    }

    bool can_beat(int x, int y, int dx, int dy, player_t player){
        x += dx;
        y += dy;

        int cnt = 0;

        while(get_player(x, y) == OPPONENT(player)){
            x += dx;
            y += dy;
            cnt++;
        }

        return cnt > 0 and get_player(x, y) == player;
    }

    void do_move(pos_t move, player_t player, bool printDEBUG){
        lastlastPos = lastPos;
        lastPos = move;
        moves_made++;

        if(move.isEmpty())
            return;

        pos_t currentMove = move;
        pos_t originalMove = move;

        board[currentMove.get_y()][currentMove.get_x()] = player;

        for(auto &dir : DIRS){
            currentMove = originalMove;
            vector<pos_t> positionsToBeat;

            currentMove.set_x(currentMove.get_x() + dir.get_x());
            currentMove.set_y(currentMove.get_y() + dir.get_y());

            // if(printDEBUG)
            //     cerr << "Starting from " << originalMove.get_x() << " " << originalMove.get_y() << endl;

            while(get_player(currentMove.get_x(), currentMove.get_y()) == OPPONENT(player)){
                positionsToBeat.push_back(currentMove);
                
                // if(printDEBUG)
                //     cerr << "   In " << currentMove.get_x() << " " << currentMove.get_y() << " with value: " << (int)board[currentMove.get_y()][currentMove.get_x()] << endl;

                currentMove.set_x(currentMove.get_x() + dir.get_x());
                currentMove.set_y(currentMove.get_y() + dir.get_y());
            }

            // if(printDEBUG)       
            //     cerr << "   " << positionsToBeat.size() << endl;

            // if(printDEBUG)
            //     cerr << "   In " << currentMove.get_x() << " " << currentMove.get_y() << " with value: " << (int)board[currentMove.get_y()][currentMove.get_x()] << endl;
            if(get_player(currentMove.get_x(), currentMove.get_y()) == player){
                for(pos_t &pos : positionsToBeat){
                    board[pos.get_y()][pos.get_x()] = player;
                }
            }
        }
    }

    double heuristic_eval(player_t player){

        double p_tiles=0, p_corners=0, p_weighted=0, p_mobility=0, p_occupiedcorners;

/* tiles and weighted tiles */

        double player_tile_count = 0;
        double opponent_tile_count = 0;

        double player_tile_weighted = 0;
        double opponent_tile_weighted = 0;

        for(int i = 0; i<8; i++){
            for(int j = 0; j<8; j++){
                if(get_player(j, i) == player){
                    player_tile_count++;
                    player_tile_weighted += BOARD_WEIGHTS[i][j];
                }
                else if(get_player(j, i) == OPPONENT(player)){
                    opponent_tile_count++;
                    opponent_tile_weighted += BOARD_WEIGHTS[i][j];
                }
            }
        }

        if(player_tile_count > opponent_tile_count)
            p_tiles = 100.0 * (player_tile_count) / (player_tile_count + opponent_tile_count);
        else if(opponent_tile_count > player_tile_count)
            p_tiles = -100.0 * (opponent_tile_count) / (player_tile_count + opponent_tile_count);
    
        p_weighted = player_tile_weighted - opponent_tile_weighted;
/* mobility */

        double player_move_count = moves(player).size();
        double opponent_move_count = moves(OPPONENT(player)).size();

        if(player_move_count > opponent_move_count)
            p_mobility = 100.0 * (player_move_count) / (player_move_count + opponent_move_count);
        else if(opponent_move_count > player_move_count)
            p_mobility = -100.0 * (opponent_move_count) / (player_move_count + opponent_move_count);
    
/* occupied corners */

        double player_corners = (board[0][0] == player) +
                                (board[0][7] == player) +
                                (board[7][0] == player) +
                                (board[7][7] == player);
        
        double opponent_corners = (board[0][0] == OPPONENT(player)) +
                                  (board[0][7] == OPPONENT(player)) +
                                  (board[7][0] == OPPONENT(player)) +
                                  (board[7][7] == OPPONENT(player));
        
        p_occupiedcorners = 25.0 * (player_corners - opponent_corners);
        
/* close corners */
        double player_closeCorners = 0;
        double opponent_closeCorners = 0;

        for(auto &corner : CLOSE_CORNERS){
            if(board[corner.get_y()][corner.get_x()] == player)
                player_closeCorners++;
            else if(board[corner.get_y()][corner.get_x()] == OPPONENT(player))
                opponent_closeCorners++;
        }

        p_corners = -12.5 * (player_closeCorners - opponent_closeCorners);

        return 350*p_tiles + 801.724 * p_occupiedcorners + 382.026 * p_corners + 78.922 * p_mobility + 200 * p_weighted;
    }

    bool terminal(){
        if(moves_made >= 64)
            return true;

        if(moves_made < 2)
            return false;
        
        return lastPos.isEmpty() && lastlastPos.isEmpty();
    }

    bool willWin(player_t player){
        int res = 0;

        for(int i = 0; i<8; i++){
            for(int j = 0; j<8; j++){
                if(board[i][j] == OPPONENT(player))
                    return false;
            }
        }
    }

};

class Player{
public:
    Reversi current_game;
    int my_player;

    void reset(){
        current_game = Reversi();
        my_player = 1;
        say("RDY");

    }

    void say(string message){
        cout << message;
        cout << '\n';
        cout.flush();
    }

    pair<string, vector<string>> hear(){
        string line;


        cin >> line;
        cin.ignore();

        //cerr << line << endl;

        size_t pos = 0;
        std::string token;

        string cmd;
        vector<string> args;

        while ((pos = line.find(" ")) != std::string::npos) {
            token = line.substr(0, pos);
            args.push_back(token);
            line.erase(0, pos + string(" ").length());

            //cerr << token << endl;
        }

        cmd = args[0];
        args.erase(args.begin());

        return {cmd, args};
    }

    static pair<double, pos_t> min_max(Reversi currentBoard, int depth, bool maximizingPlayer, player_t currentPlayer, player_t originalPlayer, double alpha, double beta){

        //cerr << currentBoard.draw() << endl;
        //cerr << "Heura value (depth " << depth << ") : " << currentBoard.heuristic_eval(originalPlayer) << endl;


        if(depth == 0 || currentBoard.terminal()){
            return { currentBoard.heuristic_eval(originalPlayer), pos_t() }; 
        }

        if(maximizingPlayer){
            double maxEval = -DBL_MAX;

            auto moves = currentBoard.moves(currentPlayer);
            
            pos_t moveToMake;

            for(auto &move : moves){
                Reversi newGame = currentBoard;
                newGame.do_move(move, currentPlayer, false);

                auto res = Player::min_max(newGame, depth - 1, !maximizingPlayer, OPPONENT(currentPlayer), originalPlayer, alpha, beta);

                maxEval = max(maxEval, res.first);

                if(maxEval == res.first)
                    moveToMake = move;

                alpha = max(maxEval, alpha);

                if(beta <= alpha)
                    break;
            }

            //cerr << "At depth " << depth << " picked max " << maxEval << endl;

            return { maxEval, moveToMake };
        }
        else{
            double minEval = DBL_MAX;
            pos_t moveToMake;

            auto moves = currentBoard.moves(currentPlayer);

            for(auto &move : moves){
                Reversi newGame = currentBoard;
                newGame.do_move(move, currentPlayer, false);

                auto res = Player::min_max(newGame, depth - 1, !maximizingPlayer, OPPONENT(currentPlayer), originalPlayer, alpha, beta);

                minEval = min(minEval, res.first);

                if(minEval == res.first)
                    moveToMake = move;

                beta = min(minEval, beta);

                if(beta <= alpha)
                    break;
            }
            //cerr << "At depth " << depth << " picked " << minEval << endl;

            return { minEval, moveToMake };
        }

    }

    void loop2(){
        srand(time(NULL));

        reset();

        while(1)
        {
            float turnTime;

            string command; 
            cin >> command; 
            cin.ignore();

            chrono::_V2::system_clock::time_point  start_time;

            if(command == "BYE")
                return;
            else if(command == "ONEMORE"){
                reset();
                continue;
            } 
            else if(command == "HEDID") {   
                float time1, time2; 
                cin >> time1 >> time2;
                turnTime = time1*100;

                string x,y; cin >> x >> y;

                //cerr << "He did " << x << " " << y << endl;

                if(x[0] != '-') {
                    int x0 = x[0] - '0';
                    int y0 = y[0] - '0';

                    current_game.do_move(pos_t(x0, y0), OPPONENT(my_player), true); 
                } 
                else 
                    current_game.do_move(pos_t(-1, -1), OPPONENT(my_player), true);

            }
            else{
                //assert(command == "UGO");

                string x,y; 
                cin >> x >> y;

                my_player = PLAYER0;
            }

            //cerr << "I'm player: " << (int)my_player << endl;

            //cerr << current_game.draw() << endl;

            start_time = chrono::high_resolution_clock::now();

            auto moves = current_game.moves(my_player);

            pos_t move_to_make;

            //cerr << "-------- MINMAX -------------" << endl;
            move_to_make = Player::min_max(current_game, 5, true, my_player, my_player, -DBL_MAX, DBL_MAX).second;

            current_game.do_move(move_to_make, my_player, false);

            //cerr << "After my move " << move_to_make.get_x() << " " << move_to_make.get_y() << endl;
            //cerr << current_game.draw() << endl;

            cout << "IDO ";
            cout << move_to_make.get_x() << " " << move_to_make.get_y() << endl;

        }

    }
};

int main(){

    Player pl;

    pl.loop2();
}
