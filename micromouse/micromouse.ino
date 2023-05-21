#include "movement.h"
#include "BluetoothSerial.h"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

//6 2 1.5 //300rpm-6 1.25 1.5 //1000rpm-200-2,3,0.5 //1000rpm-200-1.75,4.40,0.650 //1000rpm-distPID 2,0,2.5
/* NOTE:-
*    [0 2 4 5 12 15] : 0,2,12-critical
*    GPIO 0 (must be LOW to enter boot mode)
*    GPIO 2 (must be floating or LOW during boot)
*    GPIO 4
*    GPIO 5 (must be HIGH during boot)
*    GPIO 12 (must be LOW during boot)
*    GPIO 15 (must be HIGH during boot)
*/

#define lEncPin1 5
#define lEncPin2 18
#define rEncPin1 19
#define rEncPin2 21


BluetoothSerial SerialBT;
String recvMsg = "";
char recvChar;

volatile uint64_t lc1 = 0, rc1 = 0, lc2 = 0, rc2 = 0;
int irl = 0, irr = 0;

Movement move;

void IRAM_ATTR lisr1() {move.lc1++;lc1++;}
void IRAM_ATTR risr1() {move.rc1++;rc1++;}
void IRAM_ATTR lisr2() {move.lc2++;lc2++;}
void IRAM_ATTR risr2() {move.rc2++;rc2++;}

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

void loop()
{  
  BTRecv();
  SerialDebug('s');
  while(move.stopped!=true){
    SerialBT.print(analogRead(IRLeftPin));SerialBT.print(" ");
    //SerialBT.print(move.wallLeft());
    SerialBT.print(" , ");
    SerialBT.print(analogRead(IRFrontPin));SerialBT.print(" ");
    //SerialBT.print(move.wallFront());
    SerialBT.print(" , ");
    SerialBT.print(analogRead(IRRightPin));SerialBT.print(" ");
    //SerialBT.println(move.wallRight());
    //move.slowfwdspidtune();
    mainfunc();
    }
   //  if(!move.wallFront())
   //      move.slowfwd();
   
   // else{
   //  if(move.wallLeft() && move.wallRight()){
   //      move.slowrturn();
   //      move.slowrturn();
   //  }
   //  if(!move.wallLeft())
   //      move.slowlturn();
   //  else if(!move.wallRight())
   //      move.slowrturn();
   
   //}
  //move.slowfwdspidtune();
  
}

/* Tuning Functions***************************************************************************
 * - For Tuning all the PID values and testing.
 *********************************************************************************************/
void tuneMovingForward(){
  BTRecv();
  //move.stdFwd();
  SerialDebug('x');
}

void tuneSPID(){
  BTRecv();
  move.sPIDTune();
  SerialDebug('t');
}

void tuneTurningLeft(){
  BTRecv();
  move.turnLeftTune();
  SerialDebug('a');
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
              move.forceStop();
            }
            else if (move.stopped == true){
              move.stopped = false;
              move.sPID.SetTunings(move.sPID.GetKp(),move.sPID.GetKi(),move.sPID.GetKd());
              move.dPID.SetTunings(move.dPID.GetKp(),move.dPID.GetKi(),move.dPID.GetKd());
              move.alPID.SetTunings(move.alPID.GetKp(),move.alPID.GetKi(),move.alPID.GetKd());
              move.arPID.SetTunings(move.arPID.GetKp(),move.arPID.GetKi(),move.arPID.GetKd());
              // move.stdFwd();
              // move.turnLeft();
              // move.stdFwd();
            }
            rc1=0;rc2=0;
            lc1=0;lc2=0;
            move.LMinSpeed = 150;
            move.RMinSpeed = 150;
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
      SerialBT.print(move.dPID.GetKp(),5);SerialBT.print(" ");
      SerialBT.print(move.dPID.GetKi(),5);SerialBT.print(" ");
      SerialBT.print(move.dPID.GetKd(),5);
    SerialBT.print("]  <");
    // SerialBT.print(move.dist);SerialBT.print(" ");
    SerialBT.print((lc1/210.0)*(10.52));SerialBT.print("> (");
    SerialBT.print(move.lms);SerialBT.print(" ");
    SerialBT.print(move.rms);SerialBT.print(") [");
    SerialBT.print(move.lrc);SerialBT.print(" ");
    SerialBT.print(move.dOp);SerialBT.print("] ");
    SerialBT.print(move.tempMode);SerialBT.print(" ");
    SerialBT.print((analogRead(IRFrontPinm) < move.F_IR_THRESHOLD));SerialBT.print(" ");
    SerialBT.print(analogRead(32));SerialBT.print(" ");
    SerialBT.print((move.lc+move.rc)/2);SerialBT.print(" ");
    SerialBT.print(move.counts);SerialBT.println(" ");
    //SerialBT.print((move.lc+move.rc)/2);SerialBT.println(" ");
    // SerialBT.print(move.scaledSpeedRedux);SerialBT.print(" ");
    // SerialBT.print(" ");SerialBT.println((2 + move.scaledSpeedRedux));
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
    SerialBT.print(move.l - move.r);SerialBT.print(" ");
    SerialBT.print(move.irError);SerialBT.print(" ");
    SerialBT.print(move.sOp);SerialBT.print(" ");
    SerialBT.print(move.scaled_op);SerialBT.print(" ");
    SerialBT.print(move.mSpeed + move.scaled_op);SerialBT.print(" ");
    SerialBT.print(" ");SerialBT.println(move.mSpeed + 2 - move.scaled_op);
    break;
  case 'd': //For distance PID
    SerialBT.print("["); //Dist PID Debugging
      SerialBT.print(move.dPID.GetKp(),5);SerialBT.print(" ");
      SerialBT.print(move.dPID.GetKi(),5);SerialBT.print(" ");
      SerialBT.print(move.dPID.GetKd(),5);
    SerialBT.print("]  ");
    SerialBT.print(move.dist);SerialBT.print(" ");
    SerialBT.print(move.dRange);SerialBT.print(" ");
    SerialBT.print(move.lrc);SerialBT.print(" ");
    SerialBT.print(move.dOp);SerialBT.print(" ");
    SerialBT.print(move.scaledSpeedRedux);SerialBT.print(" ");
    SerialBT.print((move.mSpeed - move.scaled_op - move.scaledSpeedRedux));SerialBT.print(" ");
    SerialBT.print(" ");SerialBT.println((move.mSpeed+ 2 + move.scaled_op - move.scaledSpeedRedux));
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
