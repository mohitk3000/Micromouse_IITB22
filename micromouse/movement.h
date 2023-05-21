#ifndef movement
#define movement
#define LIBRARY_VERSION  0.1

#include "myPID.h"
#include "peripherals.h"
#include <stdint.h>

class Movement
{
  public:
    Movement(int, int, int, int, volatile uint64_t*, volatile uint64_t*, int*, int*);

    void motor_l(int);
    void motor_r(int);

    void stop();

    void forwardTune();
    void forwardTuneNew();
    void turnRightTune();
    void turnLeftTune();

    void slowfwdspidtune();
    void slowfwd();
    void slowlturn();
    void slowrturn();
    bool wallLeft();
    bool wallFront();
    bool wallRight();
    void forceStop();
    void stdFwd();
    bool forward();
    bool turnRight();
    bool turnLeft();
    void speedTest();
    void BTRecv();
    void SerialDebug();

    //double aKp = 1.31919,aKi=0,aKd=2.1;
    double angle_pid_setPoint = 150,lmop = 0,rmop = 0; //190.410
    double LMinSpeed = 150, RMinSpeed = 150;
    double irError,sOp,dOp,counts=359.23534,dSetPoint = 0,lrc = 0,irSetPoint = 0,scaled_op,scaledSpeedRedux = 0,sf = 10.4,dist = 18,dRange = 60; //50 is default distance
    int *lIR,*rIR,mSpeed = 10;//210;
    bool stopped = true;
    double isf = 1.9975;
    char tempMode = 'b';
    bool temp = false;
    int counts9cm = 179.58;
    const int L_IR_SETPOINT = 4.3,
              R_IR_SETPOINT = 4.3;
    double l,r;
    double lc,rc;
    volatile uint64_t lc1,rc1,lc2,rc2;
    int lms,rms;

    PID sPID;
    PID dPID;
    PID alPID;
    PID arPID;
    IR ir;

  private:
    
    #define IRLeftPinm 32
    #define IRRightPinm 34
    #define IRFrontPinm 35

    #define FREQ 30000
    #define RESOLUTION 8
    #define OP_LIMIT 2000

    #define DIA 3.35 
    #define WIDTH 10.75
    #define PI 3.14159265

    #define DEF_AKP 1.25 //hey these values are changed ref to previous version for changes!!!
    #define DEF_AKI 0
    #define DEF_AKD 0.125
    #define DEF_SKP 1.25 //or use 0.05 before value :0.25  //Straight PID - perfect
    #define DEF_SKI 0
    #define DEF_SKD 0.125 //before value : 0.00625
    #define DEF_DKP 1   //Dekhlo bhaii
    #define DEF_DKI 0
    #define DEF_DKD 0.955//0.495

    // #define DEF_AKP 1.31919 
    // #define DEF_AKI 0
    // #define DEF_AKD 0.125
    // #define DEF_SKP 0.25
    // #define DEF_SKI 0
    // #define DEF_SKD 0.00625
    // #define DEF_DKP 1.31919
    // #define DEF_DKI 0
    // #define DEF_DKD 0.25

    const int MLP1 = 12,
              MLP2 = 13,
              MRP1 = 26,
              MRP2 = 25;
    const int opLim = 2000;   

};
#endif
