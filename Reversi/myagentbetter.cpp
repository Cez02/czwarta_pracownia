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

#define CHECK_BIT(mask, pos) (mask & ((ll)1 << pos))

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
    unsigned char data;

    pos_t(int x, int y){
        if(x == -1)
            data = 0xFF;
        else
            data = (y<<3) + x;
    }

    pos_t()
        : pos_t(-1, -1) {}

    bool isEmpty(){
        return data == 0xFF;
    }

    void set_x(int x){
        if(x == -1){
            data = 0xff;
            return;
        }
        data = (get_y()<<3 + x);
    }

    void set_y(int y){
        if(y == -1){
            data = 0xff;
            return;
        }
        data = (y<<3 + get_x());
    }

    int get_x() const{
        if(data == 0xFF)
            return -1;
        return data & 0x7;
    }

    int get_y() const{
        if(data == 0xFF)
            return -1;
        return data >> 3;
    }

    friend bool operator==(const pos_t &x, const pos_t &y)
    {
        return x.get_x() == y.get_x() && x.get_y() == y.get_y();
    }
};

pos_t DIRS[] = {
    pos_t(-1, -1),
    pos_t(-1, 0),
    pos_t(-1, 1),
    pos_t(0, 1),
    pos_t(1, 1),
    pos_t(1, 0),
    pos_t(1, -1),
    pos_t(0, -1)
};

pos_t CLOSE_CORNERS[] = {
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
    sui moves_made;

    Reversi(){
        setup_initial_board();
    }

    void setup_initial_board(){
        playerPieces[PLAYER0] = 0x0000000810000000;
        playerPieces[PLAYER1] = 0x0000001008000000;
        moves_made = 0;
    }

    player_t get_player(int x, int y){
        if(playerPieces[PLAYER0] & ((ll)1 << ((y<<3) + x)))
            return PLAYER0;
        if(playerPieces[PLAYER1] & ((ll)1 << ((y<<3) + x)))
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
        return board & ((ll)1 << ((y<<3) + x));
    }

    void set_one(board_t &board, int x, int y){
        board |= ((ll)1 << ((y<<3) + x));
    }

    board_t moves(player_t player){
        board_t filled = 0;
        board_t freespaces = 0;

        for(direction_t dir = 0; dir < 8; dir++){
            filled = playerPieces[player];
            filled = (Shift(filled, dir) & playerPieces[OPPONENT(player)]);

            filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);
            filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);
            filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);
            filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);
            filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);

            // now we check the spaces that are good

            freespaces |= free_spaces() & Shift(filled, dir);
        }

        return freespaces;
    }

    int count_moves(player_t player){
        board_t filled = 0;

        board_t freespaces = 0;

        for(direction_t dir = 0; dir < 8; dir++){
            filled = playerPieces[player];

            filled = (Shift(filled, dir) & playerPieces[OPPONENT(player)]);

            filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);
            filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);
            filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);
            filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);
            filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);

            // now we check the spaces that are good

            freespaces |= (~filled) & Shift(filled, dir);
        }

        return __popcount(freespaces);
    }

    void do_move(pos_t move, player_t player, bool printDEBUG){
        moves_made++;

        if(move.isEmpty())
            return;

        set_one(playerPieces[player], move.get_x(), move.get_y());

        board_t filled = 0;
        board_t toFill = 0;

        for(direction_t dir = 0; dir < 8; dir++){
            filled = 0;
            set_one(filled, move.get_x(), move.get_y());

            filled = (Shift(filled, dir) & playerPieces[OPPONENT(player)]);

            filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);
            filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);
            filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);
            filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);
            filled |= (Shift(filled, dir) & playerPieces[OPPONENT(player)]);


            // now we check the spaces that are good

            if(Shift(filled, dir) & playerPieces[player])
                toFill |= filled;
        }

        playerPieces[player] |= toFill;
        playerPieces[OPPONENT(player)] = (playerPieces[OPPONENT(player)] | toFill) & (~toFill);
    }

    float heuristic_eval(player_t player){

        float p_tiles=0, p_corners=0, p_weighted=0, p_mobility=0, p_occupiedcorners=0, p_fronttiles=0;

/* tiles and weighted tiles */

        float player_tile_count = 0;
        float opponent_tile_count = 0;

        float player_tile_weighted = 0;
        float opponent_tile_weighted = 0;

        float player_front_tiles = 0;
        float opponent_front_tiles = 0;

        int boardInd = 0;
        int i, j;
        while(boardInd < 64){
            i = boardInd >> 3;
            j = boardInd & (ll)7;

            if(get_player(j, i) == player){
                player_tile_count++;
                player_tile_weighted += BOARD_WEIGHTS[i][j];

                for(auto &dir : DIRS){
                    if(get_player(j + dir.get_x(), i + dir.get_y()) == NONE)
                        player_front_tiles++;
                }
            }
            else if(get_player(j, i) == OPPONENT(player)){
                opponent_tile_count++;
                opponent_tile_weighted += BOARD_WEIGHTS[i][j];

                for(auto &dir : DIRS){
                    if(get_player(j + dir.get_x(), i + dir.get_y()) == NONE)
                        opponent_front_tiles++;
                }
            }
            boardInd++;
        }

        if(player_tile_count > opponent_tile_count)
            p_tiles = 100.0 * (player_tile_count) / (player_tile_count + opponent_tile_count);
        else if(opponent_tile_count > player_tile_count)
            p_tiles = -100.0 * (opponent_tile_count) / (player_tile_count + opponent_tile_count);
    
        if(player_front_tiles > opponent_front_tiles)
            p_fronttiles = 100.0 * (player_front_tiles) / (player_front_tiles + opponent_front_tiles);
        else if(opponent_front_tiles > player_front_tiles)
            p_fronttiles = -100.0 * (opponent_front_tiles) / (player_front_tiles + opponent_front_tiles);

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

        return 10 * p_tiles + 801.724 * p_occupiedcorners + 78.922 * p_mobility + 382.026 * p_corners + 230 * p_weighted - 74.396 * p_fronttiles;
    }

    player_t terminal(player_t player){
        if(moves_made < 2)
            return NONE;

        if(count_moves(PLAYER0) == 0 && count_moves(PLAYER1) == 0){
            return __popcount(playerPieces[player]) > __popcount(playerPieces[OPPONENT(player)]) ? player : OPPONENT(player); 
        }

        return NONE;
    }

};

int my_player;

class Player{
public:
    Reversi current_game;

    void reset(){
        current_game = Reversi();
        my_player = 1;
        cout << "RDY" << endl;
    }


    static float min_max(Reversi currentBoard, int depth, player_t currentPlayer, float alpha, float beta){
        if(depth == 0){
            return currentBoard.heuristic_eval(my_player); 
        }

        player_t winner;
        if((winner = currentBoard.terminal(my_player)) != NONE){
            return winner == my_player ? FLT_MAX : -FLT_MAX;
        }

        if(currentPlayer == my_player){
            float maxEval = -FLT_MAX;

            auto moves = currentBoard.moves(currentPlayer);
            int moveind = 0;
            while(moves){
                while((moves & (ll)1) == 0){
                    moves >>= 1;
                    moveind++;
                }

                auto move = pos_t(moveind & (ll)7, moveind >> (3));
                moveind++;
                moves >>=1;

                Reversi newGame = currentBoard;
                newGame.do_move(move, currentPlayer, false);

                auto res = Player::min_max(newGame, depth - 1, OPPONENT(currentPlayer), alpha, beta);

                maxEval = max(maxEval, res);

                alpha = max(maxEval, alpha);

                if(beta <= alpha)
                    break;
            }
            

            //cerr << "At depth " << depth << " picked max " << maxEval << endl;

            return maxEval;
        }
        else{
            float minEval = FLT_MAX;

            auto moves = currentBoard.moves(currentPlayer);
            int moveind = 0;
            while(moves){
                while((moves & (ll)1) == 0){
                    moves >>= 1;
                    moveind++;
                }

                auto move = pos_t(moveind & (ll)7, moveind >> (3));
                moveind++;
                moves >>=1;

                Reversi newGame = currentBoard;
                newGame.do_move(move, currentPlayer, false);

                auto res = Player::min_max(newGame, depth - 1, OPPONENT(currentPlayer), alpha, beta);

                minEval = min(minEval, res);

                beta = min(minEval, beta);

                if(beta <= alpha)
                    break;
            }
            
            //cerr << "At depth " << depth << " picked " << minEval << endl;

            return minEval;
        }
    }

    static pos_t pickmove(Reversi currentBoard, int depth, player_t currentPlayer, float alpha, float beta){
        float maxEval = -FLT_MAX;
        pos_t moveToMake;

        auto moves = currentBoard.moves(currentPlayer);

        int moveind = 0;
        while(moves){
            while((moves & (ll)1) == 0){
                moves >>= 1;
                moveind++;
            }

            auto move = pos_t(moveind & (ll)7, moveind >> (3));
            moveind++;
            moves >>=1;

            //cout << "Available move: " << move.get_x() << " " << move.get_y() << endl;

            Reversi newGame = currentBoard;
            newGame.do_move(move, currentPlayer, false);

            auto res = Player::min_max(newGame, depth - 1, OPPONENT(currentPlayer), alpha, beta);

            maxEval = max(maxEval, res);

            if(maxEval == res)
                moveToMake = move;

            alpha = max(maxEval, alpha);

            if(beta <= alpha)
                break;
        }

        return moveToMake;
    }

    void loop(){
        srand(time(NULL));

        reset();


        while(1)
        {
            float turnTime;

            string command; 
            cin >> command; 
            cin.ignore();

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

            pos_t move_to_make;

            //cerr << "-------- MINMAX -------------" << endl;
            move_to_make = Player::pickmove(current_game, 4, my_player, -FLT_MAX, FLT_MAX);

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

    pl.loop();
}
