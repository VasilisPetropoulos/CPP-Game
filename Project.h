#ifndef PROJECT_H
#define PROJECT_H

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <utility>
#include <algorithm>
#include <unistd.h>
#include <ncursesw/ncurses.h>
#include <locale.h>
using namespace std;

class Hero {
    public:
        Hero(string name);

        vector<pair<int, int>> scan_neighbors(const vector<string> &map, bool &is_fallback, vector<pair<int, int>> &Hero_used_pos, vector<pair<int, int>> &Hero_fallback_used);

        virtual void move_algorithm(vector<string> &map, vector<pair<int, int>> coordinates, Hero &hero) = 0;  //pure virtual method
        char get_name() const;  //Returns only first character of name in order the name to fit in functions and in map
        void set_xy(int x, int y);
        int get_x() const;
        int get_y() const;
        void set_key(bool key);
        bool get_key() const;
        void set_trapped(bool trapped);
        bool get_trapped() const;
        void set_lose(bool lose);
        bool get_lose() const;
        void set_win(bool win);
        bool get_win() const;
        virtual ~Hero() {};
    protected:
        string name;
        int x;
        int y;
        bool key;
        bool trapped;
        bool lose;
        bool win;
};

class S;

class G: public Hero {
    public:
        G(string name);
        void move_algorithm(vector<string> &map, vector<pair<int, int>> coordinates, Hero &hero);
        void locate_G(vector<string> &map);

    private:
        vector<pair<int, int>> G_used_pos; //Positions visited by hero G
        vector<pair<int, int>> G_fallback_used;  //Fall back positions
};

class S: public Hero {
    public:
        S(string name);
        void move_algorithm(vector<string> &map, vector<pair<int, int>> coordinates, Hero &hero);
        void locate_S(vector<string> &map, char first_hero);

    private:
        vector<pair<int, int>> S_used_pos; //Positions visited by hero S
        vector<pair<int, int>> S_fallback_used;  //Fall back positions
};

vector<pair<int, int>> find_hero_coordinates(const vector<string> &map, char requested_hero);

//Read map_file and return a vector<string> with its contents
vector<string> read_map(const string &map_file);
//Print map - print the strings of the vector that includes map contents
void print_map(const vector<string> &map);

//Locate S, T (2), K (only first hero and features)
void locate_to_map(vector<string> &map, char hero_or_feature, bool clear_map = false);

void win_function(vector<string> &map, Hero &hero);

#endif
