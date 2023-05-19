#include <bits/stdc++.h>

using namespace std;

typedef char player_t;

typedef uint64_t ll;
typedef uint64_t board_t;

typedef short unsigned int sui;

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

board_t SHIFT_MASKS[] = {
    0xFEFEFEFEFEFEFEFE,
    0xFEFEFEFEFEFEFE00,
    0xFFFFFFFFFFFFFF00,
    0x7F7F7F7F7F7F7F00,
    0x7F7F7F7F7F7F7F7F,
    0x007F7F7F7F7F7F7F,
    0x00FFFFFFFFFFFFFF,
    0x00FEFEFEFEFEFEFE
};

sui SHIFT_AMOUNTS[] = {
    1,
    9,
    8,
    7,
    1,
    9,
    8,
    7
};


typedef char direction_t;


class Reversi{
public:
    board_t playerPieces[2];

    int moves_made = 0;
    pos_t lastPos, lastlastPos;

    Reversi(){
        setup_initial_board();
    }

    void setup_initial_board(){
        playerPieces[PLAYER0] = 0x0000000810000000;
        playerPieces[PLAYER1] = 0x0000001008000000;
        
        moves_made = 0;
        lastPos = pos_t(-1, -1);
        lastlastPos = pos_t(-1, -1);
    }

    player_t get_player(int x, int y){
        if(playerPieces[PLAYER0] & ((ll)1 << (y * 8 + x)))
            return PLAYER0;
        if(playerPieces[PLAYER1] & ((ll)1 << (y * 8 + x)))
            return PLAYER1;
        return NONE;
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

    string drawBoard(board_t board){
        stringstream resString;

        for(int i = 0; i<8; i++){
            for(int j = 0; j<8; j++){
                if(!is_one(board, j, i))
                    resString << '.';
                else
                    resString << '#';
            }
            resString << endl;
        }

        return resString.str();
    }

    board_t LeftShift(board_t board, direction_t dir){
        return (board << SHIFT_AMOUNTS[dir]) & SHIFT_MASKS[dir];
    }

    board_t RightShift(board_t board, direction_t dir){
        return (board >> SHIFT_AMOUNTS[dir]) & SHIFT_MASKS[dir];
    }

    board_t Shift(board_t board, direction_t dir){
        if(dir <= NORTHEAST)
            return LeftShift(board, dir);
        
        return RightShift(board, dir);
    }

    board_t free_spaces(){
        return ~(playerPieces[PLAYER0] | playerPieces[PLAYER1]);
    }

    bool is_one(board_t board, int x, int y){
        return board & ((ll)1 << (8*y + x));
    }

    void set_one(board_t &board, int x, int y){
        board |= ((ll)1 << (8*y + x));
    }

    /*
    We will be doing shifts 
    */
    void moves(player_t player, vector<pos_t> &moves){
        board_t filled = 0;

        board_t freespaces = 0;

        for(direction_t dir = 0; dir < 8; dir++){

            board_t tmpPlayer = playerPieces[player];

            filled = playerPieces[player];
            filled = (Shift(filled, dir) & playerPieces[OPPONENT(player)]);

            for(int i = 0; i<5; i++){
                filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);
            }

            // now we check the spaces that are good

            freespaces |= free_spaces() & Shift(filled, dir);
        }

        for(int i = 0; i<8; i++){
            for(int j = 0; j<8; j++){
                if(is_one(freespaces, j, i))
                    moves.push_back(pos_t(j, i));
            }
        }

        // cerr << moves.size() << endl;
        // for(auto &move : moves){
        //     cerr << "Move " << move.get_x() << " " << move.get_y() << endl;
        // }

        // cerr << drawBoard(freespaces) << endl;
    }

    int count_moves(player_t player){
        board_t filled = 0;

        board_t freespaces = 0;

        for(direction_t dir = 0; dir < 8; dir++){

            board_t tmpPlayer = playerPieces[player];

            filled = playerPieces[player];

            filled = (Shift(filled, dir) & playerPieces[OPPONENT(player)]);

            for(int i = 0; i<5; i++){
                filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);
            }

            // now we check the spaces that are good

            freespaces |= (~filled) & Shift(filled, dir);
        }

        int cnt = 0;
        for(int i = 0; i<8; i++){
            for(int j = 0; j<8; j++){
                if(is_one(freespaces, j, i))
                    cnt++;
            }
        }

        return cnt;
    }

    void do_move(pos_t move, player_t player, bool printDEBUG){
        lastlastPos = lastPos;
        lastPos = move;
        moves_made++;

        if(move.isEmpty())
            return;

        pos_t currentMove = move;
        pos_t originalMove = move;

        set_one(playerPieces[player], move.get_x(), move.get_y());

        board_t filled = 0;
        board_t toFill = 0;

        for(direction_t dir = 0; dir < 8; dir++){

            board_t tmpPlayer = 0;
            set_one(tmpPlayer, move.get_x(), move.get_y());

            filled = tmpPlayer;

            // cerr << "Printing boards:" << endl;
            // cerr << drawBoard(filled) << endl;
            // cerr << endl;
            // cerr << drawBoard(playerPieces[OPPONENT(player)]);

            filled = (Shift(filled, dir) & playerPieces[OPPONENT(player)]);

            for(int i = 0; i<5; i++){
                filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);
            }

            // cerr << "direction " << (int)dir << endl;
            // cerr << drawBoard(filled) << endl;

            // now we check the spaces that are good

            if(Shift(filled, dir) & playerPieces[player])
                toFill |= filled;
        }

        // cerr << "moving:\n";
        // cerr << drawBoard(toFill) << endl;

        playerPieces[player] |= toFill;
        playerPieces[OPPONENT(player)] = (playerPieces[OPPONENT(player)] | toFill) & (~toFill);
    }

    float heuristic_eval(player_t player){

        float p_tiles=0, p_corners=0, p_weighted=0, p_mobility=0, p_occupiedcorners;

/* tiles and weighted tiles */

        float player_tile_count = 0;
        float opponent_tile_count = 0;

        float player_tile_weighted = 0;
        float opponent_tile_weighted = 0;

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

        float player_move_count = count_moves(player);
        float opponent_move_count = count_moves(OPPONENT(player));

        if(player_move_count > opponent_move_count)
            p_mobility = 100.0 * (player_move_count) / (player_move_count + opponent_move_count);
        else if(opponent_move_count > player_move_count)
            p_mobility = -100.0 * (opponent_move_count) / (player_move_count + opponent_move_count);
    
/* occupied corners */

        float player_corners = (get_player(0, 0) == player) +
                                (get_player(0, 7) == player) +
                                (get_player(7, 0) == player) +
                                (get_player(7, 7) == player);
        
        float opponent_corners = (get_player(0, 0) == OPPONENT(player)) +
                                  (get_player(0, 7) == OPPONENT(player)) +
                                  (get_player(7, 0) == OPPONENT(player)) +
                                  (get_player(7, 7) == OPPONENT(player));
        
        p_occupiedcorners = 25.0 * (player_corners - opponent_corners);
        
/* close corners */
        float player_closeCorners = 0;
        float opponent_closeCorners = 0;

        for(auto &corner : CLOSE_CORNERS){
            if(get_player(corner.get_x(), corner.get_y()) == player)
                player_closeCorners++;
            else if(get_player(corner.get_x(), corner.get_y()) == OPPONENT(player))
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

    static pair<float, pos_t> min_max(Reversi currentBoard, int depth, player_t currentPlayer, player_t originalPlayer, float alpha, float beta){
        if(depth == 0 || currentBoard.terminal()){
            return { currentBoard.heuristic_eval(originalPlayer), pos_t() }; 
        }

        if(currentPlayer == originalPlayer){
            float maxEval = -FLT_MAX;

            vector<pos_t> moves;
            currentBoard.moves(currentPlayer, moves);
            
            pos_t moveToMake;

            for(auto &move : moves){
                Reversi newGame = currentBoard;
                newGame.do_move(move, currentPlayer, false);

                auto res = Player::min_max(newGame, depth - 1, OPPONENT(currentPlayer), originalPlayer, alpha, beta);

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
            float minEval = FLT_MAX;
            pos_t moveToMake;

            vector<pos_t> moves;
            currentBoard.moves(currentPlayer, moves);
            
            for(auto &move : moves){
                Reversi newGame = currentBoard;
                newGame.do_move(move, currentPlayer, false);

                auto res = Player::min_max(newGame, depth - 1, OPPONENT(currentPlayer), originalPlayer, alpha, beta);

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

            pos_t move_to_make;

            //cerr << "-------- MINMAX -------------" << endl;
            move_to_make = Player::min_max(current_game, 4, my_player, my_player, -FLT_MAX, FLT_MAX).second;

            current_game.do_move(move_to_make, my_player, false);

            // cerr << "After my move " << move_to_make.get_x() << " " << move_to_make.get_y() << endl;
            // cerr << current_game.draw() << endl;

            cout << "IDO ";
            cout << move_to_make.get_x() << " " << move_to_make.get_y() << endl;

        }

    }
};

int main(){

    Player pl;

    pl.loop2();
}
