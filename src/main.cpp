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

#include <curses.h>
#include <vector>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"

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

bool check_for_edge(std::vector<std::string>  rendered_map, player_struct player);

std::vector<std::string> toggle_hex_status(std::vector<std::string>  new_rendered_map, player_struct player);
std::vector<std::string> copy_map(std::vector<std::string>  map_to_copy);
std::vector<std::string> initial_map_setup(player_struct player);

int select_ant(ant_struct ant, player_struct player);

std::string view_menu();

void map_refresh(std::vector<std::string>  rendered_map);
void create_ant(int tick, ant_struct ant, player_struct player, std::vector<std::string>  rendered_map);
void kill_ant();
void view_ants(ant_struct ant, int tick);
void save_game(std::vector<std::string>  rendered_map);
void load_game(std::vector<std::string>  rendered_map);

int main(int argc, char *argv[]){

    //set the set to the time for rand
    srand(time(NULL));

    //initial basic declarations
    int ch, tick, vector_size, total_ants;
    int selected_ant = NULL;
    std::vector<std::string>  rendered_map, underground_map, surface_map;
    std::string chosen_menu_option;
    std::string current_map;

    //standard starting stuff for Curses
    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);

    //set window size
    resize_term(TERM_Y_SIZE,TERM_X_SIZE);

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
    rendered_map = initial_map_setup(player);
    map_refresh(rendered_map);
    //underground_map = copy_map(rendered_map);
    current_map = "underground";

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
    if(rendered_map[player.posy - 1][player.posx] != '*'){
        ant.posy.push_back(player.posy - 1);
    }else{
        ant.posy.push_back(player.posy - 2);
    }
    rendered_map[ant.posy.at(1)][ant.posx.at(1)] = ant.character.at(1);
    map_refresh(rendered_map);

    //main game loop
    while(chosen_menu_option != "Quit"){

        /*checks to see if a key was pressed,
        if one has been, it performs the necessary code to
        complete the desired action*/
        ///TODO change it to 'x' miliseconds later and let it be configurable in a config file
        timeout(100);
        switch(ch = getch()){
            //move up
            case 'w':
                player.replace_character_new = rendered_map[player.posy - 1][player.posx];
                player.old_posx = player.posx;
                player.old_posy = player.posy;
                player.posy = player.posy - 1;
                if(check_for_edge(rendered_map, player) == false){
                    rendered_map[player.old_posy][player.old_posx] = player.replace_character;
                    rendered_map[player.posy][player.posx] = player.player_character;
                    player.replace_character = player.replace_character_new;
                }else{
                    player.posy = player.posy + 1;
                }
                map_refresh(rendered_map);
                break;

            //move left
            case 'a':
                player.replace_character_new = rendered_map[player.posy][player.posx - 1];
                player.old_posx = player.posx;
                player.old_posy = player.posy;
                player.posx = player.posx - 1;
                if(check_for_edge(rendered_map, player) == false){
                    rendered_map[player.old_posy][player.old_posx] = player.replace_character;
                    rendered_map[player.posy][player.posx] = player.player_character;
                    player.replace_character = player.replace_character_new;
                }else{
                    player.posx = player.posx + 1;
                }
                map_refresh(rendered_map);
                break;

            //move down
            case 's':
                player.replace_character_new = rendered_map[player.posy + 1][player.posx];
                player.old_posx = player.posx;
                player.old_posy = player.posy;
                player.posy = player.posy + 1;
                if(check_for_edge(rendered_map, player) == false){
                    rendered_map[player.old_posy][player.old_posx] = player.replace_character;
                    rendered_map[player.posy][player.posx] = player.player_character;
                    player.replace_character = player.replace_character_new;
                }else{
                    player.posy = player.posy - 1;
                }
                map_refresh(rendered_map);
                break;

            //move right
            case 'd':
                player.replace_character_new = rendered_map[player.posy][player.posx + 1];
                player.old_posx = player.posx;
                player.old_posy = player.posy;
                player.posx = player.posx + 1;
                if(check_for_edge(rendered_map, player) == false){
                    rendered_map[player.old_posy][player.old_posx] = player.replace_character;
                    rendered_map[player.posy][player.posx] = player.player_character;
                    player.replace_character = player.replace_character_new;
                }else{
                    player.posx = player.posx - 1;
                }
                map_refresh(rendered_map);
                break;

            //select an ant
            case 'e':
                selected_ant = select_ant(ant, player);
                break;
            //clear selected ant with Shift + e
            case 'E':
                selected_ant = NULL;
                break;

            //select a cell
            case 't':
                rendered_map = toggle_hex_status(rendered_map, player);
                map_refresh(rendered_map);
                break;

            //create ants with their initial value
            case 'c':
                vector_size = ant.number.back() + 1;
                ant.number.push_back(vector_size);
                ant.age.push_back(0);
                ant.birth_tick.push_back(tick);
                ant.character.push_back('e');
                ant.type.push_back("Egg");
                ant.wings.push_back(false);
                ant.posx.push_back(player.posx);
                if(rendered_map[player.posy + 1][player.posx] != '*'){
                    ant.posy.push_back(player.posy + 1);
                }else{
                    ant.posy.push_back(player.posy + 2);
                }
                rendered_map[ant.posy.at(vector_size)][ant.posx.at(vector_size)] = ant.character.at(vector_size);
                mvprintw(24, 3, "%d", ant.number.size());
                map_refresh(rendered_map);
                break;

            //bring up game menu
            case KEY_F(1):
                chosen_menu_option = view_menu();
                map_refresh(rendered_map);
                //take the appropriate actions for the chosen menu option.
                if(chosen_menu_option == "Save"){
                    save_game(rendered_map);
                    chosen_menu_option = " ";
                }else if(chosen_menu_option =="Load"){
                    load_game(rendered_map);
                    chosen_menu_option = " ";
                }else if(chosen_menu_option == "Close"){
                    chosen_menu_option = " ";
                }
                break;

            //view ants window
            case KEY_F(2):
                view_ants(ant, tick);
                map_refresh(rendered_map);
                break;
        }
        //tick the game
        tick = tick + 1;

        //after each tick updates the age of the ant then grows it if necessary
        total_ants = ant.number.back();
        for(int i = 1; i <= total_ants; i++){
            ant.age.at(i) = tick - ant.birth_tick.at(i);
        }
        for(int i = 1; i <= total_ants; i++){
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
                rendered_map[ant.posy.at(i)][ant.posx.at(i)] = ant.character.at(i);
            }
        }
        map_refresh(rendered_map);

        //show selected ant information
        if(selected_ant != NULL){
            move(26, 0);
            clrtoeol();
            mvprintw(26, 1, "Number: %d | Type: %s | Age: %d", ant.number.at(selected_ant), ant.type.at(selected_ant).c_str(), ant.age.at(selected_ant));
        }else{
            move(26, 0);
            clrtoeol();
            mvprintw(26, 1, "Number:   | Type:   | Age:   ");
        }
    }
    //terminate program
    endwin();
    return 0;
}

//copy a map from one array to another
std::vector<std::string> copy_map(std::vector<std::string>  map_to_copy){
    std::vector<std::string> copied_map;
    for(int y = 0; y <= 24; y++){
        for(int x = 0; x <= 80; x++)
            copied_map[y][x] = map_to_copy[y][x];
    }
    return copied_map;
}


//select an ant to give commands to
int select_ant(ant_struct ant, player_struct player){
    //declaring variables
    int selected_ant, total_ants;

    //check all the ants until it finds one that matches the player coordinates
    total_ants = ant.number.back();
    for(int i = 0; i <= total_ants; i++){
        if((ant.posy[i] == player.posy) && (ant.posx[i] == player.posx)){
            selected_ant = i;
            break;
        }else{
            selected_ant = NULL;
        }
    }
    return selected_ant;
}

//create a menu where you will be able to choose quit, save, and load options
std::string view_menu(){
    //function variable declarations
    char *choices[] = {"Save", "Load", "Quit", "Close"};
    char *menu_title = "Game Menu";
    int ch_b, height, width, wind_y, wind_x, title_x, menu_item_x, menu_item_y, cur_item_y, cur_item_num;
    bool close_menu = false;
    std::string cur_item;
    WINDOW *game_menu_window;

    //set window values
    height = 10;
    width = 20;
    wind_y = ((LINES - 5) - height) / 2;
    wind_x = (COLS - width) / 2;

    //create menu window and subwindow
    game_menu_window = newwin(height, width, wind_y, wind_x);
    keypad(game_menu_window, TRUE);
    wrefresh(game_menu_window);

    //print window border, menu title, and menu options.
    title_x = (width - strlen(menu_title)) / 2;
    box(game_menu_window, 0, 0);
    wattron(game_menu_window, A_BOLD);
    mvwprintw(game_menu_window, 1, title_x, menu_title);
    wattroff(game_menu_window, A_BOLD);
    menu_item_y = 3;
    menu_item_x = (width - strlen(choices[0])) / 2;
    mvwprintw(game_menu_window, menu_item_y, menu_item_x, "%s<", choices[0]);
    cur_item_y = menu_item_y;
    cur_item_num = 0;
    menu_item_y = 4;
    for(int i = 1; i <= 3; i++){
        menu_item_x = (width - strlen(choices[i])) / 2;
        mvwprintw(game_menu_window, menu_item_y, menu_item_x, choices[i]);
        menu_item_y = menu_item_y + 1;
    }
    wrefresh(game_menu_window);

    //main menu loop.
    while(close_menu != true){
        switch(ch_b = getch()){
            case KEY_DOWN:
                if(cur_item_num + 1 <= 3){
                    wmove(game_menu_window, cur_item_y, 0);
                    wclrtoeol(game_menu_window);
                    mvwprintw(game_menu_window, cur_item_y, ((width - strlen(choices[cur_item_num])) / 2), choices[cur_item_num]);
                    cur_item_num = cur_item_num + 1;
                    cur_item_y = cur_item_y + 1;
                    mvwprintw(game_menu_window, cur_item_y, ((width - strlen(choices[cur_item_num])) / 2), "%s<", choices[cur_item_num]);
                    box(game_menu_window, 0, 0);
                }
                break;
            case KEY_UP:
                if(cur_item_num - 1 >= 0){
                    wmove(game_menu_window, cur_item_y, 0);
                    wclrtoeol(game_menu_window);
                    mvwprintw(game_menu_window, cur_item_y, ((width - strlen(choices[cur_item_num])) / 2), choices[cur_item_num]);
                    cur_item_num = cur_item_num - 1;
                    cur_item_y = cur_item_y - 1;
                    mvwprintw(game_menu_window, cur_item_y, ((width - strlen(choices[cur_item_num])) / 2), "%s<", choices[cur_item_num]);
                    box(game_menu_window, 0, 0);
                }
                break;
            //enter key
            case 10:
                cur_item = choices[cur_item_num];
                close_menu = true;
                break;
        }
        wrefresh(game_menu_window);
    }
    //cleanup before quit and returns the selected option to the main function
    delwin(game_menu_window);
    clear();
    return cur_item;
}

//if needed, kill the ant
void kill_ant(){
    ///TODO make kill the ant if needed
}

//view a list of all ants when tab is pressed, alive and dead, and are able to filter the list.
void view_ants(ant_struct ant, int tick){
    //declaring function variables
    WINDOW *view_ants_wind;
    int height, width, wind_y, wind_x, ch_b, total_ants, last_ant_shown, first_ant_shown;

    //ant window variable default values
    height = 20;
    width = 60;
    wind_y = ((LINES - 5) - height) / 2;
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
    clear();
}

///FIXME to work with the new format
//saves all the data from the game to a text file
void save_game(std::vector<std::string>  rendered_map){
    /*int x;
    FILE * rendered_map_file;
    rendered_map_file = fopen("map.txt", "w");
    for(x = 0; x <= 24; x++){
        fputs(rendered_map[x], rendered_map_file);
        fputs("\n", rendered_map_file);
    }
    fclose(rendered_map_file);*/
}

//loads all data from a text file and puts it into the game
void load_game(std::vector<std::string>  rendered_map){
///TODO have function load all data from a text file if told to

}

///FIXME to work with the new format
//change the state of the hex cube to selected or not
std::vector<std::string> toggle_hex_status(std::vector<std::string>  new_rendered_map, player_struct player){

    char map_replace_character, player_replace_char;
    int posy_change[] = { -1, 0, 1, 0};
    int posx_change[] = { 0, 1, 0, -1};
    int pos_neg_one[] = { 1, -1};

    //check to find out what to replace spaces with and if the player is on a * spot.
    if(player.replace_character != '*'){
        if(player.replace_character == ':'){
            map_replace_character = '.';
            player_replace_char = '.';
        }else if(player.replace_character == '.'){
            map_replace_character = ':';
            player_replace_char = ':';
        }

        //check to see if there are any fields left to replace

        ///TODO think of better names for old_y/x(_b)
        //replace the checked spaces
        for(int i = 0; i <= 3; i++){
            int old_y = player.posy;
            int old_x = player.posx;
            int old_y_b, old_x_b;
            while((new_rendered_map[old_y + posy_change[i]][old_x + posx_change[i]] != '*') && (new_rendered_map[old_y + posy_change[i]][old_x + posx_change[i]] != '#')){
                new_rendered_map[old_y + posy_change[i]][old_x + posx_change[i]] = map_replace_character;
                old_y = old_y + posy_change[i];
                old_x = old_x + posx_change[i];
                old_y_b = old_y;
                old_x_b = old_x;
                for(int i = 0; i <= 1; i++){
                    while((new_rendered_map[old_y_b + pos_neg_one[i]][old_x_b] != '*') && (new_rendered_map[old_y_b + pos_neg_one[i]][old_x_b] != '#')){
                        new_rendered_map[old_y_b + pos_neg_one[i]][old_x_b] = map_replace_character;
                        old_y_b = old_y_b + pos_neg_one[i];
                    }
                    old_y_b = old_y;
                    while((new_rendered_map[old_y_b][old_x_b + pos_neg_one[i]] != '*') && (new_rendered_map[old_y_b][old_x_b + pos_neg_one[i]] != '#')){
                        new_rendered_map[old_y_b][old_x_b + pos_neg_one[i]] = map_replace_character;
                        old_x_b = old_x_b + pos_neg_one[i];
                    }
                    old_y_b = old_y;
                    old_x_b = old_x;
                }
            }
        }
    }else{
        player_replace_char = player.replace_character;
    }
    return new_rendered_map;
}

//checks to see if player is trying to move out of the map
bool check_for_edge(std::vector<std::string>  rendered_map, player_struct player){
    char test_char = rendered_map[player.posy][player.posx];
    if(test_char == '#'){
        return true;
    }else{
        return false;
    }
}

//prints out hex_map from map.h
std::vector<std::string> initial_map_setup(player_struct player){

    //map piece declaration.
    std::vector<std::string> initial_map;
    std::string hex_map_piece_1, hex_map_piece_2, hex_map_piece_3, hex_map_piece_4;

    hex_map_piece_1.append("#::******::::::::******::::::::******::::::::******::::::::******::::::::******#");
    hex_map_piece_2.append("#:*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::#");
    hex_map_piece_3.append("#*::::::::******::::::::******::::::::******::::::::******::::::::******:::::::#");
    hex_map_piece_4.append("#:*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::#");

    //fill the vector with empty spaces.
    for(int i = 0; i <= 24; i++){
        initial_map.push_back("                                                                                 ");
    }

    //print the # border in red
    int x, y;
    for(x = 0; x <= 79; x++){
        initial_map.at(0).at(x) = '#';
        initial_map.at(24).at(x) = '#';
    }
    for(y = 0; y <= 24; y++){
        initial_map.at(y).at(0) = '#';
        initial_map.at(y).at(79) = '#';
    }

    //print the pieces of the map until full
    int x_b;
    int piece_number = 1;
    for(y = 1; y < 24; y++){
        switch(piece_number){
            case 1:
                initial_map.at(y) = hex_map_piece_1;
                piece_number = 2;
                break;
            case 2:
                initial_map.at(y) = hex_map_piece_2;
                piece_number = 3;
                break;
            case 3:
                initial_map.at(y) = hex_map_piece_3;
                piece_number = 4;
                break;
            case 4:
                initial_map.at(y) = hex_map_piece_4;
                piece_number = 1;
                break;
        }
    }

    //terminates each of the lines
    for(y = 0; y <= 24; y++){
        initial_map[y][80] = '\0';
    }

    //put player in their initial position
    initial_map[player.posy][player.posx] = player.player_character;
    return initial_map;
}

//refreshes the map to show any changes that have been made
void map_refresh(std::vector<std::string>  rendered_map){
    int x, y;
    for(y = 0; y <= 24; y++){
        for(x = 0; x <= 79; x++){
            if(rendered_map[y][x] == '#'){
                attron(COLOR_PAIR(1));
                mvaddch(y, x, rendered_map[y][x]);
                attroff(COLOR_PAIR(1));
            }else if(rendered_map[y][x] == '@'){
                attron(COLOR_PAIR(2));
                mvaddch(y, x, rendered_map[y][x]);
                attroff(COLOR_PAIR(2));
            }else if(rendered_map[y][x] == '.'){
                attron(COLOR_PAIR(3));
                mvaddch(y, x, rendered_map[y][x]);
                attroff(COLOR_PAIR(3));
            }else if(rendered_map[y][x] == '*'){
                attron(COLOR_PAIR(4));
                mvaddch(y, x, rendered_map[y][x]);
                attroff(COLOR_PAIR(4));
            }else if(rendered_map[y][x] == 'e' || rendered_map[y][x] == 'l' || rendered_map[y][x] == 'p' || rendered_map[y][x] == 'a'){
                attron(COLOR_PAIR(5));
                mvaddch(y, x, rendered_map[y][x]);
                attroff(COLOR_PAIR(5));
            }else if(rendered_map[y][x] == 'Q'){
                attron(COLOR_PAIR(6));
                mvaddch(y, x, rendered_map[y][x]);
                attroff(COLOR_PAIR(6));
            }else if(rendered_map[y][x] == ':'){
                    attron(COLOR_PAIR(7));
                    mvaddch(y, x, rendered_map[y][x]);
                    attroff(COLOR_PAIR(7));
            }else{
                mvaddch(y, x, rendered_map[y][x]);
            }
        }
    }
    refresh();
}
