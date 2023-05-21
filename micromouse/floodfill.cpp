#include "floodfill.h"

void Floodfill::mainfunc() {
    //log("Running...");
    int x = 0;
    int y = 0;
    int state = 0; // 0 is going to center
                   // 1 is returning to start
    int changed = 0; // 1 is the state of bot has changed between 'going to center' and 'going to start'
    Direction smallest;
    Node nodes[16][16];
    for(int i = 0; i < MAZE_SIZE*MAZE_SIZE; i++){
        nodes[i/MAZE_SIZE][i%MAZE_SIZE].x = i/MAZE_SIZE;
        nodes[i/MAZE_SIZE][i%MAZE_SIZE].y = i%MAZE_SIZE;
        nodes[i/MAZE_SIZE][i%MAZE_SIZE].value = ((i/MAZE_SIZE <= (MAZE_SIZE-2)/2) ? ((MAZE_SIZE-2)/2 - i/MAZE_SIZE) : (i/MAZE_SIZE - MAZE_SIZE/2)) + ((i%MAZE_SIZE <= (MAZE_SIZE-2)/2) ? ((MAZE_SIZE-2)/2 - i%MAZE_SIZE) : (i%MAZE_SIZE - MAZE_SIZE/2));
        nodes[i/MAZE_SIZE][i%MAZE_SIZE].value2 = i/MAZE_SIZE + i%MAZE_SIZE;
        //API::setText(i/MAZE_SIZE, i%MAZE_SIZE, std::to_string(nodes[i/MAZE_SIZE][i%MAZE_SIZE].value).c_str());
    }

    Direction facing = NORTH;

    while (true) {
        // updating wall information
        nodes[x][y].visited = true;
        nodes[x][y].init(facing, move.wallFront(), move.wallLeft(), move.wallRight());
        if(y != MAZE_SIZE - 1)
            nodes[x][y+1].walls[SOUTH] = nodes[x][y].walls[NORTH];
        if(y != 0)
            nodes[x][y-1].walls[NORTH] = nodes[x][y].walls[SOUTH];
        if(x != MAZE_SIZE - 1)
            nodes[x+1][y].walls[WEST] = nodes[x][y].walls[EAST];
        if(x != 0)
            nodes[x-1][y].walls[EAST] = nodes[x][y].walls[WEST];
        

        // samllest == UNINITILIZED (written due to some bug) then flood fill
        if(!(smallest != UNINITILIZED)){
            queue<pair<int, int>> q;
            // if changed is 0 add current cell and all its neighbours, o/w add all the cells to queue
            if(!changed){
                q.push(make_pair(x, y));
                q.push(make_pair(x + 1, y));
                q.push(make_pair(x - 1, y));
                q.push(make_pair(x, y + 1));
                q.push(make_pair(x, y - 1));
            }
            else{
                changed = 0;
            }

            while(!q.empty()){
                pair<int, int> crNodePos = q.front();
                q.pop();
                Node& cr = nodes[crNodePos.first][crNodePos.second];
                Node* neighbours[4];
                int neighboursFound = 0;
                int minFound = cr.value;

                if(state == 0){
                    if((crNodePos.first == (MAZE_SIZE - 2)/2 || crNodePos.first == MAZE_SIZE/2) && (crNodePos.second == (MAZE_SIZE - 2)/2 || crNodePos.second == MAZE_SIZE/2)){
                        continue;
                    }
                }
                else{
                    if(crNodePos.first == 0 && crNodePos.second == 0){
                        continue;
                    }
                }
                // ignore if out of bounds
                if(crNodePos.first > MAZE_SIZE - 1 || crNodePos.first < 0 || crNodePos.second > MAZE_SIZE - 1 || crNodePos.first < 0){
                    continue;
                }

                // Add all accessible neighbours to an array and keep track of its size
                if(cr.walls[NORTH] == false ){
                    Node& next = nodes[crNodePos.first][crNodePos.second+1];
                    minFound = std::min(minFound, next.value);
                    neighbours[neighboursFound] = &next;
                    neighboursFound++;
                }
                if(cr.walls[SOUTH] == false ){
                    Node& next = nodes[crNodePos.first][crNodePos.second-1];
                    minFound = std::min(minFound, next.value);
                    neighbours[neighboursFound] = &next;
                    neighboursFound++;
                }
                if(cr.walls[EAST] == false ){
                    Node& next = nodes[crNodePos.first+1][crNodePos.second];
                    minFound = std::min(minFound, next.value);
                    neighbours[neighboursFound] = &next;
                    neighboursFound++;
                }
                if(cr.walls[WEST] == false ){
                    Node& next = nodes[crNodePos.first-1][crNodePos.second];
                    minFound = std::min(minFound, next.value);
                    neighbours[neighboursFound] = &next;
                    neighboursFound++;
                }

                // compairing neighbours
                // if bot is stuck at a corner, then recalculate value of current cell and add all neighbours to the queue
                if(minFound >= cr.value){
                    cr.value = minFound + 1;
                    //API::setText(cr.x, cr.y, std::to_string(cr.value).c_str());
                    for(int j = 0; j < neighboursFound; j++){
                        q.push(make_pair(neighbours[j]->x, neighbours[j]->y));    
                    }
                }
            }
        }
        
        // we assume that 1000 movement are sufficient
        char movements[500];
        int numMovements = 0;
        // we will break out of this loop if bot is stuck and need flood fill to be run again(smallest == UNINITILIZED) or it has reached destination or it entered cell it hasn't visited before
        while(nodes[x][y].visited && changed == 0){
            // move to smallest neighbouring node
            smallest = UNINITILIZED;
            // this logic needs to be changed, bot should give higher preference to stright routes compaired to ones requiring turns
            //if no north wall then check north
            if(nodes[x][y].walls[NORTH] == false){
                if(nodes[x][y].value > nodes[x][y+1].value){
                    smallest = NORTH;
                }
            }
            if(nodes[x][y].walls[SOUTH] == false){
                if(nodes[x][y].value > nodes[x][y-1].value){
                    smallest = SOUTH;
                }
            }
            if(nodes[x][y].walls[EAST] == false){
                if(nodes[x][y].value > nodes[x+1][y].value){
                    smallest = EAST;
                }
            }
            if(nodes[x][y].walls[WEST] == false){
                if(nodes[x][y].value > nodes[x-1][y].value){
                    smallest = WEST;
                }
            }
            if(smallest == UNINITILIZED) break;
            // change this so bot will make a single turn and not multiple
            int turn = getTurnDirection(facing, smallest);
            switch(turn){
                case -1:
                    movements[numMovements] = 'L';
                    numMovements++;
                    break;
                case 1:
                    movements[numMovements] = 'R';
                    numMovements++;
                    break;
                case 2:
                    movements[numMovements] = 'R';
                    movements[numMovements + 1] = 'R';
                    numMovements += 2;
                    break;
            }
            facing = smallest;
            movements[numMovements] = 'F';
            numMovements++;

            // update value of current position
            switch(facing){
                case SOUTH:
                    y--;
                    break;
                case NORTH:
                    y++;
                    break;
                case EAST:
                    x++;
                    break;
                case WEST:
                    x--;
                    break;
                case UNINITILIZED:
                    break;
            }

            int tp;
            if(state == 0){
                if((x == (MAZE_SIZE - 2)/2 || x == MAZE_SIZE/2) && (y == (MAZE_SIZE - 2)/2 || y == MAZE_SIZE/2)){
                    state = 1;
                    changed = 1;
                    smallest = UNINITILIZED;
                    for(int i = 0; i < MAZE_SIZE*MAZE_SIZE; i++){
                        tp = nodes[i/MAZE_SIZE][i%MAZE_SIZE].value;
                        nodes[i/MAZE_SIZE][i%MAZE_SIZE].value = nodes[i/MAZE_SIZE][i%MAZE_SIZE].value2;
                        nodes[i/MAZE_SIZE][i%MAZE_SIZE].value2 = tp;
                        //API::setText(i/MAZE_SIZE, i%MAZE_SIZE, std::to_string(nodes[i/MAZE_SIZE][i%MAZE_SIZE].value).c_str());
                    }
                }
            }
            else if(state == 1){
                if(x == 0 && y == 0){
                    state = 0; 
                    changed = 1;
                    smallest = UNINITILIZED;
                    for(int i = 0; i < MAZE_SIZE*MAZE_SIZE; i++){
                        tp = nodes[i/MAZE_SIZE][i%MAZE_SIZE].value;
                        nodes[i/MAZE_SIZE][i%MAZE_SIZE].value = nodes[i/MAZE_SIZE][i%MAZE_SIZE].value2;
                        nodes[i/MAZE_SIZE][i%MAZE_SIZE].value2 = tp;
                        //API::setText(i/MAZE_SIZE, i%MAZE_SIZE, std::to_string(nodes[i/MAZE_SIZE][i%MAZE_SIZE].value).c_str());
                    }
                }
            }
        }
        movements[numMovements] = 0;
        turnByString(movements);
    }
}

void Floodfill::turnByString(char* movements){
    int i = 0;
    while(movements[i] != 0){
        if(movements[i] == 'L'){
            //SerialBT.println("Turning Left");
            m.slowlturn();
        }
        if(movements[i] == 'R'){
            //SerialBT.println("Turning Right");
            m.slowrturn();
        }
        if(movements[i] == 'F'){
            //SerialBT.println("Moving Forward");
            m.slowfwd();
        }
        i++;
    }
}