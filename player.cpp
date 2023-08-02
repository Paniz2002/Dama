#include "player.hpp"
using namespace std;

struct Cell{
    Player::piece field[8][8];
    Cell * next;
};
typedef Cell* Node;

struct Player::Impl{

    Node head;
    int player_nr;
    int dummy;
    enum direction{sx,dx,bsx,bdx,asx,adx,all};

    void destroy( Node cell) const;
    Node copy(Node source) const;
    void prepend(Player::piece field[8][8]);
    static piece chartopiece(char c);
    char piecetochar(piece p) const;

    bool canMove(Player::piece field[8][8], int r, int c, int number,direction dir);
    bool canMoveDama(Player::piece field[8][8], int r, int c, direction dir);

    bool canEat(Player::piece field[8][8], int r, int c, int number,direction dir);
    bool canEatDama(Player::piece field[8][8], int r, int c, int number,direction dir);


    void eat(Player::piece field[8][8], int i, int j, int number, direction dir);
    bool eatDama(Player::piece field[8][8], int i, int j, int number);

    void move(Player::piece field[8][8], int i, int j, int number,direction dir);
    bool moveDama(Player::piece field[8][8], int i, int j);

    piece operator()(int r, int c, int history_offset = 0) const;

    bool is_valid(Player::piece field[8][8]);
    void load_board(const string& filename);
    void store_board(const string& filename, int history_offset = 0) const;
};

/* costruttore */
Player::Player(int player_nr ){
    if(player_nr != 1 && player_nr != 2) {
        throw player_exception{
                player_exception::index_out_of_bounds,
                "player number is wrong"
        };
    }
    srand(time(NULL));
    pimpl = new Impl;
    pimpl->head = nullptr;
    pimpl->player_nr = player_nr;
}


/* distruttore */

Player::~Player(){
    pimpl->destroy(pimpl->head);
    delete pimpl;
}

void Player::Impl:: destroy(Node cell)const{
    if(cell){
        destroy(cell->next);
        delete cell;
    }
}


/* copy constructor */

Player::Player(const Player& source){
    pimpl = new Impl;
    pimpl->player_nr = source.pimpl->player_nr;
    pimpl->head = pimpl->copy(source.pimpl->head);
}

Node Player::Impl::copy(Node source) const {
    if(source == nullptr){
        return nullptr;
    }else{
        Node dest = new Cell;

        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                dest->field[i][j] = source->field[i][j];
            }
        }

        dest->next = copy(source->next);

        return dest;
    }
}
Player& Player::operator=(const Player & source){
    if(this != &source) {
        pimpl->destroy(pimpl->head);
        pimpl = new Impl;
        pimpl->player_nr = source.pimpl->player_nr;
        pimpl->head = pimpl->copy(source.pimpl->head);
    }
    return *this;
}



/*
 * Operatore di accesso alla memoria del giocatore.
 * Questo operatore restituisce la pedina contenuta in riga r e colonna c della “history_offset”-esima
 * scacchiera più recente. In altre parole, se history_offset=0 va restituita la pedina in coordinata (r,c)
 * dell’ultima scacchiera vista; se history_offset=1 va restituita la pedina in coordinata (r,c) della penultima
 * scacchiera vista, ... Lanciare una player_exception con campo err_type uguale a index_out_of_bounds (msg a scelta)
 * se le coordinate (r,c,history_offset) non esistono in memoria.
 */

Player::piece Player::operator()(int r, int c, int history_offset) const {
    return pimpl->operator()(r,c,history_offset);
}

Player::piece Player::Impl::operator()(int r, int c, int history_offset) const {

    if(r > 7 || c > 7 || r < 0 || c < 0 || head == nullptr || history_offset < 0){
        throw player_exception{
                player_exception::index_out_of_bounds,
                "coordinates don't exist in memory"
        };
    }

    piece ret;
    Node iter = head;

    for(int k = 0; k <= history_offset; k++){
        if(iter == nullptr){
            throw player_exception{
                    player_exception::index_out_of_bounds,
                    "coordinates don't exist in memory"
            };
        }

        if(k == history_offset){
            for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8; j++){
                    if(i == r && j == c)
                        ret = iter->field[i][j];
                }
            }
        }

        iter = iter->next;

    }

    return ret;
}

void Player::Impl::prepend(Player::piece field[8][8]){
    Node newone = new Cell;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            newone->field[i][j] = field[i][j];
        }
    }
    newone->next = head;
    head = newone;
}

Player::piece Player::Impl::chartopiece(char c){
    switch(c){
        case 'O':
            return piece::O;
        case 'o':
            return piece::o;
        case 'X':
            return piece::X;
        case 'x':
            return piece::x;
        default:
            return piece::e;
    }
}

char Player::Impl::piecetochar(Player::piece p) const {
    switch(p)
    {
        case piece::x:
            return 'x';
        case piece::X:
            return 'X';
        case piece::o:
            return 'o';
        case piece::O:
            return 'O';
        default:
            return ' ';
    }
}



bool::Player::Impl::is_valid(piece field[8][8]){
    bool ret = true;
    for(int i = 0; i < 8; i++){
        int j;
        if(i % 2 == 0)
            j = 1;
        else
            j = 0;

        for(;j < 8; j = j + 2){
            if(field[i][j] != e)
                ret = false;
        }
    }

    int o_count = 0;
    int x_count = 0;

    for(int i = 0; i < 8; i++){
        for(int j = 0; j <8; j++){
            if(field[i][j] == o || field[i][j] == O)
                o_count++;
            else if(field[i][j] == x || field[i][j] == X)
                x_count++;
        }
    }

    if(x_count > 12 || o_count > 12)
        ret = false;

    return ret;

}

void::Player::load_board(const string &filename){
    pimpl->load_board(filename);
}

/* manca l'eccezione se la scacchiera non è corretta */
void Player::Impl::load_board(const string &filename) {
    ifstream is(filename,ifstream::in);
    char c;
    if(is.fail()){
        throw player_exception{
            player_exception::missing_file,
            "Il file non esiste"
        };
    }
    Player::piece field[8][8];
    for(int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++) {
            c = is.get();
            field[i][j] = chartopiece(c);
            is.get();
        }
    }
    is.close();
    prepend(field);
    if(!is_valid(field)){
        throw player_exception{
            player_exception::invalid_board,
            "Invalid board"};
    }
}

void Player::store_board(const string &filename, int history_offset) const {
    pimpl->store_board(filename,history_offset);
}

void Player::Impl::store_board(const string &filename, int history_offset) const {
    Node new_list = head;

    for(int i = 0; i <= history_offset; i++){
        if(new_list == nullptr){
            throw player_exception{
                    player_exception::index_out_of_bounds,
                    "store_board-->lista nulla"
            };
        }else if(i == history_offset){
            ofstream os(filename);
            os.clear();
            piece p;
            for(int j = 0; j < 8; j++){
                for(int k = 0; k < 8; k++){
                    p = new_list->field[j][k];
                    char c;
                    c = piecetochar(p);
                    os.put(c);

                    if(k != 7)
                        os.put(' ');
                }
                if(j != 7)
                    os.put('\n');
            }
        }

        new_list = new_list->next;
    }
}


void Player::init_board(const string &filename) const {

    Player::piece field[8][8];

    for(int i = 0; i < 3; i++){
        if(i % 2 == 0){
            for(int j = 0; j < 8; j=j+2) {
                field[i][j] = Player::o;

                field[i][j+1] = Player::e;
            }
        }else{
            for(int j = 0; j < 8; j=j+2){
                field[i][j] = Player::e;

                field[i][j+1] = Player::o;
            }
        }
    }
    for(int j = 0; j < 8; j++){
        field[3][j] = Player::e;
        field[4][j] = Player::e;
    }

    for(int i = 5; i < 8; i++){
        if(i % 2 == 0){
            for(int j = 0; j < 8; j=j+2) {
                field[i][j] = Player::x;

                field[i][j+1] = Player::e;
            }
        }else{
            for(int j = 0; j < 8; j=j+2) {
                field[i][j] = Player::e;

                field[i][j+1] = Player::x;
            }
        }
    }

    pimpl->prepend(field);


    ofstream os(filename);
    os.clear();
    piece p;
    for(int j = 0; j < 8; j++){
            for(int k = 0; k < 8; k++){

                p = field[j][k];

                os.put(pimpl->piecetochar(p));
                if(k != 7)
                    os.put(' ');
            }


        if(j != 7)
            os.put('\n');
    }
}

void Player::move() {
    bool moved = false;


    if(pimpl->head == nullptr){
        throw player_exception{
                player_exception::index_out_of_bounds,
                "player history is empty"
        };
    }

    piece newboard[8][8];

    /* copia della board */
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            newboard[i][j] = pimpl->head->field[i][j];
            //cout<<pimpl->piecetochar(newboard[i][j])<<" ";
        }
        //cout<<endl;
    }

    int randomEating = rand()%4;
    int randomCycle = rand()%2;

    if(randomCycle == 0)
    {
        if(randomEating > 0 ){
            for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8; j++){
                    if(!moved){
                        if (((newboard[i][j] == X && pimpl->player_nr == 1) ||
                             (newboard[i][j] == O && pimpl->player_nr == 2))) {

                            if(pimpl->eatDama(newboard,i,j,pimpl->player_nr))
                                moved = true;

                        }

                        if((pimpl->canEat(newboard, i, j, pimpl->player_nr,Impl::direction::dx)) &&
                           (pimpl->canEat(newboard, i, j, pimpl->player_nr,Impl::direction::sx))){

                            pimpl->eat(newboard, i, j,pimpl->player_nr,Impl::direction::all);
                            moved = true;

                        }
                        else if(pimpl->canEat(newboard, i, j, pimpl->player_nr,Impl::direction::dx)){

                            pimpl->eat(newboard, i, j, pimpl->player_nr,Impl::direction::dx);
                            moved = true;

                        }else if(pimpl->canEat(newboard, i, j, pimpl->player_nr,Impl::direction::sx)){

                            pimpl->eat(newboard,i,j,pimpl->player_nr,Impl::direction::sx);
                            moved = true;

                        }
                    }
                }
            }
        }


        if(!moved){
            for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8; j++){
                    if (!moved) {

                        if (((newboard[i][j] == X && pimpl->player_nr == 1) ||
                             (newboard[i][j] == O && pimpl->player_nr == 2))) {
                            if (pimpl->canMoveDama(newboard, i, j, Impl::all)) {
                                if(pimpl->moveDama(newboard, i, j))
                                    moved = true;
                            }
                        }

                        if (!moved) {
                            //può muoversi sia a destra che a sinistra
                            if ((pimpl->canMove(newboard, i, j, pimpl->player_nr, Impl::dx)) &&
                                (pimpl->canMove(newboard, i, j, pimpl->player_nr, Impl::sx))) {
                                moved = true;
                                pimpl->move(newboard, i, j, pimpl->player_nr, Impl::all);
                                //può muoversi solo a destra
                            } else if (pimpl->canMove(newboard, i, j, pimpl->player_nr, Impl::dx)) {
                                moved = true;
                                pimpl->move(newboard, i, j, pimpl->player_nr, Impl::dx);
                                //può muoversi solo a sinistra
                            } else if (pimpl->canMove(newboard, i, j, pimpl->player_nr, Impl::sx)) {
                                moved = true;
                                pimpl->move(newboard, i, j, pimpl->player_nr, Impl::sx);
                            }
                        }
                    }
                }
            }

        }
    }

    else

    {
        if(randomEating > 0 ){
            for(int i = 7; i >= 0; i--){
                for(int j = 7; j >= 0; j--){
                    if(!moved){
                        if (((newboard[i][j] == X && pimpl->player_nr == 1) ||
                             (newboard[i][j] == O && pimpl->player_nr == 2))) {

                            if(pimpl->eatDama(newboard,i,j,pimpl->player_nr))
                                moved = true;

                        }

                        if((pimpl->canEat(newboard, i, j, pimpl->player_nr,Impl::direction::dx)) &&
                           (pimpl->canEat(newboard, i, j, pimpl->player_nr,Impl::direction::sx))){

                            pimpl->eat(newboard, i, j,pimpl->player_nr,Impl::direction::all);
                            moved = true;

                        }
                        else if(pimpl->canEat(newboard, i, j, pimpl->player_nr,Impl::direction::dx)){

                            pimpl->eat(newboard, i, j, pimpl->player_nr,Impl::direction::dx);
                            moved = true;

                        }else if(pimpl->canEat(newboard, i, j, pimpl->player_nr,Impl::direction::sx)){

                            pimpl->eat(newboard,i,j,pimpl->player_nr,Impl::direction::sx);
                            moved = true;

                        }
                    }
                }
            }
        }


        if(!moved){
            for(int i = 7; i >= 0; i--){
                for(int j = 7; j >= 0; j--){
                    if (!moved) {

                        if (((newboard[i][j] == X && pimpl->player_nr == 1) ||
                             (newboard[i][j] == O && pimpl->player_nr == 2))) {
                            if (pimpl->canMoveDama(newboard, i, j, Impl::all)) {
                                if(pimpl->moveDama(newboard, i, j))
                                    moved = true;
                            }
                        }

                        if (!moved) {
                            //può muoversi sia a destra che a sinistra
                            if ((pimpl->canMove(newboard, i, j, pimpl->player_nr, Impl::dx)) &&
                                (pimpl->canMove(newboard, i, j, pimpl->player_nr, Impl::sx))) {
                                moved = true;
                                pimpl->move(newboard, i, j, pimpl->player_nr, Impl::all);
                                //può muoversi solo a destra
                            } else if (pimpl->canMove(newboard, i, j, pimpl->player_nr, Impl::dx)) {
                                moved = true;
                                pimpl->move(newboard, i, j, pimpl->player_nr, Impl::dx);
                                //può muoversi solo a sinistra
                            } else if (pimpl->canMove(newboard, i, j, pimpl->player_nr, Impl::sx)) {
                                moved = true;
                                pimpl->move(newboard, i, j, pimpl->player_nr, Impl::sx);
                            }
                        }
                    }
                }
            }

        }
    }



    /* trasformo in dama*/
    for(int i = 0; i < 8; i++){
        if(newboard[0][i] == piece::x)
            newboard[0][i] = piece::X;

        if(newboard[7][i] == piece::o)
            newboard[7][i] = piece::O;
    }

    pimpl->prepend(newboard);

}


bool Player::Impl::canMove(Player::piece field[8][8], int r, int c, int number, Impl::direction dir){
    piece tmp = field[r][c];
    if(number == 1){
        if(tmp == x ){
            if     (field[r-1][c+1] == piece::e && (dir == Impl::direction::all || dir == Impl::direction::dx)){
                if(r == 0 || c == 7)
                    return false;
                return true;
            }


            else if(field[r-1][c-1] == piece::e && (dir == Impl::direction::all || dir == Impl::direction::sx)){
                if(r == 0 || c == 0)
                    return false;
                return true;

            }
            else
                return false;
        }else
            return false;


    }else{
        if(tmp == o){
            if(field[r+1][c+1] == piece::e && (dir == Impl::direction::all || dir == Impl::direction::dx)){
                if(r == 7 || c == 7)
                    return false;
                return true;
            }

            else if(field[r+1][c-1] == piece::e && (dir == Impl::direction::all || dir == Impl::direction::sx)){
                if(r == 7 || c == 0)
                    return false;
                return true;
            }
            else
                return false;
        }else
            return false;


    }
}

bool Player::Impl::canMoveDama(Player::piece field[8][8], int r, int c, Player::Impl::direction dir) {

        if(r-1 >= 0 && c+1 < 8 && field[r - 1][c + 1] == piece::e && (dir == adx || dir == all)){
            if(r == 0 || c == 7)
                return false;
            return true;
        }
        else if(r-1 >= 0 && c-1 >= 0 && field[r - 1][c - 1] == piece::e && (dir == asx || dir == all)){
            if(r == 0 || c == 0)
                return false;
            return true;
        }else if(r + 1 < 8 && c + 1 < 8 && field[r + 1][c + 1] == piece::e && (dir == bdx || dir == all)){
            if(r == 7 || c == 7)
                return false;
            return true;
        }else if(r + 1 < 8 && c - 1 >= 0 && field[r+1][c-1] == piece::e && (dir == bsx || dir == all)){
            if(r == 7 || c == 0)
                return false;
            return true;
        }else
            return false;

}



bool Player::Impl::canEat(Player::piece field[8][8], int r, int c, int number,Impl::direction dir){
    if(number == 1){
        if(field[r][c] == piece::x || field[r][c] == piece::X) {
            if (r-2 >= 0 && c+2 < 8 && field[r - 2][c + 2] == piece::e && field[r - 1][c + 1] == piece::o &&
                (dir == Impl::direction::dx || dir == Impl::direction::all)) {
                if (r <= 1 || c >= 6)
                    return false;
                return true;
            } else if (r-2 >= 0 && c-2 >= 0 && field[r - 2][c - 2] == piece::e && field[r - 1][c - 1] == piece::o &&
                       (dir == Impl::direction::sx || dir == Impl::direction::all)) {
                if (r <= 1 || c <= 1)
                    return false;
                return true;
            } else
                return false;
        }else
            return false;
    }else{
        if(field[r][c] == piece::o || field[r][c] == piece::O) {
            if (r + 2 < 8 && c + 2 < 8 && field[r + 2][c + 2] == piece::e && field[r + 1][c + 1] == piece::x &&
                (dir == Impl::direction::dx || dir == Impl::direction::all)) {
                if (r >= 6 || c >= 6)
                    return false;
                return true;
            } else if (r + 2 < 8 && c - 2 >= 0 && field[r + 2][c - 2] == piece::e && field[r + 1][c - 1] == piece::x &&
                       (dir == Impl::direction::sx || dir == Impl::direction::all)) {
                if (r >= 6 || c <= 1)
                    return false;
                return true;
            } else
                return false;
        }else
            return false;
    }
}

bool Player::Impl::canEatDama(Player::piece field[8][8], int r, int c, int number, Player::Impl::direction dir) {

    if(number == 1){

            if (r - 2 >= 0 && c + 2 < 8 && field[r - 2][c + 2] == piece::e && ( field[r - 1][c - 1] == piece::o  || field[r - 1][c - 1] == piece::O) && dir == Impl::direction::adx) {
                return true;
            } else if (r - 2 >= 0 && c - 2 >= 0 && field[r - 2][c - 2] == piece::e && ( field[r - 1][c - 1] == piece::o  || field[r - 1][c - 1] == piece::O) && dir == Impl::direction::asx ) {
                return true;
            }else if (r + 2 < 8 && c + 2 < 8 && field[r + 2][c + 2] == piece::e && ( field[r - 1][c - 1] == piece::o  || field[r - 1][c - 1] == piece::O) && dir == Impl::direction::bdx){
                return true;
            }else if (r + 2 < 8 && c - 2 >= 0 && field[r + 2][c - 2] == piece::e && ( field[r - 1][c - 1] == piece::o  || field[r - 1][c - 1] == piece::O) && dir == Impl::direction::bsx) {
                return true;
            } else
                return false;

    }else if(number == 2){
        if (r - 2 >= 0 && c + 2 < 8 && field[r - 2][c + 2] == piece::e && ( field[r - 1][c - 1] == piece::o  || field[r - 1][c - 1] == piece::O) && dir == Impl::direction::adx) {
            return true;
        }else if (r - 2 >= 0 && c - 2 >= 0 && field[r - 2][c - 2] == piece::e && ( field[r - 1][c - 1] == piece::x  || field[r - 1][c - 1] == piece::X) && dir == Impl::direction::asx ) {
            return true;
        }else if (r + 2 < 8 && c + 2 < 8 && field[r + 2][c + 2] == piece::e && ( field[r - 1][c - 1] == piece::x  || field[r - 1][c - 1] == piece::X) && dir == Impl::direction::bdx){
            return true;
        }else if (r + 2 < 8 && c - 2 >= 0 && field[r + 2][c - 2] == piece::e && ( field[r - 1][c - 1] == piece::x  || field[r - 1][c - 1] == piece::X) && dir == Impl::direction::bsx) {
            return true;
        } else
            return false;
    }else
        return false;
}


void Player::Impl::move(Player::piece field[8][8], int i, int j, int number,Impl::direction dir){

    piece tmp = field[i][j];

    if( dir == direction::all){
        int randomDir = rand()%2;
        if(randomDir == 0){
            if(number == 1){
                field[i][j] = piece::e;
                field[i-1][j+1] = tmp;
            }else{
                field[i][j] = piece::e;
                field[i+1][j+1] = tmp;
            }
        }else if(randomDir == 1){
            if(number == 1){
                field[i][j] = piece::e;
                field[i-1][j-1] = tmp;
            }else{
                field[i][j] = piece::e;
                field[i+1][j-1] = tmp;
            }
        }


    }else if( dir == direction::dx){
        if(number == 1){
            field[i][j] = piece::e;
            field[i-1][j+1] = tmp;
        }else{
            field[i][j] = piece::e;
            field[i+1][j+1] = tmp;
        }


    }else if( dir ==  direction::sx){
        if(number == 1){
            field[i][j] = piece::e;
            field[i-1][j-1] = tmp;
        }else{
            field[i][j] = piece::e;
            field[i+1][j-1] = tmp;
        }
    }
}

bool Player::Impl::moveDama(Player::piece field[8][8], int i, int j) {

    piece tmp = field[i][j];
    int randomDir = rand()%2;
    bool ret = false;
    bool priority;
    if(i > 3)
        priority = false; //se può va verso l alto
    else
        priority = true;//se può va verso il basso


    if(priority){
        if(canMoveDama(field,i,j,bsx) && canMoveDama(field,i,j,bdx)){
            if(randomDir){
                //bsx
                field[i][j] = piece::e;
                field[i + 1][j - 1] = tmp;
            }else{
                //bdx
                field[i][j] = piece::e;
                field[i + 1][j + 1] = tmp;
            }
            ret = true;
        }else if(canMoveDama(field,i,j,bsx)){
            //bsx
            field[i][j] = piece::e;
            field[i + 1][j - 1] = tmp;
            ret = true;
        }else if(canMoveDama(field,i,j,bdx)){
            //bdx
            field[i][j] = piece::e;
            field[i + 1][j + 1] = tmp;
            ret = true;
        }else{

            if(canMoveDama(field,i,j,asx)){
                field[i][j] = piece::e;
                field[i - 1][j - 1] = tmp;
                ret = true;
            }else if(canMoveDama(field,i,j,adx)){
                field[i][j] = piece::e;
                field[i - 1][j + 1] = tmp;
                ret = true;
            }

        }
    }else{
        if(canMoveDama(field,i,j,asx) && canMoveDama(field,i,j,adx)){
            if(randomDir){
                //asx
                field[i][j] = piece::e;
                field[i - 1][j - 1] = tmp;
            }else{
                //adx
                field[i][j] = piece::e;
                field[i - 1][j + 1] = tmp;
            }
            ret = true;
        }else if(canMoveDama(field,i,j,asx)){
            //asx
            field[i][j] = piece::e;
            field[i - 1][j - 1] = tmp;
            ret = true;
        }else if(canMoveDama(field,i,j,adx)){
            //adx
            field[i][j] = piece::e;
            field[i - 1][j + 1] = tmp;
            ret = true;
        }else if(canMoveDama(field,i,j,bsx)){
            //bsx
            field[i][j] = piece::e;
            field[i + 1][j - 1] = tmp;
            ret = true;
        }else if(canMoveDama(field,i,j,bdx)){
            //bdx
            field[i][j] = piece::e;
            field[i + 1][j + 1] = tmp;
            ret = true;
        }
    }

    if(ret)
        return true;
    else
        return false;

}


void Player::Impl::eat(Player::piece field[8][8], int i, int j, int number,Impl::direction dir){

    piece tmp = field[i][j];

    if(dir == direction::all){
        int randomDir = rand()%2;
        if(randomDir == 0){
            if(number == 1){
                field[i][j] = piece::e;
                field[i-1][j+1] = piece::e;
                field[i-2][j+2] = tmp;
            }else{
                field[i][j] = piece::e;
                field[i+1][j+1] = piece::e;
                field[i+2][j+2] = tmp;
            }
        }else{
            if(number == 1){
                field[i][j] = piece::e;
                field[i-1][j-1] = piece::e;
                field[i-2][j-2] = tmp;
            }else{
                field[i][j] = piece::e;
                field[i+1][j-1] = piece::e;
                field[i+2][j-2] = tmp;
            }
        }
    }else if(dir == dx){

        if(number == 1){
            field[i][j] = piece::e;
            field[i-1][j+1] = piece::e;
            field[i-2][j+2] = tmp;
        }else{
            field[i][j] = piece::e;
            field[i+1][j+1] = piece::e;
            field[i+2][j+2] = tmp;
        }
    }else if(dir == sx){

        if(number == 1){
            field[i][j] = piece::e;
            field[i-1][j-1] = piece::e;
            field[i-2][j-2] = tmp;
        }else{
            field[i][j] = piece::e;
            field[i+1][j-1] = piece::e;
            field[i+2][j-2] = tmp;
        }
    }

}

bool Player::Impl::eatDama(Player::piece field[8][8], int i, int j, int number) {
    piece tmp = field[i][j] ;
    int randomDir = rand() % 2;
    bool ret = false;
    if(number == 1)
    {
        if(canEatDama(field,i,j,number,bsx) && canEatDama(field,i,j,number,bdx) ){

            if(randomDir)
            {
                //bsx
                field[i][j] = e;
                field[i + 1][j - 1]= e;
                field[i + 2][j - 2]= tmp;
            }else
            {
                //bdx
                field[i][j] = e;
                field[i + 1][j + 1]= e;
                field[i + 2][j + 2]= tmp;
            }
            ret = true;
        }else if(canEatDama(field,i,j,number,bsx))
        {
            field[i][j] = e;
            field[i + 1][j - 1]= e;
            field[i + 2][j - 2]= tmp;
            ret = true;
        }
        else if(canEatDama(field,i,j,number,bdx))
        {
            field[i][j] = e;
            field[i + 1][j + 1]= e;
            field[i + 2][j + 2]= tmp;
            ret = true;
        }
        else if(canEatDama(field,i,j,number,asx))
        {
            field[i][j] = piece::e;
            field[i-1][j-1] = piece::e;
            field[i-2][j-2] = tmp;
            ret = true;
        }else if(canEatDama(field,i,j,number,adx))
        {
            field[i][j] = piece::e;
            field[i-1][j+1] = piece::e;
            field[i-2][j+2] = tmp;
            ret = true;
        }
    }else
    {
        if(canEatDama(field,i,j,number,asx) && canEatDama(field,i,j,number,adx) ){

            if(randomDir)
            {
                //asx
                field[i][j] = piece::e;
                field[i-1][j-1] = piece::e;
                field[i-2][j-2] = tmp;
            }else
            {
                //adx
                field[i][j] = e;
                field[i - 1][j + 1]= e;
                field[i - 2][j + 2]= tmp;
            }
            ret = true;
        }else if(canEatDama(field,i,j,number,asx))
        {
            field[i][j] = e;
            field[i - 1][j - 1]= e;
            field[i - 2][j - 2]= tmp;
            ret = true;
        }
        else if(canEatDama(field,i,j,number,adx))
        {
            field[i][j] = e;
            field[i - 1][j + 1]= e;
            field[i - 2][j + 2]= tmp;
            ret = true;
        }
        else if(canEatDama(field,i,j,number,bsx))
        {
            field[i][j] = piece::e;
            field[i+1][j-1] = piece::e;
            field[i+2][j-2] = tmp;
            ret = true;
        }else if(canEatDama(field,i,j,number,bdx))
        {
            field[i][j] = piece::e;
            field[i+1][j+1] = piece::e;
            field[i+2][j+2] = tmp;
            ret = true;
        }
    }

    if(ret)
        return true;
    else
        return false;

}



bool Player::valid_move() const{

    if(pimpl->head->next == nullptr || pimpl->head == nullptr)
    {
        throw player_exception{
                player_exception::index_out_of_bounds,
                "there are less than 2 boards in history"
        };
    }

    if(!pimpl->is_valid(pimpl->head->field) || !pimpl->is_valid(pimpl->head->next->field))
        return false;

    piece last[8][8];
    piece previous[8][8];

    bool equals = true;
    bool valid = false;
    int count_x_last = 0;
    int count_x_prev = 0;

    int count_o_last = 0;
    int count_o_prev = 0;

    for(int i =0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            last[i][j] = pimpl->head->field[i][j];
            previous[i][j] = pimpl->head->next->field[i][j];
            if(last[i][j] != previous[i][j])
                equals = false;

            if(last[i][j] == x || last[i][j] == X )
                count_x_last++;
            else if(last[i][j] == o || last[i][j] == O)
                count_o_last++;

            if(previous[i][j] == x || previous[i][j] == X )
                count_x_prev++;
            else if(previous[i][j] == o || previous[i][j] == O)
                count_o_prev++;
        }
    }


    if( count_o_last == count_o_prev || count_o_last == count_o_prev - 1 )
        valid = true;

    if( count_x_last == count_x_prev || count_x_last == count_x_prev - 1 )
        valid = true;


    if(equals)
        return false;

    if(!valid)
        return false;
    else
        return true;
}

void Player::pop() {

    if(pimpl->head == nullptr)
    {
        throw player_exception{
                player_exception::index_out_of_bounds,
                "player history is empty"
        };
    }

    Cell* tmp = pimpl->head->next;

    delete(pimpl->head);

    pimpl->head = tmp;

}

bool Player::wins(int player_nr) const {

    if((player_nr != 1 && player_nr != 2) || pimpl->head == nullptr)
    {
        throw player_exception{
                player_exception::index_out_of_bounds,
                "player number is wrong or player history is empty"
        };
    }
    int countEnemy = 0;

    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(player_nr == 1)
            {
                if(pimpl->head->field[i][j] == o || pimpl->head->field[i][j] == o)
                    countEnemy++;
            }else
            {
                if(pimpl->head->field[i][j] == x || pimpl->head->field[i][j] == X)
                    countEnemy++;
            }
        }
    }

    if(countEnemy == 0)
        return true;
    else
        return false;

}

bool Player::wins() const {
    if(pimpl->head == nullptr)
    {
        throw player_exception{
                player_exception::index_out_of_bounds,
                "player history is empty"
        };
    }
    int countEnemy = 0;

    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(pimpl->player_nr == 1)
            {
                if(pimpl->head->field[i][j] == o || pimpl->head->field[i][j] == o)
                    countEnemy++;
            }else
            {
                if(pimpl->head->field[i][j] == x || pimpl->head->field[i][j] == X)
                    countEnemy++;
            }
        }
    }

    if(countEnemy == 0)
        return true;
    else
        return false;
}

bool Player::loses(int player_nr) const {
    if((player_nr != 1 && player_nr != 2) || pimpl->head == nullptr)
    {
        throw player_exception{
                player_exception::index_out_of_bounds,
                "player number is wrong or player history is empty"
        };
    }
    int countPedinas = 0;

    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(player_nr == 1)
            {
                if(pimpl->head->field[i][j] == x || pimpl->head->field[i][j] == X)
                    countPedinas++;
            }else
            {
                if(pimpl->head->field[i][j] == o || pimpl->head->field[i][j] == o)
                    countPedinas++;
            }
        }
    }

    if(countPedinas == 0)
        return true;
    else
        return false;
}

bool Player::loses() const {
    if(pimpl->head == nullptr)
    {
        throw player_exception{
                player_exception::index_out_of_bounds,
                "player history is empty"
        };
    }
    int countPedinas = 0;

    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(pimpl->player_nr == 1)
            {
                if(pimpl->head->field[i][j] == x || pimpl->head->field[i][j] == X)
                    countPedinas++;
            }else
            {
                if(pimpl->head->field[i][j] == o || pimpl->head->field[i][j] == o)
                    countPedinas++;
            }
        }
    }

    if(countPedinas == 0)
        return true;
    else
        return false;
}

int Player::recurrence() const {
    piece tmp[8][8];
    Cell* headCopy = pimpl->head;

    int recurrence = 0;
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            tmp[i][j] = pimpl->head->field[i][j];
        }
    }

    while(headCopy != nullptr)
    {
        bool isDifferent = false;

        for(int i = 0; i < 8; i++)
        {
            for(int j = 0; j < 8; j++)
            {
                if(tmp[i][j] != headCopy->field[i][j])
                    isDifferent = true;
            }
        }

        if(!isDifferent)
            recurrence++;

        headCopy = headCopy->next;
    }

    return recurrence;
}