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

bool check_for_edge(char game_map[25][81], player_struct player);

void initial_map_setup(char game_map[25][81], player_struct player);
void map_refresh(char game_map[25][81]);
void toggle_hex_status(char game_map[25][81], player_struct player);

int main(int argc, char *argv[]){
    int ch;
    char game_map[25][81];

    //standard starting stuff for Curses
    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();

    //color initialization
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_MAGENTA);

    //set initial position and initialize the player
    player_struct player;
    player.posy = LINES/2;
    player.posx = COLS/2;

    //show initial map
    initial_map_setup(game_map, player);
    map_refresh(game_map);

    //main game loop
    while((ch = getch()) != KEY_F(1)){

        /*checks to see if a key was pressed,
        if one has been, it performs the necessary code to
        complete the desired action*/
        switch(ch){
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
            //select a cell
            case 'e':
                toggle_hex_status(game_map, player);
                map_refresh(game_map);
                break;
        }
    }

    //terminate program
    endwin();
    return 0;
}

//change the state of the hex cube to selected or not
void toggle_hex_status(char game_map[25][81], player_struct player){
    bool checked_x_pos, checked_y_pos, checked_x_neg, checked_y_neg;
    checked_x_pos = false;
    checked_y_pos = false;
    checked_x_neg = false;
    checked_y_neg = false;
    int x = 1;
    int y = 0;
    //until all positive x and y and negative x and y values from the player have been checked, this loop will run
    while(checked_x_pos != true && checked_y_pos != true && checked_x_neg != true && checked_y_neg != true){
        //goes through checks and replaces all values in the + y range with .
        while(checked_y_pos != true){
            if(game_map[player.posy + y][player.posx] == ' '){
                game_map[player.posy + y][player.posx] = '.';
                //goes through checks and replaces all values in + x range with . for the corresponding y value
                while(checked_x_pos != true){
                    if(game_map[player.posy + y][player.posx + x] == ' '){
                        game_map[player.posy + y][player.posx + x] = '.';
                    }else if(game_map[player.posy + y][player.posx + x] == '*'){
                        checked_x_pos = true;
                    }
                    x = x + 1;
                }
                checked_x_pos = false;
                x = 1;
                //goes through checks and replaces all values in - x range with . for the corresponding y value
                while(checked_x_pos != true){
                    if(game_map[player.posy + y][player.posx - x] == ' '){
                        game_map[player.posy + y][player.posx - x] = '.';
                    }else if(game_map[player.posy + y][player.posx - x] == '*'){
                        checked_x_pos = true;
                    }
                    x = x + 1;
                }
            }else if(game_map[player.posy + y][player.posx]== '@'){
                x = 1;
                //checks the + x values in the player row and replaces with .
                while(checked_x_pos != true){
                    if(game_map[player.posy + y][player.posx + x] == ' '){
                        game_map[player.posy + y][player.posx + x] = '.';
                    }else if(game_map[player.posy + y][player.posx + x] == '*'){
                        checked_x_pos = true;
                    }
                    x = x + 1;
                }
                x = 1;
                checked_x_pos = false;
                //checks the - x values in the player row and replaces with .
                while(checked_x_pos != true){
                    if(game_map[player.posy + y][player.posx - x] == ' '){
                        game_map[player.posy + y][player.posx - x] = '.';
                    }else if(game_map[player.posy + y][player.posx - x] == '*'){
                        checked_x_pos = true;
                    }
                    x = x + 1;
                }
            }else if(game_map[player.posy + y][player.posx] == '*'){
                checked_y_pos = true;
                checked_x_pos = true;
            }
            checked_x_pos = false;
            x = 1;
            y = y + 1;
        }
        x = 1;
        y = 0;
        //goes thorough checks and replaces all values in the - y range with .
        while(checked_y_neg != true){
            if(game_map[player.posy - y][player.posx] == ' '){
                game_map[player.posy - y][player.posx] = '.';
                //goes through checks and replaces all values in - x range with . for the corresponding y value
                while(checked_x_neg != true){
                    if(game_map[player.posy - y][player.posx - x] == ' '){
                        game_map[player.posy - y][player.posx - x] = '.';
                    }else if(game_map[player.posy - y][player.posx - x] == '*'){
                        checked_x_neg = true;
                    }
                    x = x + 1;
                }
                checked_x_neg = false;
                x = 1;
                //goes through checks and replaces all values in + x range with . for the corresponding y value
                while(checked_x_neg != true){
                    if(game_map[player.posy - y][player.posx + x] == ' '){
                        game_map[player.posy - y][player.posx + x] = '.';
                    }else if(game_map[player.posy - y][player.posx + x] == '*'){
                        checked_x_neg = true;
                    }
                    x = x + 1;
                }
            }else if(game_map[player.posy - y][player.posx]== '@'){
                //checks the - x values in the player row and replaces with .
                while(checked_x_neg != true){
                    if(game_map[player.posy - y][player.posx + x] == ' '){
                        game_map[player.posy - y][player.posx + x] = '.';
                    }else if(game_map[player.posy - y][player.posx + x] == '*'){
                        checked_x_neg = true;
                    }
                    x = x + 1;
                }
            }else if(game_map[player.posy - y][player.posx] == '*'){
                checked_y_neg = true;
                checked_x_neg = true;
            }
            checked_x_neg = false;
            x = 1;
            y = y + 1;
        }
    }
    player.replace_character = '.';
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
            }else{
                mvaddch(y, x, game_map[y][x]);
            }
        }
    }
    refresh();
}
