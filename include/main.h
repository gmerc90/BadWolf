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

#ifndef MAIN_H
#define MAIN_H


class main
{
    public:

        struct player_struct{
            int posy;
            int posx;
            int old_posy;
            int old_posx;
            char player_character = '@';
            char replace_character;
            char replace_character_new;
        };
        char game_map[25][81];

    protected:

    private:

        void initial_map_setup(char game_map[25][81], player_struct player);
        void map_refresh(char game_map[25][81]);
        void toggle_hex_status(char game_map[25][81], player_struct player);

        bool toggle_hex_on(char game_map[25][81], player_struct player);




};

#endif // MAIN_H
