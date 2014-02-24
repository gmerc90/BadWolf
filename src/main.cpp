/*  BadWolf
    Copyright (C) 2014  Gary Mercado (errdivideby0)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <ncurses.h>
#include <menu.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"
#include "map.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define TERM_Y_SIZE 30
#define TERM_X_SIZE 80

struct player_struct{
    int posy;
    int posx;
    int old_posy;
    int old_posx;
    char player_character = '@';
    char replace_character;
    char replace_character_new;
};


struct ant_struct{
    std::vector<int> number;
    std::vector<int> birth_tick;
    std::vector<int> age;
    std::vector<int> posy;
    std::vector<int> posx;
    std::vector<int> old_posy;
    std::vector<int> old_posx;
    std::vector<char> holding;
    std::vector<std::string> type;
    std::vector<char> character;
    std::vector<bool> wings;
};

void initial_map_setup(char game_map[25][81], player_struct player);
void map_refresh(char game_map[25][81]);
char toggle_hex_status(char game_map[25][81], player_struct player);
void create_ant(int tick, ant_struct ant, player_struct player, char game_map[25][81]);
void kill_ant();
void view_ants(ant_struct ant, int tick);
void save_game(char game_map[25][81]);
void load_game(char game_map[25][81]);

bool check_for_edge(char game_map[25][81], player_struct player);

std::string view_menu();

int main(int argc, char *argv[]){

    //set the set to the time for rand
    srand(time(NULL));

    //initial basic declarations
    int ch, tick, vector_size, total_ants;
    char game_map[25][81];
    std::string chosen_menu_option;

    //standard starting stuff for Curses
    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);

    //set window size
    resizeterm(TERM_Y_SIZE,TERM_X_SIZE);

    //color initialization
    init_pair(1, COLOR_RED, COLOR_RED);
    init_pair(2, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(3, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(4, COLOR_WHITE, COLOR_WHITE);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(7, COLOR_RED, COLOR_BLACK);


    //set initial position and initialize the player
    player_struct player;
    player.posy = (LINES - 5)/2;
    player.posx = COLS/2;
    player.replace_character = ':';

    //show initial map
    initial_map_setup(game_map, player);
    map_refresh(game_map);

    //set initial tick value
    tick = 0;

    //creates the initial ant structure and the queen
    ant_struct ant;
    ant.number.resize(1);
    ant.number.push_back(1);
    ant.age.resize(1);
    ant.age.push_back(0);
    ant.birth_tick.resize(1);
    ant.birth_tick.push_back(tick);
    ant.character.resize(1);
    ant.character.push_back('Q');
    ant.type.resize(1);
    ant.type.push_back("Queen");
    ant.wings.resize(1);
    ant.wings.push_back(true);
    ant.posx.resize(1);
    ant.posx.push_back(player.posx);
    ant.posy.resize(1);
    if(game_map[player.posy - 1][player.posx] != '*'){
        ant.posy.push_back(player.posy - 1);
    }else{
        ant.posy.push_back(player.posy - 2);
    }
    game_map[ant.posy.at(1)][ant.posx.at(1)] = ant.character.at(1);
    map_refresh(game_map);

    //main game loop
    while(chosen_menu_option != "Quit"){

        /*checks to see if a key was pressed,
        if one has been, it performs the necessary code to
        complete the desired action*/
        //TODO change it to 'x' miliseconds later and let it be configurable in a config file
        timeout(100);
        switch(ch = getch()){
            //move up
            case 'w':
                player.replace_character_new = game_map[player.posy - 1][player.posx];
                player.old_posx = player.posx;
                player.old_posy = player.posy;
                player.posy = player.posy - 1;
                if(check_for_edge(game_map, player) == false){
                    game_map[player.old_posy][player.old_posx] = player.replace_character;
                    game_map[player.posy][player.posx] = player.player_character;
                    player.replace_character = player.replace_character_new;
                }else{
                    player.posy = player.posy + 1;
                }
                map_refresh(game_map);
                break;
            //move left
            case 'a':
                player.replace_character_new = game_map[player.posy][player.posx - 1];
                player.old_posx = player.posx;
                player.old_posy = player.posy;
                player.posx = player.posx - 1;
                if(check_for_edge(game_map, player) == false){
                    game_map[player.old_posy][player.old_posx] = player.replace_character;
                    game_map[player.posy][player.posx] = player.player_character;
                    player.replace_character = player.replace_character_new;
                }else{
                    player.posx = player.posx + 1;
                }
                map_refresh(game_map);
                break;
            //move down
            case 's':
                player.replace_character_new = game_map[player.posy + 1][player.posx];
                player.old_posx = player.posx;
                player.old_posy = player.posy;
                player.posy = player.posy + 1;
                if(check_for_edge(game_map, player) == false){
                    game_map[player.old_posy][player.old_posx] = player.replace_character;
                    game_map[player.posy][player.posx] = player.player_character;
                    player.replace_character = player.replace_character_new;
                }else{
                    player.posy = player.posy - 1;
                }
                map_refresh(game_map);
                break;
            //move right
            case 'd':
                player.replace_character_new = game_map[player.posy][player.posx + 1];
                player.old_posx = player.posx;
                player.old_posy = player.posy;
                player.posx = player.posx + 1;
                if(check_for_edge(game_map, player) == false){
                    game_map[player.old_posy][player.old_posx] = player.replace_character;
                    game_map[player.posy][player.posx] = player.player_character;
                    player.replace_character = player.replace_character_new;
                }else{
                    player.posx = player.posx - 1;
                }
                map_refresh(game_map);
                break;
            //select a cell
            case 'e':
                player.replace_character = toggle_hex_status(game_map, player);
                map_refresh(game_map);
                break;
            //create ants with their initial value
            case 'c':
                vector_size = ant.number.back() + 1;
                ant.number.resize(vector_size);
                ant.number.push_back(vector_size);
                ant.age.resize(vector_size);
                ant.age.push_back(0);
                ant.birth_tick.resize(vector_size);
                ant.birth_tick.push_back(tick);
                ant.character.resize(vector_size);
                ant.character.push_back('e');
                ant.type.resize(vector_size);
                ant.type.push_back("Egg");
                ant.wings.resize(vector_size);
                ant.wings.push_back(false);
                ant.posx.resize(vector_size);
                ant.posx.push_back(player.posx);
                ant.posy.resize(vector_size);
                if(game_map[player.posy + 1][player.posx] != '*'){
                    ant.posy.push_back(player.posy + 1);
                }else{
                    ant.posy.push_back(player.posy + 2);
                }
                game_map[ant.posy.at(vector_size)][ant.posx.at(vector_size)] = ant.character.at(vector_size);
                mvprintw(24, 3, "%d", ant.number.size());
                map_refresh(game_map);
                break;
            //view ants window
            case KEY_F(2):
                view_ants(ant, tick);
                map_refresh(game_map);
                break;
            //bring up game menu
            case KEY_F(1):
                chosen_menu_option = view_menu();
                map_refresh(game_map);
                break;
        }
        //tick the game
        tick = tick + 1;

        //after each tick updates the age of the ant then grows it if necessary
        total_ants = ant.number.back();
        for(int i = 1; i <= total_ants; i++){
            ant.age.at(i) = tick - ant.birth_tick.at(i);
        }
        for(int i = 1; i <= ant.number.back(); i++){
            if(ant.type.at(i) != "Queen"){
                switch(ant.age.at(i)){
                    case 50:
                        ant.type.at(i) = "Larva";
                        ant.character.at(i) = 'l';
                        break;
                    case 100:
                        ant.type.at(i) = "Pupa";
                        ant.character.at(i) = 'p';
                        break;
                    case 150:
                        ant.type.at(i) = "Adult";
                        ant.character.at(i) = 'a';
                        break;
                    default:
                        break;
                }
                game_map[ant.posy.at(i)][ant.posx.at(i)] = ant.character.at(i);
            }
        }
        map_refresh(game_map);

        if(chosen_menu_option == "Save"){
            save_game(game_map);
            chosen_menu_option = " ";
        }else if(chosen_menu_option =="Load"){
            load_game(game_map);
            chosen_menu_option = " ";
        }else if(chosen_menu_option == "Close"){
            chosen_menu_option = "";
            chosen_menu_option = " ";
        }
    }
    //terminate program
    endwin();
    return 0;
}

//create a menu where you will be able to choose quit, save, and load options
std::string view_menu(){
    //function variable declarations
    char *choices[] = {"Save", "Load", "Quit", "Close"};
    char *menu_title = "Game Menu";
    int ch_b, n_choices, height, width, wind_y, wind_x, title_x;
    bool close_menu = false;
    std::string cur_item;
    ITEM **menu_items;
    MENU *game_menu;
    WINDOW *game_menu_window;

    //set window values
    height = 10;
    width = 20;
    wind_y = (LINES - height) / 2;
    wind_x = (COLS - width) / 2;

    //creates the choices for the menu then creates the menu itself
    n_choices = ARRAY_SIZE(choices);
    menu_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
    for(int i = 0; i < n_choices; i++){
        menu_items[i] = new_item(choices[i], "");
    }
    menu_items[n_choices] = (ITEM *)NULL;
    game_menu = new_menu((ITEM **)menu_items);

    //create menu window and subwindow
    game_menu_window = newwin(height, width, wind_y, wind_x);
    keypad(game_menu_window, TRUE);
    set_menu_win(game_menu, game_menu_window);
    set_menu_sub(game_menu, derwin(game_menu_window, height - 2, width - 2, 2, 2));
    wrefresh(game_menu_window);

    //print window b\order and title
    title_x = (width - strlen(menu_title)) / 2;
    box(game_menu_window, 0, 0);
    mvwprintw(game_menu_window, 1, title_x, menu_title);
    refresh();

    //post the menu the refresh everything.
    post_menu(game_menu);
    refresh();
    wrefresh(game_menu_window);

    //main menu loop.
    while(close_menu != true){
        switch(ch_b = getch()){
            case KEY_DOWN:
                menu_driver(game_menu, REQ_DOWN_ITEM);
                break;
            case KEY_UP:
                menu_driver(game_menu, REQ_UP_ITEM);
                break;
            case 10:
                cur_item = item_name(current_item(game_menu));
                close_menu = true;
                break;
        }
        wrefresh(game_menu_window);

    }
    //cleanup before quit
    unpost_menu(game_menu);
    for(int i = 0; i < n_choices; i++){
        free_item(menu_items[i]);
    }
    free_menu(game_menu);
    delwin(game_menu_window);
    return cur_item;
}

//if needed, kill the ant
void kill_ant(){
    //TODO make kill the ant if needed
}

//view a list of all ants when tab is pressed, alive and dead, and are able to filter the list.
void view_ants(ant_struct ant, int tick){
    //declaring function variables
    WINDOW *view_ants_wind;
    int height, width, wind_y, wind_x, ch_b, total_ants, last_ant_shown, first_ant_shown;

    //ant window variable default values
    height = 20;
    width = 60;
    wind_y = (LINES - height) / 2;
    wind_x = (COLS - width) / 2;
    total_ants = ant.number.back();

    //creates the view ants window
    view_ants_wind = newwin(height, width, wind_y, wind_x);
    box(view_ants_wind, 0, 0);
    scrollok(view_ants_wind, TRUE);
    wrefresh(view_ants_wind);

    //checks to see if the first ant has been created, if it has been they will be printed out.
    int y = 1;
    for(int i = 1; i <= total_ants; i++){
        if(y > 18){
            break;
        }
        mvwprintw(view_ants_wind, y, 1, "Ant Number: %d | Age: %d | Type: %s", ant.number.at(i), ant.age.at(i), ant.type.at(i).c_str());
        y = y + 1;
        last_ant_shown = i;
    }
    wrefresh(view_ants_wind);
    first_ant_shown = 1;

    //scroll the window up and down when the appropriate key is pressed and print out the necessary lines.
    while((ch_b = getch()) != 'q'){
        switch(ch_b){
            case KEY_DOWN:
                if(last_ant_shown + 1 <= ant.number.back()){
                    last_ant_shown = last_ant_shown + 1;
                    first_ant_shown = first_ant_shown + 1;
                    wborder(view_ants_wind, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
                    wrefresh(view_ants_wind);
                    wscrl(view_ants_wind, 1);
                    box(view_ants_wind, 0, 0);
                    mvwprintw(view_ants_wind, 18, 1, "Ant Number: %d | Age: %d | Type: %s", ant.number.at(last_ant_shown),
                              ant.age.at(last_ant_shown), ant.type.at(last_ant_shown).c_str());
                    wrefresh(view_ants_wind);
                }
                break;
            case KEY_UP:
                if(first_ant_shown - 1 >= 1){
                    last_ant_shown = last_ant_shown - 1;
                    first_ant_shown = first_ant_shown -1;
                    wborder(view_ants_wind, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
                    wrefresh(view_ants_wind);
                    wscrl(view_ants_wind, -1);
                    box(view_ants_wind, 0, 0);
                    mvwprintw(view_ants_wind, 1, 1, "Ant Number: %d | Age: %d | Type: %s", ant.number.at(first_ant_shown),
                              ant.age.at(first_ant_shown), ant.type.at(first_ant_shown).c_str());
                    wrefresh(view_ants_wind);
                }
                break;
        }
    }

    delwin(view_ants_wind);
}

//saves all the data from the game to a text file
void save_game(char game_map[25][81]){
    int x;
    FILE * game_map_file;
    game_map_file = fopen("map.txt", "w");
    for(x = 0; x <= 24; x++){
        fputs(game_map[x], game_map_file);
        fputs("\n", game_map_file);
    }
    fclose(game_map_file);
}

//loads all data from a text file and puts it into the game
void load_game(char game_map[25][81]){
//TODO have function load all data from a text file if told to

}

//change the state of the hex cube to selected or not
//FIXME for some reason the block directly above the player will not toggle off
//FIXME the block directly above the player that remains toggled on prevents the cell from correctly toggling on again
char toggle_hex_status(char game_map[25][81], player_struct player){

    bool fields_checked = false;
    bool replace_character_determined = false;
    char map_replace_character, player_replace_char;
    std::vector<int> posy_change, posx_change;
    int posy_check[] = { -1, 0, 1, 0};
    int posx_check[] = { 0, 1, 0, -1};
    while(fields_checked != true){

        //check to find out what to replace spaces with and if the player is on a * spot.
        if(player.replace_character == '*'){
            player_replace_char = player.replace_character;
            fields_checked = true;
        }else if(player.replace_character == ':'){
            map_replace_character = '.';
            player_replace_char = '.';
        }else if(player.replace_character == '.'){
            map_replace_character = ':';
            player_replace_char = ':';
        }

        //Check spaces to replace
        for(int i = 0; i <= 3; i++){
            if(game_map[player.posy + posy_check[i]][player.posx + posx_check[i]] != '*'){
                posy_change.resize(posy_change.size() + 1);
                posy_change.push_back(posy_check[i]);
                posx_change.resize(posx_change.size() + 1);
                posx_change.push_back(posx_check[i]);
            }
        }

        //check to see if there are no fields left to replace
        if(posy_change.size() == 0){
            fields_checked = true;
        }

        //replace the checked spaces
        /*for(int i = 0; i <= posy_change.size(); i++){
            game_map[player.posy + posy_change[i]][player.posx + posx_change[i]] = map_replace_character;
        }*/
        int x = 1;
        for(int i = 0; i <= posy_change.size(); i++){
            mvprintw(25, x, "%d, %d |", posx_change[i], posy_change[i]);
            x = x + 5;
        }

        //clear the vectors
        posy_change.clear();
        posx_change.clear();

        fields_checked = true;
    }
    return player_replace_char;
}


//checks to see if player is trying to move out of the map
bool check_for_edge(char game_map[25][81], player_struct player){
    char test_char = game_map[player.posy][player.posx];
    if(test_char == '#'){
        return true;
    }else{
        return false;
    }
}

//prints out hex_map from map.h
void initial_map_setup(char game_map[25][81], player_struct player){

    //print the # border in red
    int x, y;
    for(x = 0; x <= 79; x++){
        game_map[0][x] = '#';
        game_map[24][x] = '#';
    }
    for(y = 0; y <= 24; y++){
        game_map[y][0] = '#';
        game_map[y][79] = '#';
    }

    //print the pieces of the map until full
    int x_b;
    int piece_number = 1;
    for(y = 1; y < 24; y++){
        switch(piece_number){
            case 1:
                for(x_b = 1; x_b < 79; x_b++){
                    game_map[y][x_b] = hex_map_piece_1[0][x_b];
                }
                piece_number = 2;
                break;
            case 2:
                for(x_b = 1; x_b < 79; x_b++){
                    game_map[y][x_b] = hex_map_piece_2[0][x_b];
                }
                piece_number = 3;
                break;
            case 3:
                for(x_b = 1; x_b < 79; x_b++){
                    game_map[y][x_b] = hex_map_piece_3[0][x_b];
                }
                piece_number = 4;
                break;
            case 4:
                for(x_b = 1; x_b < 79; x_b++){
                    game_map[y][x_b] = hex_map_piece_4[0][x_b];
                }
                piece_number = 1;
                break;
        }
    }

    //terminates each of the lines
    for(y = 0; y <= 24; y++){
        game_map[y][80] = '\0';
    }

    //put player in their initial position
    game_map[player.posy][player.posx] = player.player_character;
}

//refreshes the map to show any changes that have been made
void map_refresh(char game_map[25][81]){
    int x, y;
    for(y = 0; y <= 24; y++){
        for(x = 0; x <= 79; x++){
            if(game_map[y][x] == '#'){
                attron(COLOR_PAIR(1));
                mvaddch(y, x, game_map[y][x]);
                attroff(COLOR_PAIR(1));
            }else if(game_map[y][x] == '@'){
                attron(COLOR_PAIR(2));
                mvaddch(y, x, game_map[y][x]);
                attroff(COLOR_PAIR(2));
            }else if(game_map[y][x] == '.'){
                attron(COLOR_PAIR(3));
                mvaddch(y, x, game_map[y][x]);
                attroff(COLOR_PAIR(3));
            }else if(game_map[y][x] == '*'){
                attron(COLOR_PAIR(4));
                mvaddch(y, x, game_map[y][x]);
                attroff(COLOR_PAIR(4));
            }else if(game_map[y][x] == 'e' || game_map[y][x] == 'l' || game_map[y][x] == 'p' || game_map[y][x] == 'a'){
                attron(COLOR_PAIR(5));
                mvaddch(y, x, game_map[y][x]);
                attroff(COLOR_PAIR(5));
            }else if(game_map[y][x] == 'Q'){
                attron(COLOR_PAIR(6));
                mvaddch(y, x, game_map[y][x]);
                attroff(COLOR_PAIR(6));
            }else if(game_map[y][x] == ':'){
                    attron(COLOR_PAIR(7));
                    mvaddch(y, x, game_map[y][x]);
                    attroff(COLOR_PAIR(7));
            }else{
                mvaddch(y, x, game_map[y][x]);
            }
        }
    }
    refresh();
}
