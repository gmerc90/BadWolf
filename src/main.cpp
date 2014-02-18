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
#include <vector>
#include <string>
#include "main.h"
#include "map.h"

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
};

void initial_map_setup(char game_map[25][81], player_struct player);
void map_refresh(char game_map[25][81]);
void toggle_hex_status(char game_map[25][81], player_struct player);

void create_ant(int tick, ant_struct ant, player_struct player, char game_map[25][81]);
void kill_ant();
void view_ants(ant_struct ant, int tick, bool first_ant);

void save_game();
void load_game();

bool check_for_edge(char game_map[25][81], player_struct player);

int main(int argc, char *argv[]){

    //initial basic declarations
    int ch, tick, vector_size, total_ants;
    char game_map[25][81];
    bool first_ant;

    //standard starting stuff for Curses
    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();

    //set window size
    resizeterm(25,80);

    //color initialization
    init_pair(1, COLOR_RED, COLOR_RED);
    init_pair(2, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(3, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(4, COLOR_WHITE, COLOR_WHITE);
    init_pair(5, COLOR_BLUE, COLOR_BLUE);
    init_pair(6, COLOR_CYAN, COLOR_CYAN);

    //set initial position and initialize the player
    player_struct player;
    player.posy = LINES/2;
    player.posx = COLS/2;
    player.replace_character = ' ';

    //creates the initial ant structure and sets an ant number to one
    ant_struct ant;
    ant.number.resize(1);
    ant.number.push_back(1);
    first_ant = true;

    //show initial map
    initial_map_setup(game_map, player);
    map_refresh(game_map);

    //set initial tick value
    tick = 0;

    //main game loop
    while((ch = getch()) != KEY_F(1)){

        /*checks to see if a key was pressed,
        if one has been, it performs the necessary code to
        complete the desired action*/
        //TODO change it to 'x' miliseconds later and let it be configurable in a config file
        timeout(100);
        switch(ch){
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
                toggle_hex_status(game_map, player);
                break;
            //create ants with their initial value
            case 'c':
                if(first_ant == true){
                    vector_size = ant.number.back();
                    first_ant = false;
                }else if(first_ant == false){
                    vector_size = ant.number.back() + 1;
                    ant.number.resize(vector_size);
                    ant.number.push_back(vector_size);
                }
                ant.age.resize(vector_size);
                ant.age.push_back(0);
                ant.birth_tick.resize(vector_size);
                ant.birth_tick.push_back(tick);
                ant.character.resize(vector_size);
                ant.character.push_back('e');
                ant.type.resize(vector_size);
                ant.type.push_back("Egg");
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
            case KEY_F(2):
                view_ants(ant, tick, first_ant);
                map_refresh(game_map);
                break;
        }
        //tick the game
        tick = tick + 1;

        //after each tick updates the age of the ant then grows it if necessary
        if(first_ant == false){
            total_ants = ant.number.back();
            for(int i = 1; i <= total_ants; i++){
                ant.age.at(i) = tick - ant.birth_tick.at(i);
            }
            for(int i = 1; i <= ant.number.back(); i++){
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
            map_refresh(game_map);
        }
    }
    //terminate program
    endwin();
    return 0;
}

//if needed, kill the ant
void kill_ant(){
    //TODO make kill the ant if needed
}

//view a list of all ants when tab is pressed, alive and dead, and are able to filter the list.
void view_ants(ant_struct ant, int tick, bool first_ant){
    //declaring function variables
    WINDOW *view_ants_wind;
    int height, width, starty, startx, ch_b, total_ants, last_ant_shown, first_ant_shown;

    //ant window variable default values
    height = 20;
    width = 60;
    starty = (LINES - height)/2;
    startx = (COLS - width)/2;
    total_ants = ant.number.back();

    //creates the view ants window
    view_ants_wind = newwin(height, width, starty, startx);
    box(view_ants_wind, 0, 0);
    scrollok(view_ants_wind, TRUE);
    wrefresh(view_ants_wind);

    //checks to see if the first ant has been created, if it has been they will be printed out.
    if(first_ant == true){
        mvwprintw(view_ants_wind, 1, 1, "There are not any ants yet.");
        wrefresh(view_ants_wind);
        while((ch_b = getch()) != 'q'){

        }
    }else if( first_ant == false){
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
    }
    delwin(view_ants_wind);
}

//saves all the data from the game to a text file
void save_game(){
//TODO have function save all program data to a text file
}

//loads all data from a text file and puts it into the game
void load_game(){
//TODO have function load all data from a text file if told to
}

//change the state of the hex cube to selected or not
//FIXME for some reason the block directly above the player will not toggle off
//FIXME when toggled off, sometimes the block where the player was remains toggled on
//FIXME the block directly above the player that remains toggled on prevents the cell from correctly toggling on again
void toggle_hex_status(char game_map[25][81], player_struct player){
    bool checked_x_pos, checked_y_pos, checked_x_neg, checked_y_neg;
    checked_x_pos = false;
    checked_y_pos = false;
    checked_x_neg = false;
    checked_y_neg = false;
    int x = 1;
    int y = 0;
    //until all positive x and y and negative x and y values from the player have been checked, this loop will run.
    while(checked_x_pos != true && checked_y_pos != true && checked_x_neg
          != true && checked_y_neg != true){
        //checks to see if the player is on a *
        if(player.replace_character == '*'){
            checked_x_neg = true;
            checked_x_pos = true;
            checked_y_neg = true;
            checked_y_pos = true;
        }
        //goes through checks and replaces all values in the + y range with .
        while(checked_y_pos != true){
            switch(game_map[player.posy + y][player.posx]){

                //turn untoggled cells on
                case ' ':
                    game_map[player.posy + y][player.posx] = '.';
                    //goes through checks and replaces all values
                    //in + x range with '.' for the corresponding y value
                    while(checked_x_pos != true){
                        if(game_map[player.posy + y][player.posx + x] == ' '){
                            game_map[player.posy + y][player.posx + x] = '.';
                        }else if(game_map[player.posy + y][player.posx + x] == '*'){
                            checked_x_pos = true;
                        }else if(game_map[player.posy + y][player.posx + x] == '#'){
                            checked_x_pos = true;
                        }
                        x = x + 1;
                    }
                    checked_x_pos = false;
                    x = 1;
                    //goes through checks and replaces all values
                    //in - x range with '.' for the corresponding y value
                    while(checked_x_pos != true){
                        if(game_map[player.posy + y][player.posx - x] == ' '){
                            game_map[player.posy + y][player.posx - x] = '.';
                        }else if(game_map[player.posy + y][player.posx - x] == '*'){
                            checked_x_pos = true;
                        }else if(game_map[player.posy + y][player.posx - x] == '#'){
                            checked_x_pos = true;
                        }
                        x = x + 1;
                    }
                    player.replace_character = '.';
                    break;

                //turn toggled cells off
                case '.':
                    game_map[player.posy + y][player.posx] = ' ';
                    //goes through checks and replaces all values
                    //in + x range with '.' for the corresponding y value
                    while(checked_x_pos != true){
                        if(game_map[player.posy + y][player.posx + x] == '.'){
                            game_map[player.posy + y][player.posx + x] = ' ';
                        }else if(game_map[player.posy + y][player.posx + x] == '*'){
                            checked_x_pos = true;
                        }else if(game_map[player.posy + y][player.posx + x] == '#'){
                            checked_x_pos = true;
                        }
                        x = x + 1;
                    }
                    checked_x_pos = false;
                    x = 1;
                    //goes through checks and replaces all values
                    //in - x range with '.' for the corresponding y value
                    while(checked_x_pos != true){
                        if(game_map[player.posy + y][player.posx - x] == '.'){
                            game_map[player.posy + y][player.posx - x] = ' ';
                        }else if(game_map[player.posy + y][player.posx - x] == '*'){
                            checked_x_pos = true;
                        }else if(game_map[player.posy + y][player.posx - x] == '#'){
                            checked_x_pos = true;
                        }
                        x = x + 1;
                    }
                    break;

                case '@':
                    x = 1;
                    //goes through checks and replaces all values
                    //in + x range with '.' for the corresponding y value
                    if(game_map[player.posy + y][player.posx + x] == ' '){
                        while(checked_x_pos != true){
                            if(game_map[player.posy + y][player.posx + x] == ' '){
                                game_map[player.posy + y][player.posx + x] = '.';
                            }else if(game_map[player.posy + y][player.posx + x] == '*'){
                                checked_x_pos = true;
                            }else if(game_map[player.posy + y][player.posx + x] == '#'){
                                checked_x_pos = true;
                            }
                            x = x + 1;
                        }
                        x = 1;
                        checked_x_pos = false;
                        //goes through checks and replaces all values
                        //in - x range with '.' for the corresponding y value
                        while(checked_x_pos != true){
                            if(game_map[player.posy + y][player.posx - x] == ' '){
                                game_map[player.posy + y][player.posx - x] = '.';
                            }else if(game_map[player.posy + y][player.posx - x] == '*'){
                                checked_x_pos = true;
                            }else if(game_map[player.posy + y][player.posx - x] == '#'){
                                checked_x_pos = true;
                            }
                            x = x + 1;
                        }
                    }else if(game_map[player.posy + y][player.posx + x] == '.'){
                        while(checked_x_pos != true){
                            if(game_map[player.posy + y][player.posx + x] == '.'){
                                game_map[player.posy + y][player.posx + x] = ' ';
                            }else if(game_map[player.posy + y][player.posx + x] == '*'){
                                checked_x_pos = true;
                            }else if(game_map[player.posy + y][player.posx + x] == '#'){
                                checked_x_pos = true;
                            }
                            x = x + 1;
                        }
                        x = 1;
                        checked_x_pos = false;
                        //goes through checks and replaces all values
                        //in - x range with '.' for the corresponding y value
                        while(checked_x_pos != true){
                            if(game_map[player.posy + y][player.posx - x] == '.'){
                                game_map[player.posy + y][player.posx - x] = ' ';
                            }else if(game_map[player.posy + y][player.posx - x] == '*'){
                                checked_x_pos = true;
                            }else if(game_map[player.posy + y][player.posx - x] == '#'){
                                checked_x_pos = true;
                            }
                            x = x + 1;
                        }

                    }
                    break;

                case '*':
                    checked_y_pos = true;
                    checked_x_pos = true;
                    break;

                case '#':
                    checked_y_pos = true;
                    checked_x_pos = true;
                    break;
            }
            checked_x_pos = false;
            x = 1;
            y = y + 1;
        }
        x = 1;
        y = 0;
        //goes thorough checks and replaces all values in the - y range with .
        while(checked_y_neg != true){
            switch(game_map[player.posy - y][player.posx]){
                case ' ':
                    game_map[player.posy - y][player.posx] = '.';
                    //goes through checks and replaces all values in - x range with
                    //. for the corresponding y value
                    while(checked_x_neg != true){
                        if(game_map[player.posy - y][player.posx - x] == ' '){
                            game_map[player.posy - y][player.posx - x] = '.';
                        }else if(game_map[player.posy - y][player.posx - x] == '*'){
                            checked_x_neg = true;
                        }else if(game_map[player.posy - y][player.posx - x] == '#'){
                            checked_x_neg = true;
                        }
                        x = x + 1;
                    }
                    checked_x_neg = false;
                    x = 1;
                    //goes through checks and replaces all values in + x range with
                    //. for the corresponding y value
                    while(checked_x_neg != true){
                        if(game_map[player.posy - y][player.posx + x] == ' '){
                            game_map[player.posy - y][player.posx + x] = '.';
                        }else if(game_map[player.posy - y][player.posx + x] == '*'){
                            checked_x_neg = true;
                        }else if(game_map[player.posy - y][player.posx + x] == '#'){
                            checked_x_neg = true;
                        }
                        x = x + 1;
                    }
                    player.replace_character = '.';
                    break;

                case '.':
                    game_map[player.posy - y][player.posx] = '.';
                    //goes through checks and replaces all values in - x range with
                    //. for the corresponding y value
                    while(checked_x_neg != true){
                        if(game_map[player.posy - y][player.posx - x] == '.'){
                            game_map[player.posy - y][player.posx - x] = ' ';
                        }else if(game_map[player.posy - y][player.posx - x] == '*'){
                            checked_x_neg = true;
                        }else if(game_map[player.posy - y][player.posx - x] == '#'){
                            checked_x_neg = true;
                        }
                        x = x + 1;
                    }
                    checked_x_neg = false;
                    x = 1;
                    //goes through checks and replaces all values in + x range with
                    //. for the corresponding y value
                    while(checked_x_neg != true){
                        if(game_map[player.posy - y][player.posx + x] == '.'){
                            game_map[player.posy - y][player.posx + x] = ' ';
                        }else if(game_map[player.posy - y][player.posx + x] == '*'){
                            checked_x_neg = true;
                        }else if(game_map[player.posy - y][player.posx + x] == '#'){
                            checked_x_neg = true;
                        }
                        x = x + 1;
                    }
                    break;
                case '*':
                    checked_y_neg = true;
                    checked_x_neg = true;
                    break;
                case '#':
                    checked_y_neg = true;
                    checked_x_neg = true;
                    break;
            }
            checked_x_neg = false;
            x = 1;
            y = y + 1;
        }
    }
    map_refresh(game_map);
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
    int x2;
    int piece_number = 1;
    for(y = 1; y < 24; y++){
        switch(piece_number){
            case 1:
                for(x2 = 1; x2 < 79; x2++){
                    game_map[y][x2] = hex_map_piece_1[0][x2];
                }
                piece_number = 2;
                break;
            case 2:
                for(x2 = 1; x2 < 79; x2++){
                    game_map[y][x2] = hex_map_piece_2[0][x2];
                }
                piece_number = 3;
                break;
            case 3:
                for(x2 = 1; x2 < 79; x2++){
                    game_map[y][x2] = hex_map_piece_3[0][x2];
                }
                piece_number = 4;
                break;
            case 4:
                for(x2 = 1; x2 < 79; x2++){
                    game_map[y][x2] = hex_map_piece_4[0][x2];
                }
                piece_number = 1;
                break;
        }
    }

    //put player in their initial position
    game_map[player.posy][player.posx] = player.player_character;
}

//refreshes the map to show any changes that have been made
void map_refresh(char game_map[25][81]){
    int x, y;
    for(y = 0; y <=24; y++){
        for(x = 0; x <=79; x++){
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
            }else if(game_map[y][x] == 'e'){
                attron(COLOR_PAIR(5));
                mvaddch(y, x, game_map[y][x]);
                attroff(COLOR_PAIR(5));
            }else if(game_map[y][x] == 'l'){
                attron(COLOR_PAIR(6));
                mvaddch(y, x, game_map[y][x]);
                attroff(COLOR_PAIR(6));
            }else if(game_map[y][x] == 'p'){
                attron(COLOR_PAIR(5));
                mvaddch(y, x, game_map[y][x]);
                attroff(COLOR_PAIR(5));
            }else if(game_map[y][x] == 'a'){
                attron(COLOR_PAIR(6));
                mvaddch(y, x, game_map[y][x]);
                attroff(COLOR_PAIR(6));
            }else{
                mvaddch(y, x, game_map[y][x]);
            }
        }
    }
    refresh();
}
