#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <utility>
#include <ncursesw/ncurses.h>
#include <locale.h>
#include "Project.h"
using namespace std;

int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "You have to put map file as argument...";
        exit(1);
    }

    srand(time(0));
    vector<string> map = read_map(argv[1]);

    G Grigorakis("Grigorakis");
    S Asimenia("S");


    Grigorakis.locate_G(map);
    Asimenia.locate_S(map, Grigorakis.get_name());
    locate_to_map(map, 'T');
    locate_to_map(map, 'T');
    locate_to_map(map, 'K');

    setlocale(LC_ALL, "");
    initscr();//Starts ncurses mode
    noecho();
    curs_set(0);    //Hides cursor

    //Run game for 1000 termina
    for (int i = 0; i < 1000; i++) {
        print_map(map);

        vector<pair<int, int>> G_coords = {{Grigorakis.get_x(), Grigorakis.get_y()}};

        vector<pair<int, int>> S_coords = {{Asimenia.get_x(), Asimenia.get_y()}};

        Grigorakis.move_algorithm(map, G_coords, Asimenia);

        if (Grigorakis.get_win()) {
            //Map prints G above S
            print_map(map);
            napms(500);

            //Then they become one @
            int win_x = Grigorakis.get_x(), win_y = Grigorakis.get_y();
            map[win_x][win_y] = '@';
            print_map(map);
            napms(1000);

            win_function(map, Grigorakis);

            endwin();
            cout << endl << "GRIGORAKIS AND ASIMENIA WON!!!" << endl;
            return 0;
        }

        Asimenia.move_algorithm(map, S_coords, Grigorakis);

        if (Asimenia.get_win()) {
            int win_x = Asimenia.get_x(), win_y = Asimenia.get_y();
            map[win_x][win_y] = '@';
            print_map(map);
            napms(1000);

            win_function(map, Asimenia);

            endwin();
            cout << endl << "GRIGORAKIS AND ASIMENIA WON!!!" << endl;
            return 0;
        }

        if (Grigorakis.get_lose() || Asimenia.get_lose() || (Grigorakis.get_trapped() && Asimenia.get_trapped())){
            print_map(map);
            cout << endl << "GRIGORAKIS AND ASIMENIA LOST!!!..." << endl;
            return 0;
        }

        if (i == 999) {
            cout << endl << "GAME OVER" << endl;
        }
    }

    endwin();   //terminate ncurses mode




    return 0;
}
