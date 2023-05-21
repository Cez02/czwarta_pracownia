#include <bits/stdc++.h>

using namespace std;

//#define DEBUG

/* macros to help */
typedef char player_t;

#define NONE -1
#define PLAYER0 0
#define PLAYER1 1

#define OPPONENT(player) (1 - player)

/* field type */
typedef char fieldtype_t;

#define FIELD 0
#define TRAP 1
#define HOLE 2
#define LAKE 3

/* figure type */
typedef char figure_t;

#define RAT 0
#define CAT 1
#define DOG 2
#define WOLF 3
#define JAGUAR 4
#define TIGER 5
#define LION 6
#define ELEPHANT 7


fieldtype_t MAP[9][7];

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

    friend bool operator!=(const pos_t &x, const pos_t &y)
    {
        return x.get_x() != y.get_x() || x.get_y() != y.get_y();
    }

    friend pos_t operator+(const pos_t &x, const pos_t &y)
    {
        return pos_t(x.get_x() + y.get_x(), x.get_y() + y.get_y());
    }
};

#define NULL_POS pos_t(-1, -1)

/* directions */

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

pos_t DIR_OFFSET[] = {
    pos_t(0, -1),
    pos_t(-1, 0),
    pos_t(0, 1),
    pos_t(1, 0)
};

struct figurepositions_t{
public:
    pos_t figures[8];
};

struct move_t {
    figure_t figure;
    pos_t where;
    figure_t opponentFigureToDestroy;

    move_t(figure_t figure_, pos_t where_, figure_t opponentFigureToDestroy_)
        : figure(figure_)
        , where(where_)
        , opponentFigureToDestroy(opponentFigureToDestroy_) { } 

    move_t()
        : move_t(NONE, NULL_POS, NONE) { }
};

move_t MOVE_BUFFER[1024];
move_t MOVE_BUFFER_1[1024];

class JungleGame{
public:
    figurepositions_t figurePositions[2];

    player_t playerWhoWon;

    void reset(){
        figurePositions[1].figures[RAT] = pos_t(0, 2);
        figurePositions[1].figures[CAT] = pos_t(5, 1);
        figurePositions[1].figures[DOG] = pos_t(1, 1);
        figurePositions[1].figures[WOLF] = pos_t(4, 2);
        figurePositions[1].figures[JAGUAR] = pos_t(2, 2);
        figurePositions[1].figures[TIGER] = pos_t(6, 0);
        figurePositions[1].figures[LION] = pos_t(0, 0);
        figurePositions[1].figures[ELEPHANT] = pos_t(6, 2);

        figurePositions[0].figures[RAT] = pos_t(6, 6);
        figurePositions[0].figures[CAT] = pos_t(1, 7);
        figurePositions[0].figures[DOG] = pos_t(5, 7);
        figurePositions[0].figures[WOLF] = pos_t(2, 6);
        figurePositions[0].figures[JAGUAR] = pos_t(4, 6);
        figurePositions[0].figures[TIGER] = pos_t(0, 8);
        figurePositions[0].figures[LION] = pos_t(6, 8);
        figurePositions[0].figures[ELEPHANT] = pos_t(0, 6);

        playerWhoWon = NONE;
    }

    fieldtype_t get_field_type(int x, int y){
        if(x < 0 || x > 6 || y < 0 || y > 8)
            return NONE;
        return MAP[y][x];
    }

    fieldtype_t get_field_type(pos_t pos){
        return get_field_type(pos.get_x(), pos.get_y());
    }

    string draw(){
        stringstream sstream;

        for(int i = 0; i<9; i++){
            for(int j = 0; j<7; j++){
                pos_t pos(j, i);

                if(figurePositions[PLAYER0].figures[RAT] == pos)
                    sstream << 'R';
                else if(figurePositions[PLAYER0].figures[CAT] == pos)
                    sstream << 'C';
                else if(figurePositions[PLAYER0].figures[DOG] == pos)
                    sstream << 'D';
                else if(figurePositions[PLAYER0].figures[WOLF] == pos)
                    sstream << 'W';
                else if(figurePositions[PLAYER0].figures[JAGUAR] == pos)
                    sstream << 'J';
                else if(figurePositions[PLAYER0].figures[TIGER] == pos)
                    sstream << 'T';
                else if(figurePositions[PLAYER0].figures[LION] == pos)
                    sstream << 'L';
                else if(figurePositions[PLAYER0].figures[ELEPHANT] == pos)
                    sstream << 'E';

                else if(figurePositions[PLAYER1].figures[RAT] == pos)
                    sstream << 'r';
                else if(figurePositions[PLAYER1].figures[CAT] == pos)
                    sstream << 'c';
                else if(figurePositions[PLAYER1].figures[DOG] == pos)
                    sstream << 'd';
                else if(figurePositions[PLAYER1].figures[WOLF] == pos)
                    sstream << 'w';
                else if(figurePositions[PLAYER1].figures[JAGUAR] == pos)
                    sstream << 'j';
                else if(figurePositions[PLAYER1].figures[TIGER] == pos)
                    sstream << 't';
                else if(figurePositions[PLAYER1].figures[LION] == pos)
                    sstream << 'l';
                else if(figurePositions[PLAYER1].figures[ELEPHANT] == pos)
                    sstream << 'e';

                else if(MAP[i][j] == FIELD)
                    sstream << '.';
                else if(MAP[i][j] == TRAP)
                    sstream << '#';
                else if(MAP[i][j] == HOLE)
                    sstream << '*';
                else if(MAP[i][j] == LAKE)
                    sstream << '~';
            }
            sstream << "\n";
        }

        return sstream.str();
    }

    bool check_potential_move(figure_t figure, player_t player, pos_t newpos, figure_t &figureToDestroy){
        figureToDestroy = NONE;
        pos_t originalPos = figurePositions[player].figures[figure];

        if(get_field_type(newpos.get_x(), newpos.get_y()) == NONE)
            return false;

        // check if figure can go over given field
        if(figure != RAT && figure != LION && figure != ELEPHANT && get_field_type(newpos.get_x(), newpos.get_y()) == LAKE)
            return false;


        // check if figure will overlap friendly creatures

        for(figure_t friendlyfigure = RAT; friendlyfigure <= ELEPHANT; friendlyfigure++){
            if(friendlyfigure == figure)
                continue;
            
            if(figurePositions[player].figures[friendlyfigure] == newpos)
                return false;
        }

        // check if figure will destroy opponents figure
        for(figure_t opponentfigure = RAT; opponentfigure <= ELEPHANT; opponentfigure++){
            
            if(figurePositions[OPPONENT(player)].figures[opponentfigure] == newpos){
                
                if(get_field_type(originalPos) == TRAP)
                    return false;

                if(get_field_type(newpos.get_y(), newpos.get_x())== TRAP){
                    figureToDestroy = opponentfigure;
                    return true;
                }

                if(figure == ELEPHANT && opponentfigure == RAT)
                    continue;

                if(opponentfigure <= figure || (opponentfigure == ELEPHANT && figure == RAT && get_field_type(originalPos) != LAKE)){
                    figureToDestroy = opponentfigure;
                    return true;
                }
            }
        }

        return true;

    }

    // fills the buffer and returns the number of moves generated
    int moves(move_t moveBuffer[], player_t player){
        int currentMove = 0;
        figure_t figureToDestroy;

        for(figure_t figure = RAT; figure <= ELEPHANT; figure++){

            // figure has been destroyed
            if(figurePositions[player].figures[figure].isEmpty())
                continue;

            // check standard moves
            for(int dir = NORTH; dir <= WEST; dir++){
                pos_t newpos = figurePositions[player].figures[figure] + DIR_OFFSET[dir];

                if(check_potential_move(figure, player, newpos, figureToDestroy)){
                    moveBuffer[currentMove] = move_t(figure, newpos, figureToDestroy);
                    currentMove++;
                }
            }

            // check going over water for big cats
            if(figure == TIGER || figure == LION){

                /* north */
                int dir = NORTH;
                pos_t newpos = figurePositions[player].figures[figure] + DIR_OFFSET[dir];
                bool reachedLand = true;

                for(int i = 0; i<3; i++){
                    if(get_field_type(newpos.get_y(), newpos.get_x()) == LAKE && figurePositions[OPPONENT(player)].figures[RAT] != newpos) {
                        newpos = newpos + DIR_OFFSET[dir];
                    }
                    else{
                        reachedLand = false;
                        break;
                    }
                }

                if(reachedLand){
                    if(check_potential_move(figure, player, figurePositions[player].figures[figure] + DIR_OFFSET[dir], figureToDestroy)){
                        moveBuffer[currentMove] = move_t(figure, newpos, figureToDestroy);
                        currentMove++;
                    }
                }

                /* south */
                dir = SOUTH;
                newpos = figurePositions[player].figures[figure] + DIR_OFFSET[dir];
                reachedLand = true;

                for(int i = 0; i<3; i++){
                    if(get_field_type(newpos.get_y(), newpos.get_x()) == LAKE && figurePositions[OPPONENT(player)].figures[RAT] != newpos) {
                        newpos = newpos + DIR_OFFSET[dir];
                    }
                    else{
                        reachedLand = false;
                        break;
                    }
                }

                if(reachedLand){
                    if(check_potential_move(figure, player, figurePositions[player].figures[figure] + DIR_OFFSET[dir], figureToDestroy)){
                        moveBuffer[currentMove] = move_t(figure, newpos, figureToDestroy);
                        currentMove++;
                    }
                }


                /* west - horizontal require two over-water moves */
                dir = WEST;
                newpos = figurePositions[player].figures[figure] + DIR_OFFSET[dir];
                reachedLand = true;

                for(int i = 0; i<2; i++){
                    if(get_field_type(newpos.get_y(), newpos.get_x()) == LAKE && figurePositions[OPPONENT(player)].figures[RAT] != newpos) {
                        newpos = newpos + DIR_OFFSET[dir];
                    }
                    else{
                        reachedLand = false;
                        break;
                    }
                }

                if(reachedLand){
                    if(check_potential_move(figure, player, figurePositions[player].figures[figure] + DIR_OFFSET[dir], figureToDestroy)){
                        moveBuffer[currentMove] = move_t(figure, newpos, figureToDestroy);
                        currentMove++;
                    }
                }

                /* east - horizontal require two over-water moves */
                dir = EAST;
                newpos = figurePositions[player].figures[figure] + DIR_OFFSET[dir];
                reachedLand = true;

                for(int i = 0; i<2; i++){
                    if(get_field_type(newpos.get_y(), newpos.get_x()) == LAKE && figurePositions[OPPONENT(player)].figures[RAT] != newpos) {
                        newpos = newpos + DIR_OFFSET[dir];
                    }
                    else{
                        reachedLand = false;
                        break;
                    }
                }

                if(reachedLand){
                    if(check_potential_move(figure, player, figurePositions[player].figures[figure] + DIR_OFFSET[dir], figureToDestroy)){
                        moveBuffer[currentMove] = move_t(figure, newpos, figureToDestroy);
                        currentMove++;
                    }
                }

            }
        }

        return currentMove;
    }

    void do_move(move_t move, player_t player){
        if(move.where.isEmpty())
            return;

        figurePositions[player].figures[move.figure] = move.where;
        
        if(move.opponentFigureToDestroy != NONE)
            figurePositions[OPPONENT(player)].figures[move.opponentFigureToDestroy] = NULL_POS;

        if(player == PLAYER0 && move.where == pos_t(3, 0))
            playerWhoWon = PLAYER0;
        else if(player == PLAYER1 && move.where == pos_t(3, 8))
            playerWhoWon = PLAYER1;
    }

    player_t game_ended(){
        return playerWhoWon;
    }
};

void parsemap(string text, int line){
    for(int i = 0; i<7; i++){
        if(text[i] == '.')
            MAP[line][i] = FIELD;
        if(text[i] == '#')
            MAP[line][i] = TRAP;
        if(text[i] == '*')
            MAP[line][i] = HOLE;
        if(text[i] == '~')
            MAP[line][i] = LAKE;
    }
}

player_t myPlayer;

class Player{
public:
    JungleGame currentGame;

    void reset(){
        currentGame.reset();
        cout << "RDY" << endl;
        myPlayer = PLAYER1; 
    }

    move_t pick_random_move(JungleGame &originalGame, player_t player){
        int movesCnt = currentGame.moves(MOVE_BUFFER, player);

        if(movesCnt){
            return MOVE_BUFFER[rand() % movesCnt];
        }

        return move_t();
    }

    float play_random_games(const JungleGame &originalGame, move_t firstMove, player_t originalPlayer, int moves){

        JungleGame currentGame = originalGame;
        currentGame.do_move(firstMove, originalPlayer);

        player_t currentPlayer = OPPONENT(originalPlayer);

        float playerWins=0, oppWins=0;

        while(moves){
            
            move_t moveToMake = pick_random_move(currentGame, currentPlayer);
            currentGame.do_move(moveToMake, currentPlayer);

            if(currentGame.game_ended() == OPPONENT(myPlayer)){
                oppWins++;
                currentGame = originalGame;
                currentGame.do_move(firstMove, originalPlayer);
                currentPlayer = OPPONENT(originalPlayer);
            }
            else if (currentGame.game_ended() == myPlayer){
                playerWins++;
                currentGame = originalGame;
                currentGame.do_move(firstMove, originalPlayer);
                currentPlayer = OPPONENT(originalPlayer);
            }
            else{
                currentPlayer = OPPONENT(currentPlayer);
            }

            moves--;
        }

        if(playerWins > oppWins)
            return 100.0 * playerWins / (playerWins + oppWins);
        else if(oppWins > playerWins)
            return -100.0 * oppWins / (playerWins + oppWins);
        return 0;
    }

    move_t pick_move(){
        int moveCnt = currentGame.moves(MOVE_BUFFER_1, myPlayer);

        int turnsPerState = 20000 / moveCnt;

        move_t bestMove = MOVE_BUFFER_1[0];
        float bestWinRate = -INT_MAX;

        for(int i = 0; i<moveCnt; i++){
            float res = play_random_games(currentGame, MOVE_BUFFER_1[i], myPlayer, turnsPerState);
            if(res > bestWinRate){
                bestWinRate = res;
                bestMove = MOVE_BUFFER_1[i];
            }
        }

        return bestMove;
    }

    void loop(){
        srand(time(0));
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

                string sx,sy,ex,ey; cin >> sx >> sy >> ex >> ey;

                move_t opponentMove;

                if(sx[0] != '-'){
                    opponentMove.where = pos_t(sx[0] - '0', sy[0] - '0');

#ifdef DEBUG
                    cerr << "Opponent index: " << (int)OPPONENT(myPlayer) << endl;
                    cerr << "Trying to find figure for opponent in (" << opponentMove.where.get_x() << ", " <<  opponentMove.where.get_y() << ")" << endl; 
#endif

                    for(figure_t figure = RAT; figure <= ELEPHANT; figure++){
#ifdef DEBUG
                        cerr << "Comparing for figure " << (int)figure << ": " << currentGame.figurePositions[OPPONENT(myPlayer)].figures[figure].get_x() << " " << currentGame.figurePositions[OPPONENT(myPlayer)].figures[figure].get_y() << endl;
#endif

                        if(currentGame.figurePositions[OPPONENT(myPlayer)].figures[figure] == opponentMove.where){
                            opponentMove.figure = figure;
                            break;
                        }
                    }


                    if(opponentMove.figure == NONE){
                        cerr << "Opponents move seems incorrect" << endl;
                        throw runtime_error("Opponents move seems incorrect.");
                    }

                    opponentMove.where = pos_t(ex[0] - '0', ey[0] - '0');

                    for(figure_t fig = RAT; fig <= ELEPHANT; fig++){
                        if(currentGame.figurePositions[myPlayer].figures[fig] == opponentMove.where){
                            opponentMove.figure = fig;
                            break;
                        }
                    }
                }
                    
                currentGame.do_move(opponentMove, OPPONENT(myPlayer));

#ifdef DEBUG
                cerr << "Opponent move: " << (int)(opponentMove.figure) << " - " << opponentMove.where.get_x() << " " << opponentMove.where.get_y() << endl;
                cerr << currentGame.draw() << endl;
#endif

            }
            else{
                //assert(command == "UGO");

                string x,y; 
                cin >> x >> y;

#ifdef DEBUG
                cerr << "UGO FIRST" << endl;
#endif

                myPlayer = PLAYER0;
            }

            move_t moveToMake = pick_move();
            pos_t originalPos = currentGame.figurePositions[myPlayer].figures[moveToMake.figure];

            currentGame.do_move(moveToMake, myPlayer);

            cout << "IDO ";
            
            if(moveToMake.where.isEmpty())
                cout << "-1 -1 -1 -1" << endl;
            else{
                cout << originalPos.get_x() << " " << originalPos.get_y() << " " << moveToMake.where.get_x() << " " << moveToMake.where.get_y() << endl;
            }

#ifdef DEBUG
            cerr << "Player index: " << (int)myPlayer << endl;
            cerr << "Player move: " << (int)(moveToMake.figure) << " - " << moveToMake.where.get_x() << " " << moveToMake.where.get_y() << endl;
            cerr << currentGame.draw() << endl;
#endif
        }
    }

};

int main(){

    // parse text map to our map
    parsemap("..#*#..", 0);
    parsemap("...#...", 1);
    parsemap(".......", 2);
    parsemap(".~~.~~.", 3);
    parsemap(".~~.~~.", 4);
    parsemap(".~~.~~.", 5);
    parsemap(".......", 6);
    parsemap("...#...", 7);
    parsemap("..#*#..", 8);

    Player player;
    player.loop();


}