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

#include <curses.h>
#include <vector>
#include <string>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "main.h"

#define TERM_Y_SIZE 30
#define TERM_X_SIZE 80

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
    std::vector<std::string> location;
    std::vector<std::vector<std::string> > availableCommands;
};

struct cursorStruct{
    int posY;
    int posX;
};

struct moveAntReturn{
    antStruct returnAnt;
    std::vector<std::string> returnMap;
    std::vector<std::string> returnSurfaceMap;
    std::vector<std::string> returnUndergroundMap;
};

struct digReturn{
    antStruct returnAnt;
    std::vector<std::string> returnMap;
    std::vector<std::string> returnUndergroundMap;
};

struct moveCursorReturn{
    cursorStruct returnCursor;
    std::vector<std::string> returnMap;
};

struct createAntReturn{
    antStruct returnAnt;
    std::vector<std::string> returnMap;
};

createAntReturn createAnt(antStruct ant, std::vector<std::string> renderedMap, cursorStruct cursor, int tick, std::string currentMap);
digReturn undergroundDig(std::vector<std::string>  renderedMap, antStruct ant, int selectedAnt);
digReturn surfaceDig(std::vector<std::string> renderedMap, std::vector<std::string> undergroundMap, antStruct ant, int selectedAnt);
int selectAnt(antStruct ant, cursorStruct cursor, std::string currentMap);
moveAntReturn moveSelectedAnt(int selectedAnt, antStruct ant, cursorStruct cursor, std::string currentMap,
                               std::vector<std::string> renderedMap, std::vector<std::string> surfaceMap, std::vector<std::string> undergroundMap);
moveCursorReturn moveCursor(int moveValueY, int moveValueX, std::vector<std::string> renderedMap, cursorStruct cursor);
std::string viewMenu();
std::vector<std::string> initialSurfaceMapSetup(cursorStruct cursor);
std::vector<std::string> initialUndergroundMapSetup(cursorStruct cursor);
void displayInfoWindow();
void killAnt();
void loadGame();
void refreshMap(std::vector<std::string>  renderedMap, cursorStruct cursor);
void saveGame(std::vector<std::string>  renderedMap, std::vector<std::string> surfaceMap, std::vector<std::string> undergroundMap);
void statusAreaPrint(antStruct ant, int selectedAnt);
void viewAnts(antStruct ant, int tick);

int main(){

    //set the set to the time for rand
    srand(time(NULL));

    //initial variable declarations
    createAntReturn createAntReturnValues;
    digReturn digReturnValues;
    int ch, tick, totalAnts;
    int selectedAnt = NULL;
    moveAntReturn moveAntReturnValues;
    moveCursorReturn moveCursorReturnValues;
    std::string chosenMenuOption, currentMap;
    std::vector<std::string>  renderedMap, undergroundMap, surfaceMap;

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
    init_pair(2, COLOR_WHITE, COLOR_YELLOW);
    init_pair(3, COLOR_MAGENTA, COLOR_MAGENTA);
    init_pair(4, COLOR_WHITE, COLOR_WHITE);
    init_pair(5, COLOR_GREEN, COLOR_BLACK);
    init_pair(6, COLOR_RED, COLOR_BLACK);
    init_pair(7, COLOR_WHITE, COLOR_GREEN);


    //set initial position and initialize the cursor
    cursorStruct cursor;
    cursor.posY = (LINES - 5) / 2;
    cursor.posX = COLS / 2;

    //show initial map
    undergroundMap = initialUndergroundMapSetup(cursor);
    surfaceMap = initialSurfaceMapSetup(cursor);
    currentMap = "Surface";
    renderedMap = surfaceMap;
    refreshMap(renderedMap, cursor);


    //set initial tick value
    tick = 0;

    //creates the initial ant structure and the queen
    antStruct ant;
    ant.number.resize(1);
    ant.number.push_back(1);
    ant.age.resize(1);
    ant.age.push_back(0);
    ant.birthTick.resize(1);
    ant.birthTick.push_back(tick);
    ant.character.resize(1);
    ant.character.push_back('Q');
    ant.replaceCharacter.resize(1);
    ant.replaceCharacter.push_back(' ');
    ant.type.resize(1);
    ant.type.push_back("Queen");
    ant.wings.resize(1);
    ant.wings.push_back(true);
    ant.posX.resize(1);
    ant.posX.push_back(COLS / 2);
    ant.posY.resize(1);
    ant.posY.push_back(19);
    ant.location.resize(1);
    ant.location.push_back(currentMap);
    ant.availableCommands.resize(2);
    ant.availableCommands.at(1).resize(1);
    ant.availableCommands.at(1).at(0).append("Move");
    ant.availableCommands.at(1).push_back("Dig");
    ant.availableCommands.at(1).push_back("Eat Wings");
    renderedMap[ant.posY.at(1)][ant.posX.at(1)] = ant.character.at(1);
    refreshMap(renderedMap, cursor);

    //main game loop
    while(chosenMenuOption != "Quit"){

        //checks to see if a key was pressed,
        //if one has been, it performs the necessary code to
        //complete the desired action
        ///TODO change it to 'x' miliseconds later and let it be configurable in a config file
        timeout(100);
        switch(ch = getch()){

            //move up
            case KEY_UP:
                moveCursorReturnValues = moveCursor(-1, 0, renderedMap, cursor);
                renderedMap = moveCursorReturnValues.returnMap;
                cursor = moveCursorReturnValues.returnCursor;
                refreshMap(renderedMap, cursor);
                break;

            //move left
            case KEY_LEFT:
                moveCursorReturnValues = moveCursor(0, -1, renderedMap, cursor);
                renderedMap = moveCursorReturnValues.returnMap;
                cursor = moveCursorReturnValues.returnCursor;
                refreshMap(renderedMap, cursor);
                break;

            //move down
            case KEY_DOWN:
                moveCursorReturnValues = moveCursor(1, 0, renderedMap, cursor);
                renderedMap = moveCursorReturnValues.returnMap;
                cursor = moveCursorReturnValues.returnCursor;
                refreshMap(renderedMap, cursor);
                break;

            //move right
            case KEY_RIGHT:
                moveCursorReturnValues = moveCursor(0, 1, renderedMap, cursor);
                renderedMap = moveCursorReturnValues.returnMap;
                cursor = moveCursorReturnValues.returnCursor;
                refreshMap(renderedMap, cursor);
                break;

            //select an ant
            case 's':
                selectedAnt = selectAnt(ant, cursor, currentMap);
                break;

            //clear selected ant with Shift + e
            case 'S':
                selectedAnt = NULL;
                break;

            //move the selected ant to the location of the cursor
            case 'm':
                if((selectedAnt != NULL) && (renderedMap[cursor.posY][cursor.posX] == ' ')){
                    moveAntReturnValues = moveSelectedAnt(selectedAnt, ant, cursor, currentMap, renderedMap, surfaceMap, undergroundMap);
                    renderedMap = moveAntReturnValues.returnMap;
                    surfaceMap = moveAntReturnValues.returnSurfaceMap;
                    undergroundMap = moveAntReturnValues.returnUndergroundMap;
                    ant = moveAntReturnValues.returnAnt;
                    refreshMap(renderedMap, cursor);
                }
                break;

            //dig into a filled space
            case 'd':
                if(selectedAnt != NULL){
                    moveAntReturnValues = moveSelectedAnt(selectedAnt, ant, cursor, currentMap, renderedMap, surfaceMap, undergroundMap);
                    renderedMap = moveAntReturnValues.returnMap;
                    surfaceMap = moveAntReturnValues.returnSurfaceMap;
                    undergroundMap = moveAntReturnValues.returnUndergroundMap;
                    ant = moveAntReturnValues.returnAnt;
                    if(currentMap == "Underground"){
                        digReturnValues = undergroundDig(renderedMap, ant, selectedAnt);
                        renderedMap = digReturnValues.returnMap;
                        ant = digReturnValues.returnAnt;
                        refreshMap(renderedMap, cursor);
                    }else if((currentMap == "Surface") && (renderedMap[ant.posY.at(selectedAnt) + 1][ant.posX.at(selectedAnt)] == '=')){
                        digReturnValues = surfaceDig(renderedMap, undergroundMap, ant, selectedAnt);
                        renderedMap = digReturnValues.returnMap;
                        ant = digReturnValues.returnAnt;
                        undergroundMap = digReturnValues.returnUndergroundMap;
                        refreshMap(renderedMap, cursor);
                    }
                }
                break;

            //create ants with their initial value
            case 'c':
                createAntReturnValues = createAnt(ant, renderedMap, cursor, tick, currentMap);
                ant = createAntReturnValues.returnAnt;
                renderedMap = createAntReturnValues.returnMap;
                refreshMap(renderedMap, cursor);
                break;

            //bring up game menu
            case KEY_F(1):
                chosenMenuOption = viewMenu();
                refreshMap(renderedMap, cursor);
                //take the appropriate actions for the chosen menu option.
                if(chosenMenuOption == "Save"){
                    if(currentMap == "Surface"){
                        surfaceMap = renderedMap;
                    }else if(currentMap == "Underground"){
                        undergroundMap = renderedMap;
                    }
                    saveGame(renderedMap, surfaceMap, undergroundMap);
                    chosenMenuOption = " ";
                }else if(chosenMenuOption =="Load"){
                    loadGame();
                    chosenMenuOption = " ";
                }else if(chosenMenuOption == "Close"){
                    chosenMenuOption = " ";
                }
                break;

            //view ants window
            case KEY_F(2):
                viewAnts(ant, tick);
                refreshMap(renderedMap, cursor);
                break;

            //toggle view between the surface and underground
            case KEY_F(3):
                if(currentMap == "Surface"){
                    surfaceMap = renderedMap;
                    renderedMap = undergroundMap;
                    currentMap = "Underground";
                }else if(currentMap == "Underground"){
                    undergroundMap = renderedMap;
                    renderedMap = surfaceMap;
                    currentMap = "Surface";
                }
                refreshMap(renderedMap, cursor);
                break;

            //display an information window
            case KEY_F(4):
                displayInfoWindow();
                refreshMap(renderedMap, cursor);
                break;
        }
        //tick the game
        tick = tick + 1;

        //after each tick updates the age of the ant then grows it if necessary
        totalAnts = ant.number.back();
        for(int i = 1; i <= totalAnts; i++){
            ant.age.at(i) = tick - ant.birthTick.at(i);
        }
        for(int i = 1; i <= totalAnts; i++){
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
            }
            if(ant.location.at(i) == currentMap){
                renderedMap[ant.posY.at(i)][ant.posX.at(i)] = ant.character.at(i);
            }
        }
        refreshMap(renderedMap, cursor);

        //show Status Area information
        statusAreaPrint(ant, selectedAnt);
    }
    //terminate program
    endwin();
    return 0;
}
createAntReturn createAnt(antStruct ant, std::vector<std::string> renderedMap, cursorStruct cursor, int tick, std::string currentMap){

    //variable declarations
    bool antExists = false;
    createAntReturn returnValues;
    int vectorSize;

    //checks to see if an ant already exists at the location
    for(int i = 0; i <= ant.number.back(); i++){
        if((cursor.posY == ant.posY.at(i)) && (cursor.posX == ant.posX.at(i))){
            antExists = true;
        }
    }
    //if an ant does not already exists at the location, an ant will be created
    if((antExists == false) && (renderedMap[cursor.posY][cursor.posX] == ' ')){
        if(renderedMap[cursor.posY][cursor.posX] != '*'){
            vectorSize = ant.number.back() + 1;
            ant.number.push_back(vectorSize);
            ant.age.push_back(0);
            ant.birthTick.push_back(tick);
            ant.character.push_back('e');
            ant.replaceCharacter.push_back(renderedMap[cursor.posY][cursor.posX]);
            ant.type.push_back("Egg");
            ant.wings.push_back(false);
            ant.posX.push_back(cursor.posX);
            ant.posY.push_back(cursor.posY);
            ant.location.push_back(currentMap);
            ant.availableCommands.resize(vectorSize + 1);
            ant.availableCommands.at(vectorSize).resize(1);
            ant.availableCommands.at(vectorSize).at(0).append("Move");
            ant.availableCommands.at(vectorSize).push_back("Dig");
            renderedMap[ant.posY.at(vectorSize)][ant.posX.at(vectorSize)] = ant.character.at(vectorSize);
        }
    }
    //bundle the return values then return them
    returnValues.returnAnt = ant;
    returnValues.returnMap = renderedMap;
    return returnValues;
}

//change the state of the hex cube to selected or not
digReturn undergroundDig(std::vector<std::string>  renderedMap, antStruct ant, int selectedAnt){

    //variable declarations
    digReturn returnValue;
    char mapReplaceCharacter = ' ';
    int posYChange[] = { -1, 0, 1, 0};
    int posXChange[] = { 0, 1, 0, -1};
    int posNegOne[] = { 1, -1};

    //replace spaces
    for(int i = 0; i <= 3; i++){
        int y = ant.posY.at(selectedAnt);
        int x = ant.posX.at(selectedAnt);
        int yB, xB;
        while((renderedMap[y + posYChange[i]][x + posXChange[i]] != '*') && (renderedMap[y + posYChange[i]][x + posXChange[i]] != '#')){
            y = y + posYChange[i];
            x = x + posXChange[i];
            renderedMap[y][x] = mapReplaceCharacter;
            yB = y;
            xB = x;
            for(int i = 0; i <= 1; i++){
                while((renderedMap[yB + posNegOne[i]][xB] != '*') && (renderedMap[yB + posNegOne[i]][xB] != '#')){
                    yB = yB + posNegOne[i];
                    renderedMap[yB][xB] = mapReplaceCharacter;
                }
                yB = y;
                while((renderedMap[yB][xB + posNegOne[i]] != '*') && (renderedMap[yB][xB + posNegOne[i]] != '#')){
                    xB = xB + posNegOne[i];
                    renderedMap[yB][xB] = mapReplaceCharacter;
                }
                yB = y;
                xB = x;
            }
        }
    }

    //set return values and then return them to the main function
    ant.replaceCharacter.at(selectedAnt) = mapReplaceCharacter;
    renderedMap[ant.posY.at(selectedAnt)][ant.posX.at(selectedAnt)] = ant.character.at(selectedAnt);

    returnValue.returnAnt = ant;
    returnValue.returnMap = renderedMap;
    //returnValue.returnUndergroundMap = NULL;

    return returnValue;
}

//dig on the surface
digReturn surfaceDig(std::vector<std::string> renderedMap, std::vector<std::string> undergroundMap, antStruct ant, int selectedAnt){
    //variable declaration
    char digReplaceChar = ' ';
    char replaceChar = ':';
    int digChangeValues[] = {0, 1, -1};
    int replaceValues[] = {2, -2, 3, -3, 4, -4, 5, -5};
    int posYChange[] = { -1, 0, 1, 0};
    int posXChange[] = { 0, 1, 0, -1};
    int posNegOne[] = { 1, -1};
    int x, y, j;
    digReturn returnValues;

    x = ant.posX.at(selectedAnt);
    y = ant.posY.at(selectedAnt);

    //digs the hole
    while(renderedMap[y + 1][x] != '#'){
        y = y + 1;
        for(int i = 0; i <= 2; i++ ){
            renderedMap[y][x + digChangeValues[i]] = digReplaceChar;
        }
    }
    y = ant.posY.at(selectedAnt) + 1;
    j = 7;

    //makes the ant hill
    while(j > 0){
        for(int i = 0; i <= j; i++){
            renderedMap[y][x + replaceValues[i]] = replaceChar;
        }
        j = j - 2;
        y = y - 1;
    }

    //clear the space underground at the appropriate location
    y = 1;
    if(undergroundMap[y][x] == '*'){
        y = y+1;
    }
    for(int i = 0; i <= 3; i++){
        int yB, xB;
        while((undergroundMap[y + posYChange[i]][x + posXChange[i]] != '*') && (undergroundMap[y + posYChange[i]][x + posXChange[i]] != '#')){
            y = y + posYChange[i];
            x = x + posXChange[i];
            undergroundMap[y][x] = digReplaceChar;
            yB = y;
            xB = x;
            for(int i = 0; i <= 1; i++){
                while((undergroundMap[yB + posNegOne[i]][xB] != '*') && (undergroundMap[yB + posNegOne[i]][xB] != '#')){
                    yB = yB + posNegOne[i];
                    undergroundMap[yB][xB] = digReplaceChar;
                }
                yB = y;
                while((undergroundMap[yB][xB + posNegOne[i]] != '*') && (undergroundMap[yB][xB + posNegOne[i]] != '#')){
                    xB = xB + posNegOne[i];
                    undergroundMap[yB][xB] = digReplaceChar;
                }
                yB = y;
                xB = x;
            }
        }
    }

    //bundle the return values then return them
    returnValues.returnAnt = ant;
    returnValues.returnMap = renderedMap;
    returnValues.returnUndergroundMap = undergroundMap;
    return returnValues;
}

//select an ant to give commands to
int selectAnt(antStruct ant, cursorStruct cursor, std::string currentMap){
    //declaring variables
    int selectedAnt, totalAnts;

    //check all the ants until it finds one that matches the cursor coordinates
    totalAnts = ant.number.back();
    for(int i = 0; i <= totalAnts; i++){
        if((ant.posY[i] == cursor.posY) && (ant.posX[i] == cursor.posX) && (ant.location[i] == currentMap)){
            selectedAnt = i;
            break;
        }else{
            selectedAnt = NULL;
        }
    }
    return selectedAnt;
}

//takes the selected ant a moves it to the desired location.
moveAntReturn moveSelectedAnt(int selectedAnt, antStruct ant, cursorStruct cursor, std::string currentMap,
                               std::vector<std::string> renderedMap, std::vector<std::string> surfaceMap, std::vector<std::string> undergroundMap){
    //declare the initial values
    int travelY, travelX;
    moveAntReturn returnData;
    travelX = 0;
    travelY = 0;

    //find the difference in distance between the cursor and the ant
    //if on surface, only move horizontally unless the space is clear.
    if(currentMap == "Surface"){
        if(renderedMap[cursor.posY][cursor.posX] == ' '){
            travelY = cursor.posY - ant.posY.at(selectedAnt);
        }
    }else if(currentMap == "Underground"){
        travelY = cursor.posY - ant.posY.at(selectedAnt);
    }
    travelX = cursor.posX - ant.posX.at(selectedAnt);

    //move the ant to the desired location
    if(renderedMap[cursor.posY][cursor.posX] != '*'){
        if(ant.location.at(selectedAnt) != currentMap){
            if(ant.location.at(selectedAnt) == "Surface"){
                surfaceMap[ant.posY.at(selectedAnt)][ant.posX.at(selectedAnt)] = ant.replaceCharacter.at(selectedAnt);
                ant.location.at(selectedAnt) = "Underground";
            }else if(ant.location.at(selectedAnt) == "Underground"){
                undergroundMap[ant.posY.at(selectedAnt)][ant.posX.at(selectedAnt)] = ant.replaceCharacter.at(selectedAnt);
                ant.location.at(selectedAnt) = "Surface";
            }
        }else if(ant.location.at(selectedAnt) == currentMap){
            renderedMap[ant.posY.at(selectedAnt)][ant.posX.at(selectedAnt)] = ant.replaceCharacter.at(selectedAnt);
        }
        ant.posY.at(selectedAnt) = ant.posY.at(selectedAnt) + travelY;
        ant.posX.at(selectedAnt) = ant.posX.at(selectedAnt) + travelX;
        ant.replaceCharacter.at(selectedAnt) = renderedMap[ant.posY.at(selectedAnt)][ant.posX.at(selectedAnt)];
        renderedMap[ant.posY.at(selectedAnt)][ant.posX.at(selectedAnt)] = ant.character.at(selectedAnt);
    }

    //set the structure values to the appropriate values and then return them
    returnData.returnMap = renderedMap;
    returnData.returnSurfaceMap = surfaceMap;
    returnData.returnUndergroundMap = undergroundMap;
    returnData.returnAnt = ant;
    return returnData;
}

moveCursorReturn moveCursor(int moveValueY, int moveValueX, std::vector<std::string> renderedMap, cursorStruct cursor){

    //variable declarations
    moveCursorReturn returnValues;

    //checks to see if value is x or y direction, then checks to
    //make sure you're not trying to move off the map before completing the move
    if(renderedMap[cursor.posY + moveValueY][cursor.posX + moveValueX] != '#'){
        cursor.posY = cursor.posY + moveValueY;
        cursor.posX = cursor.posX + moveValueX;
    }
    //bundle values then return them
    returnValues.returnCursor = cursor;
    returnValues.returnMap = renderedMap;
    return returnValues;
}

//create a menu where you will be able to choose quit, save, and load options
std::string viewMenu(){
    //function variable declarations
    char *choices[] = {"Save", "Load", "Quit", "Close"};
    char *menuTitle = "Game Menu";
    int ch, height, width, windY, windX, titleX, menuItemX, menuItemY, curItemY, curItemNum;
    bool closeMenu = false;
    std::string curItem;
    WINDOW *gameMenuWindow;

    //set window values
    height = 10;
    width = 20;
    windY = ((LINES - 5) - height) / 2;
    windX = (COLS - width) / 2;

    //create menu window and subwindow
    gameMenuWindow = newwin(height, width, windY, windX);
    keypad(gameMenuWindow, TRUE);
    wrefresh(gameMenuWindow);

    //print window border, menu title, and menu options.
    titleX = (width - strlen(menuTitle)) / 2;
    box(gameMenuWindow, 0, 0);
    wattron(gameMenuWindow, A_BOLD);
    mvwprintw(gameMenuWindow, 1, titleX, menuTitle);
    wattroff(gameMenuWindow, A_BOLD);
    menuItemY = 3;
    menuItemX = (width - strlen(choices[0])) / 2;
    mvwprintw(gameMenuWindow, menuItemY, menuItemX, "%s<", choices[0]);
    curItemY = menuItemY;
    curItemNum = 0;
    menuItemY = 4;
    for(int i = 1; i <= 3; i++){
        menuItemX = (width - strlen(choices[i])) / 2;
        mvwprintw(gameMenuWindow, menuItemY, menuItemX, choices[i]);
        menuItemY = menuItemY + 1;
    }
    wrefresh(gameMenuWindow);

    //main menu loop.
    while(closeMenu != true){
        switch(ch = getch()){
            case KEY_DOWN:
                if(curItemNum + 1 <= 3){
                    wmove(gameMenuWindow, curItemY, 0);
                    wclrtoeol(gameMenuWindow);
                    mvwprintw(gameMenuWindow, curItemY, ((width - strlen(choices[curItemNum])) / 2), choices[curItemNum]);
                    curItemNum = curItemNum + 1;
                    curItemY = curItemY + 1;
                    mvwprintw(gameMenuWindow, curItemY, ((width - strlen(choices[curItemNum])) / 2), "%s<", choices[curItemNum]);
                    box(gameMenuWindow, 0, 0);
                }
                break;
            case KEY_UP:
                if(curItemNum - 1 >= 0){
                    wmove(gameMenuWindow, curItemY, 0);
                    wclrtoeol(gameMenuWindow);
                    mvwprintw(gameMenuWindow, curItemY, ((width - strlen(choices[curItemNum])) / 2), choices[curItemNum]);
                    curItemNum = curItemNum - 1;
                    curItemY = curItemY - 1;
                    mvwprintw(gameMenuWindow, curItemY, ((width - strlen(choices[curItemNum])) / 2), "%s<", choices[curItemNum]);
                    box(gameMenuWindow, 0, 0);
                }
                break;
            //enter key
            case 10:
                curItem = choices[curItemNum];
                closeMenu = true;
                break;
            case KEY_F(1):
                closeMenu = true;
                break;
        }
        wrefresh(gameMenuWindow);
    }
    //cleanup before quit and returns the selected option to the main function
    delwin(gameMenuWindow);
    clear();
    return curItem;
}

//setup initial surface map
std::vector<std::string> initialSurfaceMapSetup(cursorStruct cursor){

    //variable declarations
    std::vector<std::string> initialMap;
    std::string dirt, grass;

    dirt.append("#::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::#");
    grass.append("#==============================================================================#");

    //fill the vector with empty spaces.
    for(int i = 0; i <= 24; i++){
        initialMap.push_back("                                                                                 ");
    }

    //print the # border in red
    for(int x = 0; x <= 79; x++){
        initialMap.at(0).at(x) = '#';
        initialMap.at(24).at(x) = '#';
    }
    for(int y = 0; y <= 24; y++){
        initialMap.at(y).at(0) = '#';
        initialMap.at(y).at(79) = '#';
    }

    //print ground
    int y = 23;
    for(int i = 1; i < 4; i++){
        initialMap.at(y) = dirt;
        y = y - 1;
    }
    initialMap.at(y) = grass;

    return initialMap;
}

//setup initial underground map
std::vector<std::string> initialUndergroundMapSetup(cursorStruct cursor){

    //map piece declaration.
    std::vector<std::string> initialMap;
    std::string hexMapPiece1, hexMapPiece2, hexMapPiece3, hexMapPiece4;
    int pieceNumber = 1;

    hexMapPiece1.append("#::***:::::***:::::***:::::***:::::***:::::***:::::***:::::***:::::***:::::***:#");
    hexMapPiece2.append("#:*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*#");
    hexMapPiece3.append("#*:::::***:::::***:::::***:::::***:::::***:::::***:::::***:::::***:::::***:::::#");
    hexMapPiece4.append("#:*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*:::*#");

    //fill the vector with empty spaces.
    for(int i = 0; i <= 24; i++){
        initialMap.push_back("                                                                                 ");
    }

    //print the # border in red
    for(int x = 0; x <= 79; x++){
        initialMap.at(0).at(x) = '#';
        initialMap.at(24).at(x) = '#';
    }
    for(int y = 0; y <= 24; y++){
        initialMap.at(y).at(0) = '#';
        initialMap.at(y).at(79) = '#';
    }

    //print the pieces of the map until full
    for(int y = 1; y < 24; y++){
        switch(pieceNumber){
            case 1:
                initialMap.at(y) = hexMapPiece1;
                pieceNumber = 2;
                break;
            case 2:
                initialMap.at(y) = hexMapPiece2;
                pieceNumber = 3;
                break;
            case 3:
                initialMap.at(y) = hexMapPiece3;
                pieceNumber = 4;
                break;
            case 4:
                initialMap.at(y) = hexMapPiece4;
                pieceNumber = 1;
                break;
        }
    }

    //terminates each of the lines
    for(int y = 0; y <= 24; y++){
        initialMap[y][80] = '\0';
    }

    return initialMap;
}

//display the information window
void displayInfoWindow(){
    //variable declaration
    int ch, height, width, windY, windX;
    int y = 4;
    int x = NULL;
    std::vector<std::string> information;
    WINDOW *infoWindow;

    //set the information to the line strings
    information.resize(1);
    information.at(0).append("You are an ant. Your goal is to make your");
    information.push_back("colony become as successful as possible.");
    information.push_back("You will face many hard ships but here is");
    information.push_back("some information to help you along the way.");
    information.push_back("'=' represents grass and ':' represents dirt.");
    information.push_back("The Queen is represented with 'Q' and individual");
    information.push_back("ant's are represented by either 'e', 'l', 'p',");
    information.push_back(" or 'a' depeneding on it's stage (egg, larve,");
    information.push_back("pupae, or adult.");
    information.push_back("Use the arrow keys to move the cursor and use");
    information.push_back("'s' to select an ant when it's highlighted.");
    information.push_back("'c' is used to create a new ant");

    //set window values
    height = information.size() + 8;
    width = 50;
    windY = ((LINES - 5) - height) / 2;
    windX = (COLS - width) / 2;

    //create the info window and draw the borders.
    infoWindow = newwin(height, width, windY, windX);
    keypad(infoWindow, TRUE);
    box(infoWindow, 0, 0);
    wrefresh(infoWindow);

    //print the screen information
    for(int i = 0; i < information.size(); i++){
        x = (width - information.at(i).length()) / 2;
        mvwprintw(infoWindow, y, x, "%s", information.at(i).c_str());
        y = y + 1;
    }
    wrefresh(infoWindow);

    //wait for f4 to be pressed to close the window
    while((ch = getch()) != KEY_F(4)){

    }

    //delete window, clear terminal
    delwin(infoWindow);
    clear();
}

//if needed, kill the ant
void killAnt(){
    ///TODO make kill the ant if needed
}

//loads all data from a text file and puts it into the game
void loadGame(){
///TODO have function load all data from a text file if told to

}

//refreshes the map to show any changes that have been made
void refreshMap(std::vector<std::string>  renderedMap, cursorStruct cursor){
    for(int y = 0; y <= 24; y++){
        for(int x = 0; x <= 79; x++){
            if((y == cursor.posY) && (x == cursor.posX)){
                attron(COLOR_PAIR(2));
                mvaddch(y, x, renderedMap[y][x]);
                attroff(COLOR_PAIR(2));
            }else{
                if(renderedMap[y][x] == '#'){
                    attron(COLOR_PAIR(1));
                    mvaddch(y, x, renderedMap[y][x]);
                    attroff(COLOR_PAIR(1));
                }else if(renderedMap[y][x] == '.'){
                    attron(COLOR_PAIR(3));
                    mvaddch(y, x, renderedMap[y][x]);
                    attroff(COLOR_PAIR(3));
                }else if(renderedMap[y][x] == '*'){
                    attron(COLOR_PAIR(4));
                    mvaddch(y, x, renderedMap[y][x]);
                    attroff(COLOR_PAIR(4));
                }else if(renderedMap[y][x] == 'e' || renderedMap[y][x] == 'l' || renderedMap[y][x] == 'p' || renderedMap[y][x] == 'a' || renderedMap[y][x] == 'Q'){
                    attron(COLOR_PAIR(5));
                    mvaddch(y, x, renderedMap[y][x]);
                    attroff(COLOR_PAIR(5));
                }else if(renderedMap[y][x] == ':'){
                    attron(COLOR_PAIR(6));
                    mvaddch(y, x, renderedMap[y][x]);
                    attroff(COLOR_PAIR(6));
                }else if(renderedMap[y][x] == '='){
                    attron(COLOR_PAIR(7));
                    mvaddch(y, x, renderedMap[y][x]);
                    attroff(COLOR_PAIR(7));
                }else{
                    mvaddch(y, x, renderedMap[y][x]);
                }
            }
        }
    }
    refresh();
}

//saves all the data from the game to a text file
void saveGame(std::vector<std::string>  renderedMap, std::vector<std::string> surfaceMap, std::vector<std::string> undergroundMap){
    std::ofstream mapFile("map.txt");
    mapFile << "Rendered Map" << '\n';
    for(int y = 0; y <= 24; y++){
        mapFile << renderedMap.at(y);
        mapFile << '\n';
    }
    mapFile << "Surface Map" << '\n';
    for(int y = 0; y <= 24; y++){
        mapFile << surfaceMap.at(y);
        mapFile << '\n';
    }
    mapFile << "Underground Map" << '\n';
    for(int y = 0; y <= 24; y++){
        mapFile << undergroundMap.at(y);
        mapFile << '\n';
    }
    mapFile.close();
}

//prints out all the information for the status area bellow the map
void statusAreaPrint(antStruct ant, int selectedAnt){
    //variable declaration
    std::string firstLineString, thirdLineString;
    int commandX = 1;
    int totalCommands = ant.availableCommands.at(selectedAnt).size() - 1;
    char commandKey = ' ';

    firstLineString.append("F1: Menu | F2: View Ants | F3: Toggle Map | F4: More Info");
    thirdLineString.append("Available Commands");

    //clear the status area
    move(25, 0);
    clrtobot();

    //print basic controls
    mvprintw(25, 1, "%s", firstLineString.c_str());

    //checks to see if ant is selected then prints the appropriate information
    if(selectedAnt != NULL){
        move(26, 0);
        clrtoeol();
        mvprintw(26, 1, "Number: %d | Type: %s | Age: %d", ant.number.at(selectedAnt), ant.type.at(selectedAnt).c_str(), ant.age.at(selectedAnt));
    }else{
        move(26, 0);
        clrtoeol();
        mvprintw(26, 1, "Number:   | Type:   | Age:   ");
    }

    //print the available commands
    if(selectedAnt != NULL){
        mvprintw(27, 1, "%s", thirdLineString.c_str());
        for(int i = 0; i <= totalCommands; i++){
            commandKey = ant.availableCommands.at(selectedAnt).at(i).at(0);
            if(i == totalCommands){
                mvprintw(28, commandX, "%c: %s", commandKey, ant.availableCommands.at(selectedAnt).at(i).c_str());
            }else{
                mvprintw(28, commandX, "%c: %s |", commandKey, ant.availableCommands.at(selectedAnt).at(i).c_str());
                commandX = commandX + ant.availableCommands.at(selectedAnt).at(i).length() + 6;
            }
        }
    }
}

//view a list of all ants when tab is pressed, alive and dead, and are able to filter the list.
void viewAnts(antStruct ant, int tick){
    //declaring function variables
    WINDOW *viewAntsWind;
    int height, width, windY, windX, ch, totalAnts, lastAntShown, firstAntShown;

    //ant window variable default values
    height = 20;
    width = 60;
    windY = ((LINES - 5) - height) / 2;
    windX = (COLS - width) / 2;
    totalAnts = ant.number.back();

    //creates the view ants window
    viewAntsWind = newwin(height, width, windY, windX);
    box(viewAntsWind, 0, 0);
    scrollok(viewAntsWind, TRUE);
    wrefresh(viewAntsWind);

    //checks to see if the first ant has been created, if it has been they will be printed out.
    int y = 1;
    for(int i = 1; i <= totalAnts; i++){
        if(y > 18){
            break;
        }
        mvwprintw(viewAntsWind, y, 1, "Ant Number: %d | Age: %d | Type: %s | %s", ant.number.at(i), ant.age.at(i), ant.type.at(i).c_str(), ant.location.at(i).c_str());
        y = y + 1;
        lastAntShown = i;
    }
    wrefresh(viewAntsWind);
    firstAntShown = 1;

    //scroll the window up and down when the appropriate key is pressed and print out the necessary lines.
    while((ch = getch()) != KEY_F(2)){
        switch(ch){
            case KEY_DOWN:
                if(lastAntShown + 1 <= ant.number.back()){
                    lastAntShown = lastAntShown + 1;
                    firstAntShown = firstAntShown + 1;
                    wborder(viewAntsWind, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
                    wrefresh(viewAntsWind);
                    wscrl(viewAntsWind, 1);
                    box(viewAntsWind, 0, 0);
                    mvwprintw(viewAntsWind, 18, 1, "Ant Number: %d | Age: %d | Type: %s | %s", ant.number.at(lastAntShown),
                              ant.age.at(lastAntShown), ant.type.at(lastAntShown).c_str(), ant.location.at(lastAntShown).c_str());
                    wrefresh(viewAntsWind);
                }
                break;
            case KEY_UP:
                if(firstAntShown - 1 >= 1){
                    lastAntShown = lastAntShown - 1;
                    firstAntShown = firstAntShown -1;
                    wborder(viewAntsWind, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
                    wrefresh(viewAntsWind);
                    wscrl(viewAntsWind, -1);
                    box(viewAntsWind, 0, 0);
                    mvwprintw(viewAntsWind, 1, 1, "Ant Number: %d | Age: %d | Type: %s | %s", ant.number.at(firstAntShown),
                               ant.age.at(firstAntShown), ant.type.at(firstAntShown).c_str(), ant.location.at(firstAntShown).c_str());
                    wrefresh(viewAntsWind);
                }
                break;
        }
    }
    delwin(viewAntsWind);
    clear();
}
