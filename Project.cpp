#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <utility>
#include <unistd.h>
#include <random>
#include <ncursesw/ncurses.h>
#include <locale.h>
#include "Project.h"
using namespace std;

Hero::Hero(string name) {
    this->name = name;
    this->x = -1;
    this->y = -1;
    this->key = false;
    this->trapped = false;
    this->lose = false;
    this->win = false;
}

vector<pair<int, int>> Hero::scan_neighbors(const vector<string> &map, bool &is_fallback, vector<pair<int, int>> &Hero_used_pos, vector<pair<int, int>> &Hero_fallback_used) {
    vector<pair<int, int>> available_pos, nearby_used, fallback;
    int temp_row, temp_col, dx, dy;
    int height = map.size();
    int wideness = (map.empty() ? 0 : map[0].size());

    is_fallback = false;

    /*[x-1][y-1]  [x-1][y]  [x-1][y+1]
     * [x][y-1]     [x][y]   [x][y+1]
     * [x+1][y-1]  [x+1][y]  [x+1][y+1]
     */

    vector<pair<int, int>> neighbors = {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1},           {0, 1},
        {1, -1},  {1, 0},  {1, 1}
    };


    for (auto [neighbor_x, neighbor_y] : neighbors) {
        temp_row = x + neighbor_x;
        temp_col = y + neighbor_y;

        if (temp_row < 0 || temp_row >= height || temp_col < 0 || temp_col >= wideness) {
            continue;
        }

        //Check valid positions: no walls
        if (map[temp_row][temp_col] != '*' && map[temp_row][temp_col] != 'L') {
            bool used = false;
            //Check if position has been already visited
            for (auto [used_x, used_y] : Hero_used_pos) {
                if (temp_row == used_x && temp_col == used_y) {
                    used = true;
                    break;
                }
            }

            if (used == false) {
                //If position has not been visited it gets setted as available position
                available_pos.emplace_back(temp_row, temp_col);
            }
        }
    }

    //If there are new positions (not visited)
    if (!available_pos.empty()) {
        return available_pos;
    }

    //If hero has not any new position to visit he will choose one of the visited positions that are exactly nearby him

    for (auto &coord : Hero_used_pos) {
        int ux = coord.first;
        int uy = coord.second;
        dx = abs(ux - x);
        dy = abs(uy - y);

        if (dx <= 1 && dy <= 1 && (dx != 0 || dy != 0)) {
            //Check if hero gets out of map
            if (ux >= 0 && ux < height && uy >= 0 && uy < wideness && map[ux][uy] != '*') {
                nearby_used.emplace_back(ux, uy);
            }
        }
    }

    //Remove fro nearby_used every position that is in S_fallback_used
    for (auto &pos : nearby_used) {
        //If pos is already in S_fallback_used
        bool is_in_fallback = false;
        for (auto &old_fb : Hero_fallback_used) {
            if (pos == old_fb) {
                is_in_fallback = true;
                break;
            }
        }
        if (!is_in_fallback) {
            fallback.emplace_back(pos);
        }
    }

    //If there is not any fallback position we reset S_fallback_used
    if (fallback.empty()) {
        Hero_fallback_used.clear();
        //Then we use again nearby_used positions
        fallback = nearby_used;
    }

    is_fallback = true;
    return fallback;
}

char Hero::get_name() const {
    return name[0];
}

void Hero::set_xy(int x, int y) {
    this->x = x;
    this->y = y;
}

int Hero::get_x() const {
    return x;
}

int Hero::get_y() const {
    return y;
}

void Hero::set_key(bool key) {
    this->key = key;
}

bool Hero::get_key() const {
    return key;
}

void Hero::set_trapped(bool trapped) {
    this->trapped = trapped;
}

bool Hero::get_trapped() const {
    return trapped;
}

void Hero::set_lose(bool lose) {
    this->lose = lose;
}

bool Hero::get_lose() const {
    return lose;
}

void Hero::set_win(bool win) {
    this->win = win;
}

bool Hero::get_win() const {
    return win;
}

G::G(string name):
Hero(name)
{
    this->G_used_pos = {};
    this->G_fallback_used = {};
}

void G::move_algorithm(vector<string> &map, vector<pair<int, int>> coordinates, Hero &hero) {
    vector<pair<int, int>> invalid_coords = {{-1, -1}};
    vector<pair<int, int>> free_pos;
    int rand_index;
    //The coordinates where the hero will go
    int new_row, new_col;
    //Check if scan_neighbors returned fallback positions or "new" ones
    bool is_fallback = false;

    //Check invalid coordinates from function find_hero_coordinates, so when function has not find requested_hero
    if (coordinates == invalid_coords || coordinates.empty()) {
        cout << endl << endl << "move_algorithm: no valid coords. Try again in a bit" << endl;
        exit(1);
    }

    //If S has already won
    if (hero.get_win()) {
        return;
    }


    //If G is trapped
    if (get_trapped()) {
        map[x][y] = 'G';    //G stays in the same spot
        return;
    }

    free_pos = scan_neighbors(map, is_fallback, G_used_pos, G_fallback_used);

    //If G is NOT trapped
    if (!free_pos.empty()) {
        //If G does not have key
        if (!get_key()) {
            bool key_found = false;
            for (auto [best_x, best_y] : free_pos) {
                //Win condition
                if (map[best_x][best_y] == 'S' && !hero.get_trapped()) {
                    new_row = best_x;
                    new_col = best_y;

                    map[x][y] = ' ';    //clear previous position of G
                    set_xy(new_row, new_col);
                    map[new_row][new_col] = 'G';

                    set_win(true);
                    return;
                }

                else if (map[best_x][best_y] == 'K') {
                    new_row = best_x;
                    new_col = best_y;

                    map[x][y] = ' ';    //clear previous position of G
                    set_xy(new_row, new_col);
                    map[new_row][new_col] = 'G';

                    set_key(true);
                    cout << "G got key!!!" << endl;
                    key_found = true;

                    if (is_fallback) {
                        G_fallback_used.emplace_back(new_row, new_col);
                    }

                    G_used_pos.emplace_back(new_row, new_col);
                }
            }

            //If G tried to get key but did not find it
            if (!key_found) {
                rand_index = rand() % free_pos.size();

                new_row = free_pos[rand_index].first;
                new_col = free_pos[rand_index].second;

                if (map[new_row][new_col] == 'T') {
                    set_trapped(true); //Hero is trapped
                }

                map[x][y] = ' ';    //Clear previous position of G

                set_xy(new_row, new_col);
                map[new_row][new_col] = 'G';
                //Now it has been visited (used)
                if (is_fallback) {
                    G_fallback_used.emplace_back(new_row, new_col);
                }

                G_used_pos.emplace_back(new_row, new_col);
            }
        }

        //If G has key
        else {
            for (auto [best_x, best_y] : free_pos) {
                //If G finds S (G has KEY) and S is trapped, he WILL GO TO S to free her and WIN
                if (map[best_x][best_y] == 'S') {
                    new_row = best_x;
                    new_col = best_y;

                    map[x][y] = ' ';    //Clear previous position of S
                    set_xy(new_row, new_col);
                    //Set win position
                    map[new_row][new_col] = '@';

                    set_win(true);
                    return;    //No motion of S
                }
            }

            rand_index = rand() % free_pos.size();

            new_row = free_pos[rand_index].first;
            new_col = free_pos[rand_index].second;

            //If visited square is trap
            if (map[new_row][new_col] == 'T') {
                set_trapped(true); //Hero is trapped

                if (get_key()) {
                    set_lose(true); //Hero loses because he also has the key
                }

                map[x][y] = ' ';    //clear previous position of G
                set_xy(new_row, new_col);
                map[new_row][new_col] = 'G';

                if (is_fallback) {
                    G_fallback_used.emplace_back(new_row, new_col);
                }

                G_used_pos.emplace_back(new_row, new_col);

                return;
            }

            //If visited square is NOT trap
            else {
                map[x][y] = ' ';    //clear previous position of G
                set_xy(new_row, new_col);
                map[new_row][new_col] = 'G';

                if (is_fallback) {
                    G_fallback_used.emplace_back(new_row, new_col);
                }

                G_used_pos.emplace_back(new_row, new_col);
            }
        }
    }
}

void G::locate_G(vector<string> &map) {
    vector<pair<int, int>> free_pos;
    size_t rand_row, rand_col, rand_i;

    //Find free positions in map
    for (size_t row = 0; row < map.size(); row++) {
        for (size_t col = 0; col < map[row].size(); col++) {
            if (map[row][col] == ' ') {
                free_pos.emplace_back(row, col);
            }
        }
    }

    //Locate 'G' randomly in a free position
    if (!free_pos.empty()) {    //free position
        rand_i = rand() % free_pos.size();
        rand_row = free_pos[rand_i].first;  //first from pair
        rand_col = free_pos[rand_i].second; //second from pair
        map[rand_row][rand_col] = 'G';

        set_xy(rand_row, rand_col);
        //Add initial position of G
        G_used_pos.emplace_back(rand_row, rand_col);
    }
}

S::S(string name):
    Hero(name)
{
    this->S_used_pos = {};
    this->S_fallback_used = {};
}

void S::move_algorithm(vector<string> &map, vector<pair<int, int>> coordinates, Hero &hero) {
    vector<pair<int, int>> invalid_coords = {{-1, -1}};
    vector<pair<int, int>> free_pos;
    int rand_index;
    //The coordinates where the hero will go
    int new_row, new_col;
    //Check if scan_neighbors returned fallback positions or "new" ones
    bool is_fallback = false;

    //Check invalid coordinates from function find_hero_coordinates, so when function has not find requested_hero
    if (coordinates == invalid_coords || coordinates.empty()) {
        cout << endl << endl << "move_algorithm: no valid coords. Try again in a bit" << endl;
        exit(1);
    }

    //If G has already won
    if (hero.get_win()) {
        return;
    }


    //If S is trapped
    if (get_trapped()) {
        map[x][y] = 'S';    //S stays in the same spot
        return;
    }

    free_pos = scan_neighbors(map, is_fallback, S_used_pos, S_fallback_used);

    //If S is NOT trapped
    if (!free_pos.empty()) {

        //If S does not have key
        if (!get_key()) {
            bool key_found = false;
            for (auto [best_x, best_y] : free_pos) {
                //If S finds G AND G is not trapped, she does not move so G comes to her
                if (map[best_x][best_y] == 'G' && !hero.get_trapped()) {
                    return;    //No motion of S
                }

                else if (map[best_x][best_y] == 'K') {
                    new_row = best_x;
                    new_col = best_y;

                    map[x][y] = ' ';    //clear previous position of S
                    set_xy(new_row, new_col);
                    map[new_row][new_col] = 'S';

                    set_key(true);
                    cout << "S got key!!!" << endl;
                    key_found = true;

                    if (is_fallback) {
                        S_fallback_used.emplace_back(new_row, new_col);
                    }

                    S_used_pos.emplace_back(new_row, new_col);
                }
            }

            //If S tried to get key but did not find it
            if (!key_found) {
                rand_index = rand() % free_pos.size();

                new_row = free_pos[rand_index].first;
                new_col = free_pos[rand_index].second;

                if (map[new_row][new_col] == 'T') {
                    set_trapped(true); //Hero is trapped
                }

                map[x][y] = ' ';    //clear previous position of S

                set_xy(new_row, new_col);
                map[new_row][new_col] = 'S';

                //Now it has been visited (used)
                if (is_fallback) {
                    S_fallback_used.emplace_back(new_row, new_col);
                }

                S_used_pos.emplace_back(new_row, new_col);
            }
        }

        //If S has key
        else {
            for (auto [best_x, best_y] : free_pos) {
                //If S finds G (S has KEY) and G is trapped, she WILL GO TO G to free him and WIN
                if (map[best_x][best_y] == 'G' && hero.get_trapped()) {
                    new_row = best_x;
                    new_col = best_y;

                    map[x][y] = ' ';    //Clear previous position of S
                    set_xy(new_row, new_col);
                    //Set win position
                    map[new_row][new_col] = '@';

                    set_win(true);
                    return;    //No motion of S
                }

                //If S has key and G is not trapped
                else if (map[best_x][best_y] == 'G' && !hero.get_trapped()) {
                    return; //S will not move so G comes to her
                }
            }

            rand_index = rand() % free_pos.size();

            new_row = free_pos[rand_index].first;
            new_col = free_pos[rand_index].second;

            //If visited square is trap
            if (map[new_row][new_col] == 'T') {
                set_trapped(true); //Hero is trapped

                if (get_key()) {
                    set_lose(true); //Hero loses because he also has the key
                }

                map[x][y] = ' ';    //clear previous position of S
                set_xy(new_row, new_col);
                map[new_row][new_col] = 'S';

                if (is_fallback) {
                    S_fallback_used.emplace_back(new_row, new_col);
                }

                S_used_pos.emplace_back(new_row, new_col);

                return;
            }

            //If visited square is NOT trap
            else {
                map[x][y] = ' ';    //clear previous position of S
                set_xy(new_row, new_col);
                map[new_row][new_col] = 'S';

                if (is_fallback) {
                    S_fallback_used.emplace_back(new_row, new_col);
                }

                S_used_pos.emplace_back(new_row, new_col);
            }
        }
    }
}

void S::locate_S(vector<string> &map, char first_hero) {
    const size_t min_distance = 7;
    vector<pair<size_t, size_t>> pos_min_dist;   //free positions 7 squars far from first hero
    vector<pair<int, int>> first_hero_coords;
    size_t rand_row, rand_col, rand_i;

    //Find coordinates of first hero in the map
    first_hero_coords = find_hero_coordinates(map, first_hero);
    if (first_hero_coords[0].first == -1) {
        cout << "First Hero not found..." << endl;
    }

    //Find valid positions (7 squares far from first hero)
    for (int row = 0; row < map.size() &&first_hero_coords[0].first != -1; row++) {
        for (int col = 0; col < map[row].size(); col++) {
            if (map[row][col] == ' ') {
                if (abs(row - first_hero_coords[0].first) >= min_distance || abs(col - first_hero_coords[0].second) >= min_distance) {
                    pos_min_dist.emplace_back(row, col);
                }
            }
        }
    }

    //Locate second hero randomly 7 squares away from first hero
    if (!pos_min_dist.empty()) {
        rand_i = rand() % pos_min_dist.size();
        rand_row = pos_min_dist[rand_i].first;  //first from pair
        rand_col = pos_min_dist[rand_i].second; //second from pair
        map[rand_row][rand_col] = 'S';

        set_xy(rand_row, rand_col);
        //Add initial position of S
        S_used_pos.emplace_back(rand_row, rand_col);
    }


    else {
        cout << "No available positions inside the map for second hero (no " << min_distance << " squars positions free)..." << endl;
    }
}


//Function to find coordinates of heros in the map
vector<pair<int, int>> find_hero_coordinates(const vector<string> &map, char requested_hero) {
    vector<pair<int, int>> hero_coordinates = {{-1, -1}};

    for (int row = 0; row < map.size(); row++) {
        for (int col = 0; col < map[row].size(); col++) {
            if (map[row][col] == requested_hero) {
                hero_coordinates[0].first = row;
                hero_coordinates[0].second = col;
                return hero_coordinates;
            }
        }
    }

    return {{-1, -1}};
}

vector<string> read_map(const string &map_file) {
    ifstream file(map_file);
    string line;
    vector<string> map;

    if (file.is_open()) {
        while (getline(file, line)) {   //Read one line each time
            // Check for invalid characters
            for (char invalid_c : line) {
                if (invalid_c != ' ' && invalid_c != '*' && invalid_c != 'L') {
                    cout << "Invalid character '" << invalid_c << "' found in map..." << endl;
                    file.close();
                    exit(1);
                }
            }
            map.push_back(line);    //Add lines of map_file at the end of map (map is vector<string>)
        }

        file.close();
    }

    else {
        cout << "Error opening map file..." << endl;
    }

    return map;
}

void print_map(const vector<string> &map) {
    clear();    //Clears previous maps

    for (size_t i = 0; i < map.size(); i++) {
        mvprintw(i, 0, "%s", map[i].c_str());
    }

    refresh();  //Refreshes screen
    napms(500); //Waits 0.5 sec before the next termina
}

void win_function(vector<string> &map, Hero &hero) {
    vector<pair<int, int>> features;
    vector<pair<int, int>> L_coords;
    int L_x, L_y, GoL_x, GoL_y;

    for (size_t row = 0; row < map.size(); row++) {
        for (size_t col = 0; col < map[row].size(); col++) {
            if (map[row][col] != 'L' && map[row][col] != 'S' && map[row][col] != 'G' && map[row][col] != '@') {
                features.emplace_back(row, col);
            }
        }
    }

    random_device rd;
    mt19937 g(rd());
    shuffle(features.begin(), features.end(), g);

    for (auto [row, col] : features) {
        map[row][col] = ' ';
        clear();

        for (size_t i = 0; i < map.size(); i++) {
            mvprintw(i, 0, "%s", map[i].c_str());
        }

        refresh();
        napms(10);
    }

    GoL_x = hero.get_x();
    GoL_y = hero.get_y();
    L_coords = find_hero_coordinates(map, 'L');
    L_x = L_coords[0].first;
    L_y = L_coords[0].second;

    //Set coordinates as it should be in order to go to L
    while (GoL_x != L_x || GoL_y != L_y) {

        if (GoL_x < L_x) {
            GoL_x++;
        }

        else if (GoL_x > L_x) {
            GoL_x--;
        }

        if (GoL_y < L_y) {
            GoL_y++;
        }

        else if (GoL_y > L_y) {
            GoL_y--;
        }


        map[hero.get_x()][hero.get_y()] = ' ';    //Clear previous position of hero
        hero.set_xy(GoL_x, GoL_y);
        map[GoL_x][GoL_y] = '@';

        clear();
        for (size_t i = 0; i < map.size(); i++) {
            mvprintw(i, 0, "%s", map[i].c_str());
        }
        refresh();
        napms(500);
    }

    napms(500);
}

void locate_to_map(vector<string> &map, char feature, bool clear_map) {
    vector<pair<size_t, size_t>> free_pos;
    size_t rand_row, rand_col, rand_i;

    //Clear map from features
    if (clear_map) {
        for (size_t row = 0; row < map.size(); row++) {
            for (size_t col = 0; col < map[row].size(); col++) {
                if (map[row][col] == feature) {
                    map[row][col] = ' ';
                }
            }
        }
    }

    //Find free positions in map
    for (size_t row = 0; row < map.size(); row++) {
        for (size_t col = 0; col < map[row].size(); col++) {
            if (map[row][col] == ' ') {
                free_pos.emplace_back(row, col);
            }
        }
    }

    //Locate feature randomly in a free position
    if (!free_pos.empty()) {    //free position
        rand_i = rand() % free_pos.size();
        rand_row = free_pos[rand_i].first;  //first from pair
        rand_col = free_pos[rand_i].second; //second from pair
        map[rand_row][rand_col] = feature;
    }
}







