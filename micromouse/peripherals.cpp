#include "peripherals.h"
#include "movement.h"
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif
	
double IR::linearize(int i){
      double y = 0;
      double x = i*1023.0/4095.0;
      if(x<=51){
        y = 0.09*(x-34);
      }
      else if(x<=71){
        y = 1.6 + 0.01*(x - 71);
      }
      else if(x<=373){
        y = 1.6+0.004*(x-71);
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

double IR::leftIR(){return linearize(analogRead(IRLeftPin));}
double IR::rightIR(){return linearize(analogRead(IRRightPin));}

bool IR::wallLeft(){return (linearize(analogRead(IRLeftPinm)) < L_IR_THRESHOLD);}
bool IR::wallRight(){return (linearize(analogRead(IRRightPinm)) < R_IR_THRESHOLD);}
bool IR::wallFront(){return (linearize(analogRead(IRFrontPinm)) < F_IR_THRESHOLD);}