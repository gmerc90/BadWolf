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

bool checkForEdge(std::vector<std::string>  renderedMap, cursorStruct cursor);

int selectAnt(antStruct ant, cursorStruct cursor);

std::string viewMenu();

std::vector<std::string> copyMap(std::vector<std::string>  mapToCopy);
std::vector<std::string> initialMapSetup(cursorStruct cursor);

moveAntReturn moveSelectedAnt(int selectedAnt, antStruct ant, cursorStruct cursor, std::vector<std::string> renderedMap);

std::vector<std::string> toggleHexStatus(std::vector<std::string>  newMap, cursorStruct cursor);

void refreshMap(std::vector<std::string>  renderedMap, cursorStruct cursor);
void killAnt();
void viewAnts(antStruct ant, int tick);
void saveGame(std::vector<std::string>  renderedMap);
void loadGame();

int main(int argc, char *argv[]){

    //set the set to the time for rand
    srand(time(NULL));

    //initial basic declarations
    int ch, tick, vectorSize, totalAnts;
    int selectedAnt = NULL;

    std::vector<std::string>  renderedMap, undergroundMap, surfaceMap;
    std::string chosenMenuOption, currentMap;

    moveAntReturn moveAntReturnValues;

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


    //set initial position and initialize the cursor
    cursorStruct cursor;
    cursor.posY = (LINES - 5)/2;
    cursor.posX = COLS/2;

    //show initial map
    renderedMap = initialMapSetup(cursor);
    refreshMap(renderedMap, cursor);
    //undergroundMap = copyMap(renderedMap);
    currentMap = "underground";

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
    ant.replaceCharacter.push_back(':');
    ant.type.resize(1);
    ant.type.push_back("Queen");
    ant.wings.resize(1);
    ant.wings.push_back(true);
    ant.posX.resize(1);
    ant.posX.push_back(cursor.posX);
    ant.posY.resize(1);
    if(renderedMap[cursor.posY - 1][cursor.posX] != '*'){
        ant.posY.push_back(cursor.posY - 1);
    }else{
        ant.posY.push_back(cursor.posY - 2);
    }
    renderedMap[ant.posY.at(1)][ant.posX.at(1)] = ant.character.at(1);
    refreshMap(renderedMap, cursor);

    //main game loop
    while(chosenMenuOption != "Quit"){

        /*checks to see if a key was pressed,
        if one has been, it performs the necessary code to
        complete the desired action*/
        ///TODO change it to 'x' miliseconds later and let it be configurable in a config file
        timeout(100);
        switch(ch = getch()){
            //move up
            case KEY_UP:
            case 'w':
                cursor.posY = cursor.posY - 1;
                if(checkForEdge(renderedMap, cursor) != false){
                    cursor.posY = cursor.posY + 1;
                }
                refreshMap(renderedMap, cursor);
                break;

            //move left
            case KEY_LEFT:
            case 'a':
                cursor.posX = cursor.posX - 1;
                if(checkForEdge(renderedMap, cursor) != false){
                    cursor.posX = cursor.posX + 1;
                }
                refreshMap(renderedMap, cursor);
                break;

            //move down
            case KEY_DOWN:
            case 's':
                cursor.posY = cursor.posY + 1;
                if(checkForEdge(renderedMap, cursor) != false){
                    cursor.posY = cursor.posY -1;
                }
                refreshMap(renderedMap, cursor);
                break;

            //move right
            case KEY_RIGHT:
            case 'd':
                cursor.posX = cursor.posX + 1;
                if(checkForEdge(renderedMap, cursor) != false){
                    cursor.posX = cursor.posX - 1;
                }
                refreshMap(renderedMap, cursor);
                break;

            //select an ant
            case 'e':
                selectedAnt = selectAnt(ant, cursor);
                break;
            //clear selected ant with Shift + e
            case 'E':
                selectedAnt = NULL;
                break;

            //move the selected ant to the location of the cursor
            case 'm':
                moveAntReturnValues = moveSelectedAnt(selectedAnt, ant, cursor, renderedMap);
                renderedMap = moveAntReturnValues.returnMap;
                ant = moveAntReturnValues.returnAnt;
                refreshMap(renderedMap, cursor);
                break;

            //select a cell
            case 't':
                renderedMap = toggleHexStatus(renderedMap, cursor);
                refreshMap(renderedMap, cursor);
                break;

            //create ants with their initial value
            case 'c':
                vectorSize = ant.number.back() + 1;
                ant.number.push_back(vectorSize);
                ant.age.push_back(0);
                ant.birthTick.push_back(tick);
                ant.character.push_back('e');
                ant.replaceCharacter.push_back(':');
                ant.type.push_back("Egg");
                ant.wings.push_back(false);
                ant.posX.push_back(cursor.posX);
                if(renderedMap[cursor.posY + 1][cursor.posX] != '*'){
                    ant.posY.push_back(cursor.posY + 1);
                }else{
                    ant.posY.push_back(cursor.posY + 2);
                }
                renderedMap[ant.posY.at(vectorSize)][ant.posX.at(vectorSize)] = ant.character.at(vectorSize);
                mvprintw(24, 3, "%d", ant.number.size());
                refreshMap(renderedMap, cursor);
                break;

            //bring up game menu
            case KEY_F(1):
                chosenMenuOption = viewMenu();
                refreshMap(renderedMap, cursor);
                //take the appropriate actions for the chosen menu option.
                if(chosenMenuOption == "Save"){
                    saveGame(renderedMap);
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
                renderedMap[ant.posY.at(i)][ant.posX.at(i)] = ant.character.at(i);
            }
        }
        refreshMap(renderedMap, cursor);

        //show selected ant information
        if(selectedAnt != NULL){
            move(26, 0);
            clrtoeol();
            mvprintw(26, 1, "Number: %d | Type: %s | Age: %d", ant.number.at(selectedAnt), ant.type.at(selectedAnt).c_str(), ant.age.at(selectedAnt));
        }else{
            move(26, 0);
            clrtoeol();
            mvprintw(26, 1, "Number:   | Type:   | Age:   ");
        }
    }
    //terminate program
    endwin();
    return 0;
}

//takes the selected ant a moves it to the desired location.
moveAntReturn moveSelectedAnt(int selectedAnt, antStruct ant, cursorStruct cursor, std::vector<std::string> renderedMap){
    //declare the initial values
    int travelY, travelX;
    moveAntReturn returnData;

    //check to see if an ant is selected
    if(selectedAnt != NULL){

        //find the difference in distance between the cursor and the ant
        travelY = cursor.posY - ant.posY.at(selectedAnt);
        travelX = cursor.posX - ant.posX.at(selectedAnt);

        //move the ant the desired distance
        if(renderedMap[cursor.posY][cursor.posX] != '*'){
            renderedMap[ant.posY.at(selectedAnt)][ant.posX.at(selectedAnt)] = ant.replaceCharacter.at(selectedAnt);
            ant.posY.at(selectedAnt) = ant.posY.at(selectedAnt) + travelY;
            ant.posX.at(selectedAnt) = ant.posX.at(selectedAnt) + travelX;
            renderedMap[ant.posY.at(selectedAnt)][ant.posX.at(selectedAnt)] = ant.character.at(selectedAnt);
        }
    }
    //set the structure values to the appropriate values and then return them
    returnData.returnMap = renderedMap;
    returnData.returnAnt = ant;
    return returnData;
}

//copy a map from one array to another
std::vector<std::string> copyMap(std::vector<std::string>  mapToCopy){
    std::vector<std::string> copiedMap;
    for(int y = 0; y <= 24; y++){
        for(int x = 0; x <= 80; x++)
            copiedMap[y][x] = mapToCopy[y][x];
    }
    return copiedMap;
}

//select an ant to give commands to
int selectAnt(antStruct ant, cursorStruct cursor){
    //declaring variables
    int selectedAnt, totalAnts;

    //check all the ants until it finds one that matches the cursor coordinates
    totalAnts = ant.number.back();
    for(int i = 0; i <= totalAnts; i++){
        if((ant.posY[i] == cursor.posY) && (ant.posX[i] == cursor.posX)){
            selectedAnt = i;
            break;
        }else{
            selectedAnt = NULL;
        }
    }
    return selectedAnt;
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

//if needed, kill the ant
void killAnt(){
    ///TODO make kill the ant if needed
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
        mvwprintw(viewAntsWind, y, 1, "Ant Number: %d | Age: %d | Type: %s", ant.number.at(i), ant.age.at(i), ant.type.at(i).c_str());
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
                    mvwprintw(viewAntsWind, 18, 1, "Ant Number: %d | Age: %d | Type: %s", ant.number.at(lastAntShown),
                              ant.age.at(lastAntShown), ant.type.at(lastAntShown).c_str());
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
                    mvwprintw(viewAntsWind, 1, 1, "Ant Number: %d | Age: %d | Type: %s", ant.number.at(firstAntShown),
                              ant.age.at(firstAntShown), ant.type.at(firstAntShown).c_str());
                    wrefresh(viewAntsWind);
                }
                break;
        }
    }
    delwin(viewAntsWind);
    clear();
}

//saves all the data from the game to a text file
void saveGame(std::vector<std::string>  renderedMap){
    std::ofstream renderedMapFile("map.txt");
    for(int y = 0; y <= 24; y++){
        renderedMapFile << renderedMap.at(y);
        renderedMapFile << '\n';
    }
    renderedMapFile.close();
}

//loads all data from a text file and puts it into the game
void loadGame(){
///TODO have function load all data from a text file if told to

}

//change the state of the hex cube to selected or not
std::vector<std::string> toggleHexStatus(std::vector<std::string>  newMap, cursorStruct cursor){

    char mapReplaceCharacter;
    int posYChange[] = { -1, 0, 1, 0};
    int posXChange[] = { 0, 1, 0, -1};
    int posNegOne[] = { 1, -1};

    //check to find out what to replace spaces with and if the cursor is on a * spot.
    if(newMap[cursor.posY][cursor.posX] != '*'){
        if(newMap[cursor.posY][cursor.posX] == ':'){
            mapReplaceCharacter = ' ';
        }else if(newMap[cursor.posY][cursor.posX] == ' '){
            mapReplaceCharacter = ':';
        }

        //check to see if there are any fields left to replace

        ///TODO think of better names for old_y/x(_b)
        //replace the checked spaces
        for(int i = 0; i <= 3; i++){
            int oldY = cursor.posY;
            int oldX = cursor.posX;
            int oldYB, oldXB;
            while((newMap[oldY + posYChange[i]][oldX + posXChange[i]] != '*') && (newMap[oldY + posYChange[i]][oldX + posXChange[i]] != '#')){
                newMap[oldY + posYChange[i]][oldX + posXChange[i]] = mapReplaceCharacter;
                oldY = oldY + posYChange[i];
                oldX = oldX + posXChange[i];
                oldYB = oldY;
                oldXB = oldX;
                for(int i = 0; i <= 1; i++){
                    while((newMap[oldYB + posNegOne[i]][oldXB] != '*') && (newMap[oldYB + posNegOne[i]][oldXB] != '#')){
                        newMap[oldYB + posNegOne[i]][oldXB] = mapReplaceCharacter;
                        oldYB = oldYB + posNegOne[i];
                    }
                    oldYB = oldY;
                    while((newMap[oldYB][oldXB + posNegOne[i]] != '*') && (newMap[oldYB][oldXB + posNegOne[i]] != '#')){
                        newMap[oldYB][oldXB + posNegOne[i]] = mapReplaceCharacter;
                        oldXB = oldXB + posNegOne[i];
                    }
                    oldYB = oldY;
                    oldXB = oldX;
                }
            }
        }
    }
    return newMap;
}

//checks to see if cursor is trying to move out of the map
bool checkForEdge(std::vector<std::string>  renderedMap, cursorStruct cursor){
    char testChar = renderedMap[cursor.posY][cursor.posX];
    if(testChar == '#'){
        return true;
    }else{
        return false;
    }
}

//prints out hex_map from map.h
std::vector<std::string> initialMapSetup(cursorStruct cursor){

    //map piece declaration.
    std::vector<std::string> initialMap;
    std::string hexMapPiece1, hexMapPiece2, hexMapPiece3, hexMapPiece4;
    int pieceNumber = 1;

    hexMapPiece1.append("#::******::::::::******::::::::******::::::::******::::::::******::::::::******#");
    hexMapPiece2.append("#:*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::#");
    hexMapPiece3.append("#*::::::::******::::::::******::::::::******::::::::******::::::::******:::::::#");
    hexMapPiece4.append("#:*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::*::::::#");

    //fill the vector with empty spaces.
    for(int i = 0; i <= 24; i++){
        initialMap.push_back("                                                                                 ");
    }

    //print the # border in red
    int x, y;
    for(x = 0; x <= 79; x++){
        initialMap.at(0).at(x) = '#';
        initialMap.at(24).at(x) = '#';
    }
    for(y = 0; y <= 24; y++){
        initialMap.at(y).at(0) = '#';
        initialMap.at(y).at(79) = '#';
    }

    //print the pieces of the map until full
    for(y = 1; y < 24; y++){
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
    for(y = 0; y <= 24; y++){
        initialMap[y][80] = '\0';
    }

    return initialMap;
}

//refreshes the map to show any changes that have been made
void refreshMap(std::vector<std::string>  renderedMap, cursorStruct cursor){
    int x, y;
    for(y = 0; y <= 24; y++){
        for(x = 0; x <= 79; x++){
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
                }else{
                    mvaddch(y, x, renderedMap[y][x]);
                }
            }
        }
    }
    refresh();
}
