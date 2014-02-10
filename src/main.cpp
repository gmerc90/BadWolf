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

void render_hex_map();
void destroy_window(WINDOW *local_win);

int check_for_edge();

WINDOW *create_player_window(int height, int width, int startx, int starty);

int main(int argc, char *argv[]){
    WINDOW *player_window;
    int ch;
    int height, width, starty, startx;

    //standard starting stuff for Curses
    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();

    //show initial map
    render_hex_map();

    //set initial player window values
    height = 1;
    width = 1;
    starty = (LINES - height)/2;
    startx = (COLS - width)/2;

    //display player window
    player_window = create_player_window(height, width, starty, startx);


    //check for movement keys
    while((ch = getch()) != KEY_F(1)){
        switch(ch){
            case KEY_LEFT:
                destroy_window(player_window);
                player_window = create_player_window(height, width, starty, --startx);
                break;
            case KEY_RIGHT:
                destroy_window(player_window);
                player_window = create_player_window(height, width, starty, ++startx);
                break;
            case KEY_UP:
                destroy_window(player_window);
                player_window = create_player_window(height, width, --starty, startx);
                break;
            case KEY_DOWN:
                destroy_window(player_window);
                player_window = create_player_window(height, width, ++starty, startx);
                break;
        }
    }

    //terminate program
    endwin();
    return 0;
}

//draws player window
WINDOW *create_player_window(int height, int width, int starty, int startx){
    WINDOW *local_win;

    local_win = newwin(height, width, starty, startx);
    box(local_win, 0 , 0);
    wprintw(local_win, "@");

    wrefresh(local_win);
    return local_win;
}

//checks to see if player is trying to move out of the map

int check_for_edge(){

}

//destroys player window
void destroy_window(WINDOW *local_win){
    wborder(local_win,' ',' ',' ',' ',' ',' ',' ',' ');
    wrefresh(local_win);
    delwin(local_win);
}

//prints out hex_map from map.h
void render_hex_map(){
    int x, y;
    init_pair(1, COLOR_RED, COLOR_BLACK);
    for(y = 0; y <= 25; y++){
        for(x = 0; x <= 80; x++ ){
            if(hex_map[y][x] == '#'){
                attron(COLOR_PAIR(1));
                mvaddch(y, x, hex_map[y][x]);
                attroff(COLOR_PAIR(1));
            }else
                mvaddch(y, x, hex_map[y][x]);
        }
    }
    refresh();
}
