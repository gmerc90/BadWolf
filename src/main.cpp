/*Copyright (c) 2014, Gary Mercado(errdividedby0)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of errdivideby0 nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL errdivideby0 BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/

#include <ncurses.h>
#include "main.h"
#include "map.h"

struct player_struct{
    int posy;
    int posx;
    int old_posy;
    int old_posx;
    char figure = '@';
    char old_tile;
};

bool check_for_edge(char game_map[25][81]);

void initial_map_setup(char game_map[25][81], player_struct player);
void map_refresh(char game_map[25][81]);

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

    //set initial position and initialize the player
    player_struct player;
    player.posy = LINES/2;
    player.posx = COLS/2;

    //show initial map
    initial_map_setup(game_map, player);
    map_refresh(game_map);

    //main game loop
    while((ch = getch()) != KEY_F(1)){
        switch(ch){
            case KEY_LEFT:
                player.old_posx = player.posx;
                player.old_posy = player.posy;
                game_map[player.old_posy][player.old_posx] = NULL;
                game_map[player.posy][player.posx--] = player.figure;
                break;
            case KEY_RIGHT:
                player.old_posx = player.posx;
                player.old_posy = player.posy;
                game_map[player.old_posy][player.old_posx] = NULL;
                game_map[player.posy][player.posx++] = player.figure;
                break;
            case KEY_UP:
                player.old_posx = player.posx;
                player.old_posy = player.posy;
                game_map[player.old_posy][player.old_posx] = NULL;
                game_map[player.posy--][player.posx] = player.figure;
                break;
            case KEY_DOWN:
                player.old_posx = player.posx;
                player.old_posy = player.posy;
                game_map[player.old_posy][player.old_posx] = NULL;
                game_map[player.posy++][player.posx] = player.figure;
                break;
        }
        map_refresh(game_map);
    }

    //terminate program
    endwin();
    return 0;
}

//checks to see if player is trying to move out of the map
bool check_for_edge(char game_map[25][81]){
    return false;
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
    game_map[player.posy][player.posx] = player.figure;
}

void map_refresh(char game_map[25][81]){
    int x, y;
    for(y = 0; y <=24; y++){
        for(x = 0; x <=79; x++){
            if(game_map[y][x] == '#'){
                attron(COLOR_PAIR(1));
                mvaddch(y, x, game_map[y][x]);
                attroff(COLOR_PAIR(1));
            }else{
                mvaddch(y, x, game_map[y][x]);
            }
        }
    }
}
