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

        std::vector<std::string>  rendered_map;

    protected:

    private:

        struct toggle_hex_return{
            std::vector<std::string> return_map;
            char return_player_replace_character;
        };

        int ch, total_ants;

        bool check_for_edge(std::vector<std::string>  rendered_map, player_struct player);

        int select_ant(ant_struct ant, player_struct player);

        std::string view_menu();

        std::vector<std::string> copy_map(std::vector<std::string>  map_to_copy);
        std::vector<std::string> initial_map_setup(player_struct player);

        toggle_hex_return toggle_hex_status(std::vector<std::string>  new_rendered_map, player_struct player);

        void map_refresh(std::vector<std::string>  rendered_map);
        void kill_ant();
        void view_ants(ant_struct ant, int tick);
        void save_game(std::vector<std::string>  rendered_map);
        void load_game();




};

#endif // MAIN_H
