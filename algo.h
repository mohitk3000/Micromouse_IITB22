
#include <string>
#include <queue>    
#include <vector>
#include <algorithm>

using std::pair;
using std::queue;
using std::make_pair;
using std::vector;

enum Direction{
    EAST,
    NORTH,
    WEST,
    SOUTH,
    UNINITILIZED
};
char directions[] = {'e', 'n', 'w', 's', 'u'};
class Node{
public:
    bool walls[4] = {false, false, false, false};
    int value = 0;
    bool initilized = false;
    bool visited = false;
    int x, y;
    void init(Direction facing, bool frontWall, bool leftWall, bool rightWall)
    {
        walls[facing] = frontWall;
        walls[(facing+1)%4] = leftWall;
        walls[(facing+2)%4] = false;
        walls[(facing+3)%4] = rightWall;
    }
    Node(){
    }
};

void turnByString(char* movements){
    int i = 0;
    while(movements[i] != 0){
        if(movements[i] == 'L'){
            SerialBT.println("Turning left");
            move.turnLeft();
        }
        if(movements[i] == 'R'){
            SerialBT.println("Turning Right");
            move.turnRight();
        }
        if(movements[i] == 'F'){
            SerialBT.println("Moving forward");
            move.stdFwd();
        }
        i++;
    }
}


int getTurnDirection(Direction current, Direction target){
    // function will return:
                    // 0 no turn
                    // +1 turn right
                    // -1 turn left
                    // 2 turn backwards
    return((((current-target)+5)%4)-1);
}
// char can be L, R, F depending on the type of movement required
// You can modify this function to accomodate diagonal movements


void mainfunc() {
    //log("Running...");
    SerialBT.println("Running...");
    int x = 0;
    int y = 0;
    int state = 0; // 0 is going to center
                   // 1 is returning to start
    int changed = 0; // 1 is the state of bot has changed between 'going to center' and 'going to start'
    Direction smallest;
    Node nodes[64][64];
    for(int i = 0; i < 256; i++){
        nodes[i/16][i%16].x = i/16;
        nodes[i/16][i%16].y = i%16;
        nodes[i/16][i%16].value = ((i/16 <= 7) ? (7 - i/16) : (i/16 - 8)) + ((i%16 <= 7) ? (7 - i%16) : (i%16 - 8));
        //API::setText(i/16, i%16, std::to_string(nodes[i/16][i%16].value).c_str());
    }

    Direction facing = NORTH;

    //API::setColor(0, 0, 'G');
    for(int i = 0; i < 16; i++){
        nodes[0][i].walls[WEST] = true;
        nodes[0][16 - i].walls[EAST] = true;
        nodes[i][0].walls[WEST] = true;
        nodes[16 - i][i].walls[WEST] = true;
    }

    while (true) {
        // updating wall information
       // API::setColor(x, y, 'G');
        nodes[x][y].visited = true;
        SerialBT.print(move.wallFront());SerialBT.print(" ");SerialBT.print(move.wallLeft());SerialBT.print(" ");SerialBT.println(move.wallRight());
        nodes[x][y].init(facing, move.wallFront(), move.wallLeft(), move.wallRight());
        nodes[x][y+1].walls[SOUTH] = nodes[x][y].walls[NORTH];
        nodes[x][y-1].walls[NORTH] = nodes[x][y].walls[SOUTH];
        nodes[x+1][y].walls[WEST] = nodes[x][y].walls[EAST];
        nodes[x-1][y].walls[EAST] = nodes[x][y].walls[WEST];
        // for(int i = 0; i < 4; i++){
        //     if (nodes[x][y].walls[i]) API::setWall(x, y, directions[i]);
        // }

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
                for(int i = 0; i < 256; i++){
                    q.push(make_pair(i/16, i%16));
                }
                changed = 0;
            }

            do{
                pair<int, int> crNodePos = q.front();
                q.pop();
                Node& cr = nodes[crNodePos.first][crNodePos.second];
                Node* neighbours[4];
                int neighboursFound = 0;
                int minFound = cr.value;

                if(state == 0){
                    if((crNodePos.first == 7 || crNodePos.first == 8) && (crNodePos.second == 7 || crNodePos.second == 8)){
                        continue;
                    }
                }
                else{
                    if(crNodePos.first == 0 && crNodePos.second == 0){
                        continue;
                    }
                }
                // ignore if out of bounds
                if(crNodePos.first > 15 || crNodePos.first < 0 || crNodePos.second > 15 || crNodePos.first < 0){
                    continue;
                }

                // Add all accessible neighbours to an array and keep track of its size
                if(cr.walls[NORTH] == false){
                    Node& next = nodes[crNodePos.first][crNodePos.second+1];
                    minFound = std::min(minFound, next.value);
                    neighbours[neighboursFound] = &next;
                    neighboursFound++;
                }
                if(cr.walls[SOUTH] == false){
                    Node& next = nodes[crNodePos.first][crNodePos.second-1];
                    minFound = std::min(minFound, next.value);
                    neighbours[neighboursFound] = &next;
                    neighboursFound++;
                }
                if(cr.walls[EAST] == false){
                    Node& next = nodes[crNodePos.first+1][crNodePos.second];
                    minFound = std::min(minFound, next.value);
                    neighbours[neighboursFound] = &next;
                    neighboursFound++;
                }
                if(cr.walls[WEST] == false){
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
            }while(!q.empty());
        }
        
        // we assume that 1000 movement are sufficient
        char movements[1000];
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
            }

            if(state == 0){
                if((x == 7 || x == 8) && (y == 7 || y == 8)){
                    state = 1;
                    changed = 1;
                    smallest = UNINITILIZED;
                    for(int i = 0; i < 256; i++){
                        nodes[i/16][i%16].value = i/16 + i%16;
                        //API::setText(i/16, i%16, std::to_string(nodes[i/16][i%16].value).c_str());
                    }
                }
            }
            else if(state == 1){
                if(x == 0 && y == 0){
                    state = 0; 
                    changed = 1;
                    smallest = UNINITILIZED;
                    for(int i = 0; i < 256; i++){
                        nodes[i/16][i%16].value = ((i/16 <= 7) ? (7 - i/16) : (i/16 - 8)) + ((i%16 <= 7) ? (7 - i%16) : (i%16 - 8));
                        //API::setText(i/16, i%16, std::to_string(nodes[i/16][i%16].value).c_str());
                    }
                }
            }
        }
        movements[numMovements] = 0;
        turnByString(movements);
    }
}