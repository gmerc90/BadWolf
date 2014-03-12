    /*
    BadWolf
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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

#ifndef MAIN_H
#define MAIN_H

const char* choices[] = {"Save", "Load", "Quit", "Close"};
const char* menuTitle = "Game Menu";

class main
{
    public:

        //structs
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

        struct digReturn{
            antStruct returnAnt;
            std::vector<std::string> returnMap;
        };

        struct moveCursorReturn{
            cursorStruct returnCursor;
            std::vector<std::string> returnMap;
        };

        struct createAntReturn{
            antStruct returnAnt;
            std::vector<std::string> returnMap;
        };

        //variables
        int tick, selectedAnt;
        std::vector<std::string>  renderedMap, undergroundMap, surfaceMap;

    protected:

    private:

        //variables
        int ch, vectorSize, totalAnts;
        moveAntReturn moveAntReturnValues;
        moveCursorReturn moveCursorReturnValues;
        std::string chosenMenuOption, currentMap;
        digReturn digReturnValues;

        //functions
        bool checkForGround(antStruct ant, std::vector<std::string> renderedMap, cursorStruct cursor, int selectedAnt);
        createAntReturn createAnt(antStruct ant, std::vector<std::string> renderedMap, cursorStruct cursor, int tick, std::string currentMap);
        digReturn undergroundDig(std::vector<std::string>  renderedMap, antStruct ant, int selectedAnt);
        digReturn surfaceDig(std::vector<std::string> renderedMap, std::vector<std::string> undergroundMap, antStruct ant, int selectedAnt);
        int selectAnt(antStruct ant, cursorStruct cursor, std::string currentMap);
        moveAntReturn moveSelectedAnt(int selected_ant, antStruct ant, cursorStruct cursor, std::string currentMap,
                                       std::vector<std::string> renderedMap, std::vector<std::string> surfaceMap, std::vector<std::string> undergroundMap);
        moveCursorReturn moveCursor(int moveValueY, int moveValueX, std::vector<std::string> renderedMap, cursorStruct cursor);
        std::string viewMenu();
        std::vector<int> findSurface(std::vector<std::string> renderedMap, cursorStruct cursor);
        std::vector<std::string> initialSurfaceMapSetup(cursorStruct cursor);
        std::vector<std::string> initialMapSetup(cursorStruct cursor);
        void displayInfoWindow();
        void killAnt();
        void loadGame();
        void refreshMap(std::vector<std::string>  renderedMap, cursorStruct cursor);
        void saveGame(std::vector<std::string>  renderedMap, std::vector<std::string> surfaceMap, std::vector<std::string> undergroundMap);
        void statusAreaPrint(antStruct ant, int selectedAnt);
        void viewAnts(antStruct ant, int tick);
};

#endif // MAIN_H
