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

        int tick;

        char game_map[25][81];

        void save();
        void load();

    protected:

    private:
        int ch, total_ants;

        void initial_map_setup(char game_map[25][81], player_struct player);
        void map_refresh(char game_map[25][81]);
        char toggle_hex_status(char game_map[25][81], player_struct player);
        void kill_ant();
        void view_ants(ant_struct ant, int tick);

        void save_game(char game_map[25][81], char derp[2][6]);
        void load_game();

        bool check_for_edge(char game_map[25][81], player_struct player);

        std::string view_menu();



};

#endif // MAIN_H
