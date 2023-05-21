// #include <queue>
// #include <utility>
// #include <algorithm>

// using std::pair;
// using std::queue;
// using std::make_pair;

// enum Direction{
//     EAST,
//     NORTH,
//     WEST,
//     SOUTH,
//     UNINITILIZED
// };
// char directions[] = {'e', 'n', 'w', 's', 'u'};
// class Node{
// public:
//     bool walls[4] = {false, false, false, false};
//     int value = 0;
//     bool initilized = false;
//     bool visited = false;
//     int x, y;
//     void init(Direction facing, bool frontWall, bool leftWall, bool rightWall)
//     {
//         walls[facing] = frontWall;
//         walls[(facing+1)%4] = leftWall;
//         walls[(facing+2)%4] = false;
//         walls[(facing+3)%4] = rightWall;
//     }
//     Node(){
//     }
// };

// int getTurnDirection(Direction current, Direction target){
//                     // 0 no turn
//                     // +1 turn right
//                     // -1 turn left
//                     // 2 turn backwards
//     return((((current-target)+5)%4)-1);
// }

// void turnByString(char* movements){
//     int i = 0;
//     while(movements[i] != 0){
//         if(movements[i] == 'L'){
//             move.turnLeft();
//         }
//         if(movements[i] == 'R'){
//             move.turnRight();
//         }
//         if(movements[i] == 'F'){
//             move.forward();
//         }
//         i++;
//     }
// }

// void main(){
//     int x = 0;
//     int y = 0;
//     int state = 0;
//     int changed = 0;

//     Direction smallest;
//     Node nodes[64][64];

//     for(int i = 0; i < 256; i++){
//         nodes[i/16][i%16].x = i/16;
//         nodes[i/16][i%16].y = i%16;
//         nodes[i/16][i%16].value = ((i/16 <= 7) ? (7 - i/16) : (i/16 - 8)) + ((i%16 <= 7) ? (7 - i%16) : (i%16 - 8));
//     }

//     Direction facing = NORTH;

//     while(true){
//         nodes[x][y].visited = true;
//         nodes[x][y].init(facing, API::wallFront(), API::wallLeft(), API::wallRight());
//         nodes[x][y+1].walls[SOUTH] = nodes[x][y].walls[NORTH];
//         nodes[x][y-1].walls[NORTH] = nodes[x][y].walls[SOUTH];
//         nodes[x+1][y].walls[WEST] = nodes[x][y].walls[EAST];
//         nodes[x-1][y].walls[EAST] = nodes[x][y].walls[WEST];
//         for(int i = 0; i < 4; i++){
//             if (nodes[x][y].walls[i]) API::setWall(x, y, directions[i]);
//         }

//          if(!(smallest != UNINITILIZED)){
//             queue<pair<int, int>> q;
//             if(!changed){
//                 q.push(make_pair(x, y));
//                 q.push(make_pair(x + 1, y));
//                 q.push(make_pair(x - 1, y));
//                 q.push(make_pair(x, y + 1));
//                 q.push(make_pair(x, y - 1));
//             }
//             else{
//                 for(int i = 0; i < 256; i++){
//                     q.push(make_pair(i/16, i%16));
//                 }
//                 changed = 0;
//             }

//             do{
//                 pair<int, int> crNodePos = q.front();
//                 q.pop();
//                 Node& cr = nodes[crNodePos.first][crNodePos.second];
//                 Node* neighbours[4];
//                 int neighboursFound = 0;
//                 int minFound = cr.value;

//                 if(state == 0){
//                     if((crNodePos.first == 7 || crNodePos.first == 8) && (crNodePos.second == 7 || crNodePos.second == 8)){
//                         continue;
//                     }
//                 }
//                 else{
//                     if(crNodePos.first == 0 && crNodePos.second == 0){
//                         continue;
//                     }
//                 }

//                 if(crNodePos.first > 15 || crNodePos.first < 0 || crNodePos.second > 15 || crNodePos.first < 0){
//                     continue;
//                 }


//                 if(cr.walls[NORTH] == false){
//                     Node& next = nodes[crNodePos.first][crNodePos.second+1];
//                     minFound = std::min(minFound, next.value);
//                     neighbours[neighboursFound] = &next;
//                     neighboursFound++;
//                 }
//                 if(cr.walls[SOUTH] == false){
//                     Node& next = nodes[crNodePos.first][crNodePos.second-1];
//                     minFound = std::min(minFound, next.value);
//                     neighbours[neighboursFound] = &next;
//                     neighboursFound++;
//                 }
//                 if(cr.walls[EAST] == false){
//                     Node& next = nodes[crNodePos.first+1][crNodePos.second];
//                     minFound = std::min(minFound, next.value);
//                     neighbours[neighboursFound] = &next;
//                     neighboursFound++;
//                 }
//                 if(cr.walls[WEST] == false){
//                     Node& next = nodes[crNodePos.first-1][crNodePos.second];
//                     minFound = std::min(minFound, next.value);
//                     neighbours[neighboursFound] = &next;
//                     neighboursFound++;
//                 }

//                 if(minFound >= cr.value){
//                     cr.value = minFound + 1;
//                     for(int j = 0; j < neighboursFound; j++){
//                         q.push(make_pair(neighbours[j]->x, neighbours[j]->y));    
//                     }
//                 }
//             }while(!q.empty());
//         }

//         char movements[1000];
//         int numMovements = 0;

//         while(nodes[x][y].visited && changed == 0){
//             // move to smallest neighbouring node
//             smallest = UNINITILIZED;
//             // this logic needs to be changed, bot should give higher preference to stright routes compaired to ones requiring turns
//             //if no north wall then check north
//             if(nodes[x][y].walls[NORTH] == false){
//                 if(nodes[x][y].value > nodes[x][y+1].value){
//                     smallest = NORTH;
//                 }
//             }
//             if(nodes[x][y].walls[SOUTH] == false){
//                 if(nodes[x][y].value > nodes[x][y-1].value){
//                     smallest = SOUTH;
//                 }
//             }
//             if(nodes[x][y].walls[EAST] == false){
//                 if(nodes[x][y].value > nodes[x+1][y].value){
//                     smallest = EAST;
//                 }
//             }
//             if(nodes[x][y].walls[WEST] == false){
//                 if(nodes[x][y].value > nodes[x-1][y].value){
//                     smallest = WEST;
//                 }
//             }
//             if(smallest == UNINITILIZED) break;
//             // change this so bot will make a single turn and not multiple
//             int turn = getTurnDirection(facing, smallest);
//             switch(turn){
//                 case -1:
//                     movements[numMovements] = 'L';
//                     numMovements++;
//                     break;
//                 case 1:
//                     movements[numMovements] = 'R';
//                     numMovements++;
//                     break;
//                 case 2:
//                     movements[numMovements] = 'R';
//                     movements[numMovements + 1] = 'R';
//                     numMovements += 2;
//                     break;
//             }
//             facing = smallest;
//             movements[numMovements] = 'F';
//             numMovements++;

//             // update value of current position
//             switch(facing){
//                 case SOUTH:
//                     y--;
//                     break;
//                 case NORTH:
//                     y++;
//                     break;
//                 case EAST:
//                     x++;
//                     break;
//                 case WEST:
//                     x--;
//                     break;
//             }

//             if(state == 0){
//                 if((x == 7 || x == 8) && (y == 7 || y == 8)){
//                     state = 1;
//                     changed = 1;
//                     smallest = UNINITILIZED;
//                     for(int i = 0; i < 256; i++){
//                         nodes[i/16][i%16].value = i/16 + i%16;
//                     }
//                 }
//             }
//             else if(state == 1){
//                 if(x == 0 && y == 0){
//                     state = 0; 
//                     changed = 1;
//                     smallest = UNINITILIZED;
//                     for(int i = 0; i < 256; i++){
//                         nodes[i/16][i%16].value = ((i/16 <= 7) ? (7 - i/16) : (i/16 - 8)) + ((i%16 <= 7) ? (7 - i%16) : (i%16 - 8));
//                     }
//                 }
//             }
//         }

//         movements[numMovements] = 0;
//         turnByString(movements);
//     }
// }
