#ifndef movement
#define movement
#define LIBRARY_VERSION  0.1

#include "myPID.h"
#include <stdint.h>

class Movement
{
  public:
    Movement(int, int, int, int, volatile uint64_t*, volatile uint64_t*, int*, int*);

    void motor_l(double);
    void motor_r(double);

    void stop();

    void decwpid(double);
    void acceleration();
    void acc9cm();
    void accwpid();
    void const9cm();
    void constSpeed();
    void deceleration(double);
    void sPIDTune();
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
    
    void s();
    double t = 0;
    //double aKp = 1.31919,aKi=0,aKd=2.1;
    int opLim = 2000;
    int fs = 9;
    int state = 0;
    double q = 0.01;
    double angle_pid_setPoint = 162, lmop = 0,rmop = 0; //190.410
    double LMinSpeed = 150, RMinSpeed = 150;
    double irError,sOp,dOp,counts=359.23534,dSetPoint = 0,lrc = 0,irSetPoint = 0,scaled_op,scaledSpeedRedux,sf = 10.4,dist = 18,dRange = 145; //50 is default distance
    int *lIR,*rIR,mSpeed = 10;//210;
    bool stopped = true;
    double isf = 1.9975;
    char tempMode = 'b';
    bool temp = false;
    int counts9cm = 179.58;
    int L_IR_THRESHOLD = 16;//19.5;
    int R_IR_THRESHOLD = 12;//17.5;
    int F_IR_THRESHOLD = 20; //4 is also good

    int L_IR_SETPOINT = 4.3;
    int R_IR_SETPOINT = 6.5;
    double l,r;
    double lc,rc;
    int lms,rms;


    bool fw = false;
    PID sPID;
    PID dPID;
    PID alPID;
    PID arPID;
    double linearize(int i){
    double y = 0;
    double x = i*1023.0/4095.0;
    if(x<=51){
      y = 0.09*(x-34);
    }
    else if(x<=71){
      y = 1.6 + 0.01*(x - 51);
    }
    else if(x<=373){
      y = 1.8+0.004*(x-71);
    }
    else if(x<=520){
      y = 2.8+0.008*(x-373);
    }
    else if(x<=582){
      y = 4+0.013*(x-520);
    }
    else if(x<=636){
      y = 4.8+0.0225*(x-582);
    }
    else{
      y = 6+0.04*(x-636);
    }
    return y+2.7;//bcoz it has 1.5mm && 2.5cm negative offset more dist.
}

  private:
    
    #define IRLeftPinm 32
    #define IRRightPinm 34
    #define IRFrontPinm 35
    #define IRDiagLPin 22
    #define IRDiagRPin 23
    
    #define FREQ 30000
    #define RESOLUTION 8
    #define SPEED 220
    #define OP_LIMIT 2000

    #define SF 134
    #define DIA 3.35 
    #define WIDTH 11.05
    #define PI 3.14159265

    #define DEF_AKP 1.25 //hey these values are changed ref to previous version for changes!!!
    #define DEF_AKI 0
    #define DEF_AKD 0.125
    #define DEF_SKP 1.25 //or use 0.05 before value :0.25  //Straight PID - perfect
    #define DEF_SKI 0
    #define DEF_SKD 0.0625 //0.35 //before value : 0.00625
    #define DEF_DKP 0.15673   //Dekhlo bhaii
    #define DEF_DKI 0
    #define DEF_DKD 0//0.495

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
