/*
    pathfinder.cpp
        
    Method implementations for the pathfinder class.
    
    assignment: CSCI 262 Project - Pathfinder        

    author: <Aidan Lamkin>
*/

#include <iostream>
#include <fstream>
#include "pathfinder.h"

using namespace std;

// constructor - optional, in case you need to initialize anything
pathfinder::pathfinder() { ; }

// public run() method - invokes private methods to do everything;
// returns the total elevation change cost of the best optimal path.
int pathfinder::run(string data_file_name, string save_file_name, bool use_recursion) {
    // suggested implementation - modify as desired!
    _use_recursion = use_recursion;
    if (!_read_data(data_file_name)) {
        cout << "Error reading map data from \"" << data_file_name << "\"!" << endl;
        return -1;
    }

    _draw_map();

    int best_cost = _draw_paths();

    _save_image(save_file_name);

    return best_cost;
}

/*******************/
/* PRIVATE METHODS */
/*******************/

// _read_data - read and store width, height, and elevation data from the
// provided data file; return false if something goes wrong
bool pathfinder::_read_data(string data_file_name) {
    ifstream fin (data_file_name);

    string ncols; //establishing dumby variables to hold unwanted words from file
    string nrows;


    int i = 0;

    fin >> ncols >> _width;//reads in and stores width and height
    fin >> nrows >> _height;

    vector<vector<int>> elevation_map;
    vector<int> elevation_rows(_width);//declares a 2D vector

    while(!fin.eof()){//fills vector from file
        elevation_map.push_back(elevation_rows);
        for(int j = 0; j < _width; j++){
            fin >> elevation_map[i][j];
        }
        i++;
    }


    for(int i = 0; i < elevation_map.size(); i++){
        if(elevation_map[i][0] != 0) {
            _elevations.push_back(elevation_map[i]);
        }
    }

    fin.close();

    if(_elevations.size() * _elevations[0].size() == _width * _height){
        return true;
    }
    return false;
}

// _draw_map - draw the elevation data as grayscale values on our Picture
// object.
void pathfinder::_draw_map() {
    int min = 1000000;
    int max = 0;

    for(int i = 0; i < _height; i++){
        for(int j = 0; j < _width; j++){
            if(_elevations[i][j] < min){
                min = _elevations[i][j];
            }
            if(_elevations[i][j] > max){
                max = _elevations[i][j];
            }
        }
    }

    int pixel;
    _image.set(_height - 1, _width - 1, 0,0,0);
    // example of using Picture object: delete this code
    for (int i = 0; i < _height; i++) {
        for (int j = 0; j < _width; j++) {
            pixel = (_elevations[i][j] - min) * (255)/(max-min);
            int grey = pixel; // gray value equal to column
            _image.set(i, j, grey, grey, grey);
        }
    }
}

// _draw_paths() - find and draw optimal paths from west to east using either recursion or dynamic programming
// return the best optimal path cost
int pathfinder::_draw_paths() {

    vector<int> minimumCosts;
    for(int i = 0; i < _height; i++){
        minimumCosts.push_back(optCostToEast(i,0));
    }

    int minimum = 9999;
    for(int i = 0; i < minimumCosts.size(); i++){
        if(minimumCosts[i] < minimum){
            minimum = minimumCosts[i];
        }
    }

    return minimum;
}

// _save_image - we'll do this one for you :)
void pathfinder::_save_image(string save_file_name) {
    _image.save(save_file_name);
}

int pathfinder::optCostToEast(int i, int j) {
    int totalCost = 9999; //initialize totalCost to solve for minimum
    int costEast;
    int costNorthEast;
    int costSouthEast;

    if(j == _width - 2){//establishing base case where path is one block away from east edge
        costEast = fabs(_elevations[i][j] - _elevations[i][j+1]);
        if(i != _height - 1){
            costSouthEast = fabs(_elevations[i][j] -_elevations[i + 1][j + 1]);
        }
        if(i != 0){
            costNorthEast = fabs(_elevations[i][j] - _elevations[i - 1][j + 1]);
        }
        if(costEast < totalCost){//finds minimum cost for possible moves
            totalCost = costEast;
        }
        if(costNorthEast < totalCost && i != 0){
            totalCost = costNorthEast;
        }
        if(costSouthEast < totalCost && i != _height - 1){
            totalCost = costSouthEast;
        }

        return totalCost;//returns minimum cost
    }

    else{//breaks possible paths into what is possible based on current position
        if(i == 0){//if i is on the top of the map it cant go northeast
            costSouthEast = fabs(_elevations[i][j] - _elevations[i + 1][j + 1]) + optCostToEast(i + 1, j + 1);
            costEast = fabs(_elevations[i][j] - _elevations[i][j+1]) + optCostToEast(i, j + 1);
            if(costEast < totalCost){//find minimum change in elevation and returns it
                totalCost = costEast;
            }
            if(costSouthEast < totalCost){
                totalCost = costSouthEast;
            }
            return totalCost;
        }
        else if(i == _height - 1){//if i is on the bottom of the map it cant go southeast
            costNorthEast = fabs(_elevations[i][j] - _elevations[i - 1][j + 1]) + optCostToEast(i - 1, j + 1);
            costEast = fabs(_elevations[i][j] - _elevations[i][j+1]) + optCostToEast(i, j + 1);
            if(costEast < totalCost){//find minimum change in elevation and returns it
                totalCost = costEast;
            }
            if(costNorthEast < totalCost){
                totalCost = costNorthEast;
            }
            return totalCost;
        }
        else{//current position is not on top or bottom edge so it can go northeast, southeast and east
            costEast = fabs(_elevations[i][j] - _elevations[i][j+1]) + optCostToEast(i, j + 1);
            costNorthEast = fabs(_elevations[i][j] - _elevations[i - 1][j + 1]) + optCostToEast(i - 1, j + 1);
            costSouthEast = fabs(_elevations[i][j] - _elevations[i + 1][j + 1]) + optCostToEast(i + 1, j + 1);
            if(costEast < totalCost){//find minimum change in elevation and returns it
                totalCost = costEast;
            }
            if(costNorthEast < totalCost){
                totalCost = costNorthEast;
            }
            if(costSouthEast < totalCost){
                totalCost = costSouthEast;
            }

            return totalCost;
        }
    }
}