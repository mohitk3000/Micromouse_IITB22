#include "movement.h"
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//6 2 1.5 //300rpm-6 1.25 1.5 //1000rpm-200-2,3,0.5 //1000rpm-200-1.75,4.40,0.650 //1000rpm-distPID 2,0,2.5
/*
*    [0 2 4 5 12 15] : 0,2,12-critical
*    GPIO 0 (must be LOW to enter boot mode)
*    GPIO 2 (must be floating or LOW during boot)
*    GPIO 4
*    GPIO 5 (must be HIGH during boot)
*    GPIO 12 (must be LOW during boot)
*    GPIO 15 (must be HIGH during boot)
*/

#define IRLeftPin 32
#define IRRightPin 34
#define IRFrontPin 35 //15 4 22 23 32 35
#define MRP1 12
#define MRP2 13
#define MLP1 26
#define MLP2 25
#define lEncPin1 5
#define lEncPin2 18
#define rEncPin1 19
#define rEncPin2 21


BluetoothSerial SerialBT;
String recvMsg = "";
char recvChar;

volatile uint64_t lc1 = 0, rc1 = 0, lc2 = 0, rc2 = 0;
int irl = 0, irr = 0;

Movement move(MLP1,MLP2,MRP1,MRP2,&lc1,&rc1,&irl,&irr);

void IRAM_ATTR lisr1() {lc1++;}
void IRAM_ATTR risr1() {rc1++;}
void IRAM_ATTR lisr2() {lc2++;}
void IRAM_ATTR risr2() {rc2++;}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <queue>
#include <utility>
#include <algorithm>


using std::pair;
using std::queue;
using std::make_pair;

using std::pair;
using std::queue;
using std::make_pair;

enum Direction{
    EAST,
    NORTH,
    WEST,
    SOUTH,
    UNINITILIZED
};
bool mState = false;
char directions[] = {'e', 'n', 'w', 's', 'u'};
// we assume that 500 movement are sufficient
char movements[500],m[1000];
int numMovements = 0;
int MAZE_SIZE = 16;
    int x = 0;
    int y = 0;
    int state = 0; // 0 is going to center
                   // 1 is returning to start
    int changed = 0; // 1 is the state of bot has changed between 'going to center' and 'going to start'
    Direction smallest;
    Direction facing = NORTH;
class Node{
public:
    bool walls[4] = {false, false, false, false};
    int value2 = 0;
    int value = 0;
    bool initilized = false;
    bool visited = false;
    int x, y;
    void init(Direction facing, bool frontWall, bool leftWall, bool rightWall)
    {
        walls[facing] = frontWall;
        walls[(facing+1)%4] = leftWall;
        if(x != 0 || y != 0)
            walls[(facing+2)%4] = false;
        walls[(facing+3)%4] = rightWall;
    }
    Node(){
    }
};
Node nodes[16][16];


int getTurnDirection(int current, int target){
    // function will return:
                    // 0 no turn
                    // +1 turn right
                    // -1 turn left
                    // 2 turn backwards
    return((((current-target)+5)%4)-1);
}

void updateMovementsString(bool frontWall, bool leftWall, bool rightWall){
    // while (true) {
        // updating wall information
        nodes[x][y].visited = true;
        nodes[x][y].init(facing, frontWall, leftWall, rightWall);
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
                if(x != MAZE_SIZE - 1)
                    q.push(make_pair(x + 1, y));
                if(x != 0)
                    q.push(make_pair(x - 1, y));
                if(y != MAZE_SIZE - 1)
                    q.push(make_pair(x, y + 1));
                if(y != 0)
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
                    // for(int j = 0; j < neighboursFound; j++){
                    //     q.push(make_pair(neighbours[j]->x, neighbours[j]->y));    
                    // }
                    if(crNodePos.first != MAZE_SIZE - 1)
                        q.push(make_pair(crNodePos.first + 1, crNodePos.second));
                    if(crNodePos.first != 0)
                        q.push(make_pair(crNodePos.first - 1, crNodePos.second));
                    if(crNodePos.second != MAZE_SIZE - 1)
                        q.push(make_pair(crNodePos.first, crNodePos.second + 1));
                    if(crNodePos.second != 0)
                        q.push(make_pair(crNodePos.first, crNodePos.second - 1));
                }
            }
        }
        
        numMovements = 0;
        // we will break out of this loop if bot is stuck and need flood fill to be run again(smallest == UNINITILIZED) or it has reached destination or it entered cell it hasn't visited before
        while(nodes[x][y].visited && changed == 0){
            // move to smallest neighbouring node
            smallest = UNINITILIZED;
            // this logic needs to be changed, bot should give higher preference to stright routes compaired to ones requiring turns
            //if no north wall then check north
            int bestTurn = 5;
            if(nodes[x][y].walls[NORTH] == false){
                if(nodes[x][y].value > nodes[x][y+1].value){
                    int crTurn = getTurnDirection(facing, NORTH);
                    if(crTurn < bestTurn){
                        smallest = NORTH;
                        bestTurn = crTurn;
                    }
                }
            }
            if(nodes[x][y].walls[SOUTH] == false){
                if(nodes[x][y].value > nodes[x][y-1].value){
                    int crTurn = getTurnDirection(facing, SOUTH);
                    if(crTurn < bestTurn){
                        bestTurn = crTurn;
                        smallest = SOUTH;
                    }
                }
            }
            if(nodes[x][y].walls[EAST] == false){
                if(nodes[x][y].value > nodes[x+1][y].value){
                    int crTurn = getTurnDirection(facing, EAST);
                    if(crTurn < bestTurn){
                        bestTurn = crTurn;  
                        smallest = EAST;
                    }
                }
            }
            if(nodes[x][y].walls[WEST] == false){
                if(nodes[x][y].value > nodes[x-1][y].value){
                    int crTurn = getTurnDirection(facing, WEST);
                    if(crTurn < bestTurn){
                        bestTurn = crTurn;
                        smallest = WEST;
                    }
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
                    }
                }
            }
        }
        movements[numMovements] = 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

  pinMode(IRLeftPin, INPUT);
  pinMode(IRRightPin, INPUT);

  pinMode(lEncPin1, INPUT);
  pinMode(lEncPin2, INPUT);
  pinMode(rEncPin1, INPUT);
  pinMode(rEncPin2, INPUT);
  attachInterrupt(digitalPinToInterrupt(lEncPin1), lisr1, RISING);
  attachInterrupt(digitalPinToInterrupt(lEncPin2), lisr2, RISING);
  attachInterrupt(digitalPinToInterrupt(rEncPin1), risr1, RISING);
  attachInterrupt(digitalPinToInterrupt(rEncPin2), risr2, RISING);


  Serial.begin(115200);
  SerialBT.begin("Micromouse_IITBhilai");

  // delay(2000);
  // move.forward();
  // move.turnLeft();
  // move.forward();
  // move.turnRight();
  // move.forward();
}
int temp;
void loop()
{    
  BTRecv();
  SerialDebug('d');
  if(move.stopped = true){
    for(int i = 0;i<10;i++)
        SerialBT.println("MAZE ACTIV");
  for(int i = 0; i < MAZE_SIZE*MAZE_SIZE; i++){
        nodes[i/MAZE_SIZE][i%MAZE_SIZE].x = i/MAZE_SIZE;
        nodes[i/MAZE_SIZE][i%MAZE_SIZE].y = i%MAZE_SIZE;
        nodes[i/MAZE_SIZE][i%MAZE_SIZE].value = ((i/MAZE_SIZE <= (MAZE_SIZE-2)/2) ? ((MAZE_SIZE-2)/2 - i/MAZE_SIZE) : (i/MAZE_SIZE - MAZE_SIZE/2)) + ((i%MAZE_SIZE <= (MAZE_SIZE-2)/2) ? ((MAZE_SIZE-2)/2 - i%MAZE_SIZE) : (i%MAZE_SIZE - MAZE_SIZE/2));
        nodes[i/MAZE_SIZE][i%MAZE_SIZE].value2 = i/MAZE_SIZE + i%MAZE_SIZE;
    }


    for(int i = 0; i < MAZE_SIZE; i++){
        nodes[0][i].walls[WEST] = true;

        nodes[MAZE_SIZE - 1][i].walls[EAST] = true;

        nodes[i][0].walls[SOUTH] = true;

        nodes[i][MAZE_SIZE - 1].walls[NORTH] = true;
    }
    while (true) {
        bool f = move.wallFront(), l = move.wallLeft(), r = move.wallRight();
        SerialBT.print(l);SerialBT.print(" ");SerialBT.print(f);SerialBT.print(" ");SerialBT.println(r);
        updateMovementsString(f, l, r);
        int i = 0;
        while(movements[i] != 0){
            if(movements[i] == 'L'){
                SerialBT.println("turning left");
                move.slowlturn();
            }
            if(movements[i] == 'R'){
                SerialBT.println("turning right");
                move.slowrturn();
            }
            if(movements[i] == 'F'){
                SerialBT.println("moving fwd");
                move.constSpeed();
                move.constSpeed();
                move.forceStop();
            }
            i++;
        }
    }
  }
 

  // SerialBT.print(move.linearize(analogRead(IRLeftPin)));SerialBT.print(" ");SerialBT.print("");SerialBT.print(" ");SerialBT.println(move.linearize(analogRead(IRRightPin)));
  // if(move.stopped == false)dummytraversal();
  // SerialBT.print((digitalRead(22)));SerialBT.print("  ");SerialBT.print((digitalRead(23)));
}

/* Tuning Functions***************************************************************************
 * - For Tuning all the PID values and testing.
 *********************************************************************************************/
void tunedPID(){
  BTRecv();
  SerialDebug('s');
  if(move.stopped == false){
    move.accwpid();
    for(int i = 0;i < move.dRange;i++){
        move.constSpeed();
    }
    move.decwpid(temp);
    move.stopped == true;
  }
}

void tuneSPID(){
  BTRecv();
  move.slowfwdspidtune();
  SerialDebug('s');
}

void tuneTurningLeft(){
  BTRecv();
  move.turnLeftTune();
  SerialDebug('a');
}

void dummytraversal(){
    if(!move.wallFront()){
        move.constSpeed();
        move.constSpeed();
        if(!move.wallLeft())
            move.slowlturn();
        else if(!move.wallRight())
            move.slowrturn();
    }
    else{
        if(!move.wallLeft())
            move.slowlturn();
        else if(!move.wallRight())
            move.slowrturn();
        else{
            move.slowrturn();
            move.slowrturn();
        }
    }
}
/* Debugging Functions***************************************************************************
 * - BTRecv(): for receiving commands and values from phone.
 * - serialDebug(): for printing all information on phone as well as laptop(serial monitor).   
 *********************************************************************************************/

void BTRecv()
{
  if (SerialBT.available())
  {
    recvChar = SerialBT.read();
    if (recvChar == '\n')
    { 
      String s = "";
      switch(recvMsg[0])
      {
        case 'p':
        case 'P':
            for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            move.sPID.SetTunings((double)atof(s.c_str()), move.sPID.GetKi(), move.sPID.GetKd());  //sKp
          break;
        case 'i':
        case 'I':
            for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            move.sPID.SetTunings(move.sPID.GetKp(), (double)atof(s.c_str()), move.sPID.GetKd());  //sKi
          break;
        case 'd':
        case 'D':
            for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            move.sPID.SetTunings(move.sPID.GetKp(), move.sPID.GetKi(), (double)atof(s.c_str()));  //sKd
          break;
        case 'f':
        case 'F':
            for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            move.dPID.SetTunings((double)atof(s.c_str()), move.dPID.GetKi(), move.dPID.GetKd());  //dKp
          break;
        case 'g':
        case 'G':
            for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            move.dPID.SetTunings(move.dPID.GetKp(), (double)atof(s.c_str()), move.dPID.GetKd());  //dKi
          break;
        case 'h':
        case 'H':
            for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            move.dPID.SetTunings(move.dPID.GetKp(), move.dPID.GetKi(), (double)atof(s.c_str()));  //dKd
          break;
        case 'a':
        case 'A':
            for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            move.alPID.SetTunings(move.alPID.GetKp(),(double)atof(s.c_str()),move.alPID.GetKd()); //aKi
            move.arPID.SetTunings(move.arPID.GetKp(),(double)atof(s.c_str()),move.arPID.GetKd());
          break;
        case 'b':
        case 'B':
            for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            move.alPID.SetTunings((double)atof(s.c_str()),move.alPID.GetKi(),move.alPID.GetKd()); //aKp
            move.arPID.SetTunings((double)atof(s.c_str()),move.arPID.GetKi(),move.arPID.GetKd());
          break;
        case 'c':
        case 'C':
            for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            move.alPID.SetTunings(move.alPID.GetKp(),move.alPID.GetKi(),(double)atof(s.c_str())); //aKd
            move.arPID.SetTunings(move.arPID.GetKp(),move.arPID.GetKi(),(double)atof(s.c_str()));
          break;
        case 'o':
        case 'O':
            if (move.stopped == false){
              move.stopped = true;
              //move.forceStop();
            }
            else if (move.stopped == true){
              move.stopped = false;
              // move.sPID.SetTunings(move.sPID.GetKp(),move.sPID.GetKi(),move.sPID.GetKd());
              // move.dPID.SetTunings(move.dPID.GetKp(),move.dPID.GetKi(),move.dPID.GetKd());
              // move.alPID.SetTunings(move.alPID.GetKp(),move.alPID.GetKi(),move.alPID.GetKd());
              // move.arPID.SetTunings(move.arPID.GetKp(),move.arPID.GetKi(),move.arPID.GetKd());
              // move.stdFwd();
              // move.turnLeft();
              // move.stdFwd();
            }
            // rc1=0;rc2=0;
            // lc1=0;lc2=0;
            // move.LMinSpeed = 150;
            // move.RMinSpeed = 150;
          break;
        case 'l':
        case 'L':
            for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            // opLim = ((double)atof(s.c_str()));
            // mPID.SetOutputLimits(-opLim,opLim);
            move.sf = ((double)atof(s.c_str()));
        break;
        case 's':
        case 'S':
            for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            move.mSpeed = ((double)atof(s.c_str()));
          break;        
        case 'q':
        case 'Q':
            for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            move.dRange = ((double)atof(s.c_str()));
          break;
        case 'x':
        case 'X':
            for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            lc1 = 0;lc2 = 0;
            rc1 = 0;rc2 = 0;       
            move.angle_pid_setPoint = (7*abs((double)atof(s.c_str()))*WIDTH)/(12*DIA);
            move.dist = (double)atof(s.c_str());
            move.counts = (move.dist*210)/(PI*DIA);
          break;
        case 'r':
            ESP.restart();
          break;
        case 'R':
            ESP.restart();
          break;
        case 'w':
        case 'W':
            for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            move.isf = (double)atof(s.c_str());
            break;
        case 'j':
        case 'J':
            for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            move.dRange = (double)atof(s.c_str());
            break;
        case 't':
        case 'T':
          for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            move.L_IR_THRESHOLD = (double)atof(s.c_str());
            move.R_IR_THRESHOLD = (double)atof(s.c_str());
          break;
        case 'm':
        case 'M':
          for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            move.L_IR_SETPOINT = (double)atof(s.c_str());
            move.R_IR_SETPOINT = (double)atof(s.c_str());
          break;
        case 'z':
        case 'Z':
          for (int i = 1; i < recvMsg.length(); i++){
              s += recvMsg[i];
            }
            temp = (double)atof(s.c_str());
          break;
      }
     recvMsg = "";
    }
    else
      recvMsg += recvChar;
  }
}

/************************************************************************************************/
/********************************************SerialDebug*****************************************/
/************************************************************************************************/
void SerialDebug(char c)
{
  //To be printed in Serial Monitor  (Serial)
  Serial.print("{");
    Serial.print("Kp["); Serial.print(move.sPID.GetKp()); Serial.print("] ");
    Serial.print("Ki["); Serial.print(move.sPID.GetKi()); Serial.print("] ");
    Serial.print("Kd["); Serial.print(move.sPID.GetKd()); Serial.print("]");
  Serial.print("}   ");
  Serial.print("{");
    Serial.print(lc1); Serial.print("  ");Serial.print(rc1);
  Serial.print("}   ");
  Serial.print("{");
    Serial.print("irL("); Serial.print(irl); Serial.print(") ");
    Serial.print("irR("); Serial.print(irr); Serial.print(") ");
  Serial.print("}   ");
  Serial.println(move.mSpeed);


  //To be printed in Smart phone (Bluetooth)
  switch(c){
  case 'x': //For distance PID
    SerialBT.print("["); //Dist PID Debugging
      SerialBT.print(move.sPID.GetKp(),5);SerialBT.print(" ");
      SerialBT.print(move.sPID.GetKi(),5);SerialBT.print(" ");
      SerialBT.print(move.sPID.GetKd(),5);
    SerialBT.print("]  <");
    // SerialBT.print(move.dist);SerialBT.print(" ");
    SerialBT.print(move.l);SerialBT.print(" ");
    SerialBT.print(move.r);SerialBT.print(") [");
    SerialBT.print(move.l-move.r);SerialBT.print("> (");
    SerialBT.print(move.irError/10.4);SerialBT.print(") ");
    SerialBT.print(move.scaled_op);SerialBT.println(" ");
    // SerialBT.print(move.sOp);SerialBT.print(" ");
    // SerialBT.print(move.scaled_op);SerialBT.print("] ");
    // SerialBT.print(move.lms);SerialBT.print(" ");
    // SerialBT.print(move.rms);SerialBT.println(" ");
    break;
  case 't':
    SerialBT.print("[");
      SerialBT.print(move.l);SerialBT.print(" ");
      SerialBT.print(move.r);
    SerialBT.print("]  ");
    SerialBT.print(move.irError);SerialBT.print(" ");
    SerialBT.print(move.tempMode);SerialBT.print(" ");
    SerialBT.println(move.isf);
    break;
  case 's': //For straightening PID
    SerialBT.print("[");   //Straight PID Debugging
      SerialBT.print(move.sPID.GetKp(),5);SerialBT.print(" ");
      SerialBT.print(move.sPID.GetKi(),5);SerialBT.print(" ");
      SerialBT.print(move.sPID.GetKd(),5);
    SerialBT.print("]  ");
    SerialBT.print(move.l);SerialBT.print(" ");
    SerialBT.print(move.r);SerialBT.print(" ");
    SerialBT.print(move.l - move.r);SerialBT.print(" | ");
    SerialBT.print(move.irError);SerialBT.print(" | ");
    SerialBT.print(move.sOp);SerialBT.print(" ");
    SerialBT.print(move.scaled_op);SerialBT.print(" ");
    SerialBT.print(move.tempMode);SerialBT.print(" ");    
    SerialBT.print(move.mSpeed);SerialBT.print(" {");
    SerialBT.print(move.lms);SerialBT.print(" ");
    SerialBT.print(" ");SerialBT.print(move.rms);SerialBT.println("}");
    break;
  case 'd': //For distance PID
    SerialBT.print("["); //Dist PID Debugging
      SerialBT.print(move.dPID.GetKp(),5);SerialBT.print(" ");
      SerialBT.print(move.dPID.GetKi(),5);SerialBT.print(" ");
      SerialBT.print(move.dPID.GetKd(),5);
    SerialBT.print("]  ");
    SerialBT.print(((lc1+rc1)/2.0)*(3.141*3.35/210.0));SerialBT.print(" ");
   // SerialBT.print(move.dRange);SerialBT.print(" ");
    SerialBT.print(move.lrc);SerialBT.print(" ");
    SerialBT.print(move.dOp);SerialBT.print(" ");
    SerialBT.print(move.fs);SerialBT.print(" ");
    SerialBT.print(move.lms);SerialBT.print(" ");
    SerialBT.print(" ");SerialBT.println(move.rms);
    break;
  case 'a': //Fpr angular PID
    SerialBT.print("["); //Angle PID Debugging
      SerialBT.print(move.alPID.GetKp(),5);SerialBT.print(" ");
      SerialBT.print(move.alPID.GetKi(),5);SerialBT.print(" ");
      SerialBT.print(move.alPID.GetKd(),5);
    SerialBT.print("]  ");
    SerialBT.print(move.dist);SerialBT.print(" ");
    SerialBT.print(move.dRange);SerialBT.print(" ");
    SerialBT.print(move.lrc);SerialBT.print(" ");
    SerialBT.print(move.dOp);SerialBT.print(" ");
    SerialBT.print(move.scaledSpeedRedux);SerialBT.print(" ");
    SerialBT.print((move.mSpeed - move.scaled_op - move.scaledSpeedRedux));SerialBT.print(" ");
    SerialBT.print(" ");SerialBT.println((move.mSpeed+ 2 + move.scaled_op - move.scaledSpeedRedux));
    break;
  }
}
