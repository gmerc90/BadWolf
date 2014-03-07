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

        struct cursorStruct{
            int posY;
            int posX;
        };

        struct antStruct{
            std::vector<int> number;
            std::vector<int> birthTick;
            std::vector<int> age;
            std::vector<int> posY;
            std::vector<int> posX;
            std::vector<int> oldPosY;
            std::vector<int> oldPosX;
            std::vector<bool> wings;
            std::vector<char> holding;
            std::vector<char> character;
            std::vector<char> replaceCharacter;
            std::vector<char> replaceCharacterOld;
            std::vector<std::string> type;
        };

        struct moveAntReturn{
            antStruct returnAnt;
            std::vector<std::string> returnMap;
        };

        int tick, selectedAnt;

        std::vector<std::string>  renderedMap, undergroundMap, surfaceMap;

        moveAntReturn moveSelectedAnt(int selected_ant, antStruct ant, cursorStruct cursor, std::vector<std::string> renderedMap);

    protected:

    private:

        int ch, vectorSize, totalAnts;

        bool checkForEdge(std::vector<std::string>  renderedMap, cursorStruct cursor);

        moveAntReturn moveAntReturnValues;

        std::string chosenMenuOption, currentMap;

        int selectAnt(antStruct ant, cursorStruct cursor);

        std::string viewMenu();

        std::vector<std::string> copyMap(std::vector<std::string>  mapToCopy);
        std::vector<std::string> initialMapSetup(cursorStruct cursor);
        std::vector<std::string> toggleHexStatus(std::vector<std::string>  newMap, cursorStruct cursor);

        void refreshMap(std::vector<std::string>  renderedMap, cursorStruct cursor);
        void killAnt();
        void viewAnts(antStruct ant, int tick);
        void saveGame(std::vector<std::string>  renderedMap);
        void loadGame();




};

#endif // MAIN_H
