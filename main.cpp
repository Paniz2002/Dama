//
// Created by Nicola Panizzolo on 02/08/22.
//

#include "player.hpp"
using namespace std;

int main(){
    Player p1(1);
    Player p2(2);
    int i = 0;
    p1.init_board("board_0.txt");
    int invalidCount = 0;

    while(i < 200){
        try{
            cout << "PLAYER 1:" << endl;
            cout << "CARICATA BOARD: " << i << endl;
            p1.load_board ("board_" + to_string(i) + ".txt");
            p1.move();
            i++;
            cout << "SALVATA BOARD: " << i << endl;
            p1.store_board ("board_" + to_string(i) + ".txt");

            if(!p1.valid_move())
            {
                cout<< "INVALID MOVE\n\n\n\n\n\n\n\n\n\n";
                invalidCount++;
            }


            cout << "PLAYER 2:" << endl;
            cout << "CARICATA BOARD: " << i << endl;
            p2.load_board( "board_" + to_string(i) + ".txt");
            p2.move();
            i++;
            p2.store_board("board_" + to_string(i) + ".txt");
            cout << "SALVATA BOARD: " << i << endl;

            if(!p2.valid_move())
            {
                cout<< "INVALID MOVE\n\n\n\n\n\n\n\n\n\n";
                invalidCount++;
            }


        }catch(player_exception e){
            cerr << e.msg << endl;
        }
    }

    cout<<"Invalid moves : "<<invalidCount<<endl;
    cout<<"recurrence: "<<p1.recurrence()<<endl;
    if(p1.wins(1))
        cout<<"vince 1"<<endl;
    else
        cout<<"vince 2"<<endl;

    if(p1.wins())
        cout<<"vince 1"<<endl;
    else
        cout<<"vince 2"<<endl;

    if(p1.loses(1))
        cout<<"perde 1"<<endl;
    else
        cout<<"perde 2"<<endl;

    if(p1.loses())
        cout<<"perde 1"<<endl;
    else
        cout<<"perde 2"<<endl;
    cout<<"r = 3, c = 6"<<p1(3,6)<<endl;

    p1.pop();



}