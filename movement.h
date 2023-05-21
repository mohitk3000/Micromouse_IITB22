#ifndef movement
#define movement
#define LIBRARY_VERSION  0.1

#include "myPID.h"
#include <stdint.h>

class Movement
{
  public:
    Movement(int, int, int, int, volatile uint64_t*, volatile uint64_t*, int*, int*);

    void motor_l(int);
    void motor_r(int);

    void stop();

    void sPIDTune();
    void forwardTune();
    void forwardTuneNew();
    void turnRightTune();
    void turnLeftTune();

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
    int opLim = 2000;
    int state = 0;
    double angle_pid_setPoint = 190.410448,lmop = 0,rmop = 0; //190.410
    double LMinSpeed = 150, RMinSpeed = 150;
    double irError,sOp,dOp,counts=359.23534,dSetPoint = 0,lrc = 0,irSetPoint = 0,scaled_op,scaledSpeedRedux,sf = 4,dist = 18,dRange = 60; //50 is default distance
    int *lIR,*rIR,mSpeed = 210;
    bool stopped = false;
    double isf = 1.9975;
    char tempMode = 'b';
    bool temp = false;
    int counts9cm = 179.58;
    int L_IR_THRESHOLD = 900;
    int L_IR_SETPOINT = 800;
    int R_IR_THRESHOLD = 900;
    int R_IR_SETPOINT = 1800;
    int F_IR_THRESHOLD = 940; //3000 tha
    int l,r;
    double lc,rc;
    int lms,rms;

    PID sPID;
    PID dPID;
    PID alPID;
    PID arPID;

  private:
    
    #define IRLeftPinm 15
    #define IRRightPinm 4
    #define IRFrontPinm 32

    #define FREQ 30000
    #define RESOLUTION 8
    #define SPEED 220
    #define OP_LIMIT 2000

    #define SF 134
    #define DIA 3.35 
    #define WIDTH 12.15 
    #define PI 3.14159265

    #define DEF_AKP 1.31919 //Angle PID - perfect
    #define DEF_AKI 0
    #define DEF_AKD 0.125
    #define DEF_SKP 0.25  //Straight PID - perfect
    #define DEF_SKI 0
    #define DEF_SKD 0.00625
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

    int MLP1,MLP2,MRP1,MRP2;
    volatile uint64_t *lc1,*rc1;
    

};
#endif
