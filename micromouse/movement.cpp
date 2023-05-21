#include "movement.h"
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

Movement::Movement()
         :sPID (&irError,&sOp,&irSetPoint,DEF_SKP,DEF_SKI,DEF_SKD,REVERSE),
          dPID (&lrc,&dOp,&dSetPoint,DEF_DKP,DEF_DKI,DEF_DKD,DIRECT),
          alPID(&lc,&lmop,&angle_pid_setPoint,DEF_AKP,DEF_AKI,DEF_AKD,DIRECT),
          arPID(&rc,&rmop,&angle_pid_setPoint,DEF_AKP,DEF_AKI,DEF_AKD,DIRECT) {
  
  ledcSetup(0, FREQ, RESOLUTION);
  ledcSetup(1, FREQ, RESOLUTION);
  ledcSetup(2, FREQ, RESOLUTION);
  ledcSetup(3, FREQ, RESOLUTION);

  ledcAttachPin(MLP1, 0);
  ledcAttachPin(MLP2, 1);
  ledcAttachPin(MRP1, 2);
  ledcAttachPin(MRP2, 3);
  
  sPID.SetMode(AUTOMATIC);
  sPID.SetOutputLimits(-opLim, +opLim);
  sPID.SetSampleTime(25);
  
  dPID.SetMode(AUTOMATIC);
  dPID.SetOutputLimits(-opLim, +opLim);
  dPID.SetSampleTime(25);

  alPID.SetMode(AUTOMATIC);
  alPID.SetOutputLimits(-opLim, +opLim);
  alPID.SetSampleTime(25);
  arPID.SetMode(AUTOMATIC);
  arPID.SetOutputLimits(-opLim, +opLim);
  arPID.SetSampleTime(25);
}

void Movement::motor_l(int a) {
  lms = a;
  if (a >= 0) {
    ledcWrite(0, 0);
    if(a > 255) //Capping
      ledcWrite(1, 255);
    else
      ledcWrite(1, a);
    return;
  }

  if(a < -255)  //Capping
    ledcWrite(0, 255);
  else
    ledcWrite(0, -a);
  ledcWrite(1, 0);
}

void Movement::motor_r(int a) {
  rms = a;
  if (a >= 0) {
    ledcWrite(2, 0);
    if(a > 255) //Capping
      ledcWrite(3, 255);
    else
      ledcWrite(3, a);

    return;
  }
  if(a<-255) //Capping
      ledcWrite(2, 255);
    else
      ledcWrite(2, -a);
  ledcWrite(3, 0);
}

bool Movement::turnRight() {
  while(true){
    if (lc1 > angle_pid_setPoint) {
      lmop = LMinSpeed;
      motor_l(LMinSpeed);
      if (LMinSpeed > 100)LMinSpeed -= 0.1;
    }
    else {
      alPID.Compute();
      lmop = 150 + lmop / (255.0 / 105.0);
      if (lc1 < angle_pid_setPoint / 2)
        lmop = 150 + 255 - lmop;
      motor_l(-lmop);
    }


    if (rc1 > angle_pid_setPoint) {
      rmop = RMinSpeed;
      motor_r(-RMinSpeed);
      if (RMinSpeed > 100)RMinSpeed -= 0.1;
    }
    else {
      arPID.Compute();
      rmop = 150 + rmop / (255.0 / 105.0);
      if (rc1 < angle_pid_setPoint / 2)
        rmop = 150 + 255 - rmop;
      motor_r(rmop);
    }

    if(RMinSpeed <= 140 && LMinSpeed <= 140){
       lc1 = 0;
       rc1 = 0;
       LMinSpeed = 150;
       RMinSpeed = 150;
       stop();
       break;
     }
   }
}

void Movement::slowlturn(){
  while(true){

    if(lc1 > (angle_pid_setPoint) && rc1 > (angle_pid_setPoint)){
      //forceStop();
      while(LMinSpeed >= 60 && RMinSpeed >= 60){
        motor_l(-LMinSpeed);
        motor_r(RMinSpeed+2);
        LMinSpeed-=0.01;
        RMinSpeed-=0.01;
      }
      LMinSpeed = 160;
      RMinSpeed = 160;
      //
      stop();
      lc1 = 0;
      rc1 = 0;
      //stopped = true;
      return;
    }
    else{
      motor_l(+160);
      motor_r(-160);
    }
  //   if(rc > (angle_pid_setPoint)){
  //     forceStop();
  //     stop();
  //     return;
  //   }
  //   else{
  //     motor_r(-165);
  //   }
  }
}
void Movement::slowrturn(){
  while(true){

    if(lc1 > (angle_pid_setPoint) && rc1 > (angle_pid_setPoint)){
      //forceStop();
      while(LMinSpeed >= 60 && RMinSpeed >= 60){
        motor_l(LMinSpeed);
        motor_r(-RMinSpeed-2);
        LMinSpeed-=0.01;
        RMinSpeed-=0.01;
      }
      LMinSpeed = 160;
      RMinSpeed = 160;
      //
      stop();
      lc1 = 0;
      rc1 = 0;
      //stopped = true;
      return;
    }
    else{
      motor_l(-160);
      motor_r(+160);
    }
  }
  // if(lc > 2*(angle_pid_setPoint)){
  //     forceStop();
  //     stop();
  //     return;
  //   }
  //   else{
  //     motor_l(-165);
  //   }
  // }
}

bool Movement::turnLeft() {
  while(true){

    if (lc1 > angle_pid_setPoint) {
      lmop = LMinSpeed;
      motor_l(-LMinSpeed);
      if (LMinSpeed > 100)LMinSpeed -= 0.1;
    }
    else {
      alPID.Compute();
      lmop = 150 + lmop / (255.0 / 105.0);
      if (lc < angle_pid_setPoint / 2)
        lmop = 150 + 255 - lmop;
      motor_l(lmop);
    }

    if (rc1 > angle_pid_setPoint) {
      rmop = RMinSpeed;
      motor_r(RMinSpeed);
      if (RMinSpeed > 100)RMinSpeed -= 0.1;
    }
    else {
      arPID.Compute();
      rmop = 150 + rmop / (255.0 / 105.0);
      if (rc1 < angle_pid_setPoint / 2)
        rmop = 150 + 255 - rmop;
      motor_r(-rmop);
    }

    if(LMinSpeed <= 100 && RMinSpeed <= 100){
       lc1 = 0;
       rc1 = 0;
       LMinSpeed = 150;
       RMinSpeed = 150;
       stop();
       break;
     }
  }
}

bool Movement::forward() {  //Deprecated
 while(true){

    int l = ir.leftIR();
    int r = ir.rightIR();

    if(l > L_IR_THRESHOLD && r > R_IR_THRESHOLD){
      irError = 0;
      tempMode = 'n';
      sPID.SetMode(MANUAL);
      sOp = 0;
      scaled_op = 0;
    }
    else if(l > L_IR_THRESHOLD){
      irError = (R_IR_SETPOINT - r)/isf;
      tempMode = 'r';
      sPID.SetMode(AUTOMATIC);
    }
    else if(r > R_IR_THRESHOLD){
      irError = (l - L_IR_SETPOINT)/isf;
      tempMode = 'l';
      sPID.SetMode(AUTOMATIC);
    }
    else{
      irError = (l - r)/3.995; //-3995 +3995 -> -1000 +1000 //Straight PID
      tempMode = 'b';
      sPID.SetMode(AUTOMATIC);
    }

    sPID.Compute();
    scaled_op = sOp/sf;//sf

  
    lrc = -(dRange - ((lc1+rc1)*dRange)/(2*counts));
    dPID.Compute();
    scaledSpeedRedux = dRange-dOp;//(dist-dOp*(dist)/opLim);

    if (stopped == true) {
     stop();
    }
    else if (stopped == false) {
       if((rc+lc) >= 2*counts){
       stop();
       lc1 = 0;
       rc1 = 0;
       break;
     }
     else{
       motor_l(-(mSpeed - scaled_op - scaledSpeedRedux));
       motor_r(-(mSpeed+ 2 + scaled_op - scaledSpeedRedux)); //DON'T REMOVE THAT 2!!!!
     }
    }
  }
}

void Movement::slowfwd(){
   while(true){

    l = ir.leftIR();
    r = ir.rightIR();

    if(l > L_IR_THRESHOLD && r > R_IR_THRESHOLD){
      irError = 0;
      tempMode = 'n';
      if(sPID.GetMode() == AUTOMATIC)
        sPID.SetMode(MANUAL);
      sOp = 0;
      scaled_op = 0;
    }
    else if(l > L_IR_THRESHOLD && r < R_IR_THRESHOLD){
      irError = (R_IR_SETPOINT - r)*23;
      tempMode = 'r';
      if(sPID.GetMode() == MANUAL)
        sPID.SetMode(AUTOMATIC);
    }
    else if(r > R_IR_THRESHOLD && l < L_IR_THRESHOLD){
      irError = (l - L_IR_SETPOINT)*23;
      tempMode = 'l';
      if(sPID.GetMode() == MANUAL)
        sPID.SetMode(AUTOMATIC);
    }
    else{
      irError = (l - r)*sf; //-3995 +3995 -> -1000 +1000 //Straight PID
      tempMode = 'b';
      if(sPID.GetMode() == MANUAL)
        sPID.SetMode(AUTOMATIC);
    }

    sPID.Compute();
    scaled_op = sOp/sf;//sf

    if(ir.wallFront()){
      if(tempMode != 'F'){
      forceStop();
      tempMode = 'F';
    }
      lc1 = 0;
      rc1 = 0;
     //dPID.SetTunings(dPID.GetKp(),dPID.GetKi(),dPID.GetKd());
     //stopped = true;
      return;
    }

     if((rc+lc) >= 2*(counts)){
     forceStop();
      lc1 = 0;
      rc1 = 0;
     tempMode = 'F';
     //stopped = true;
     return;
   }
    else{
        motor_l(-(mSpeed - scaled_op));
        motor_r(-(2 + mSpeed + scaled_op));
      }
    }
  }


// void Movement::stdFwd(){

//   bool frontWall = (analogRead(IRFrontPinm) < F_IR_THRESHOLD);

//   lc = *lc1;rc = *rc1;
//   if(frontWall){
//     forceStop();
//     stop();
//     return;
//   }

//   if (stopped == true) {
//    stop();
//   }
//   else if (stopped == false) {
//      if((rc+lc) >= 2*(counts - 17.96)){
//      forceStop();
//      stop();
//      *lc1 = 0;
//      *rc1 = 0;
//      stopped = true;
//      dPID.SetTunings(dPID.GetKp(),dPID.GetKi(),dPID.GetKd());
//      tempMode = 'F';
//    }
//   else{
//       if((rc+lc) < 359.2){ //Acceleration for 9 cm
//         scaledSpeedRedux = 160+((lc+rc)/(counts))*(mSpeed-160);
//         motor_l(-(scaledSpeedRedux));
//         motor_r(-(2 + scaledSpeedRedux));
//         tempMode = 'a';
//       }
//       else if((rc+lc) <= 2*(counts-179.6)){
//         motor_l(-mSpeed);
//         motor_r(-mSpeed-2);
//         tempMode = 'c';
//       }
//       else{
//         lrc = -dRange*((lc+rc)/counts - 1);
//         dPID.Compute();
//         scaledSpeedRedux = mSpeed - dOp;
//         motor_l(-(scaledSpeedRedux));
//         motor_r(-(2 + scaledSpeedRedux));
//         tempMode = 'd';
//       }
//     }
//   }
// }

void Movement::stdFwd(){

  while(true){
    lc = *lc1;rc = *rc1;
    
    int a = analogRead(IRLeftPinm);
    int b = analogRead(IRRightPinm); 
    l = linearize(a);
    r = linearize(b);

    if(l > L_IR_THRESHOLD && r > R_IR_THRESHOLD){
      irError = 0;
      tempMode = 'n';
      sPID.SetMode(MANUAL);
      sOp = 0;
      scaled_op = 0;
    }
    else if(l > L_IR_THRESHOLD){
      irError = (R_IR_SETPOINT - r)/isf;
      tempMode = 'r';
      sPID.SetMode(AUTOMATIC);
    }
    else if(r > R_IR_THRESHOLD){
      irError = (l - L_IR_SETPOINT)/isf;
      tempMode = 'l';
      sPID.SetMode(AUTOMATIC);
    }
    else{
      irError = (l - r)/3.995; //-3995 +3995 -> -1000 +1000 //Straight PID
      tempMode = 'b';
      sPID.SetMode(AUTOMATIC);
    }

    sPID.Compute();
    scaled_op = sOp/sf;//sf

    if(wallFront()){
      forceStop();
     *lc1 = 0;
     *rc1 = 0;
     dPID.SetTunings(dPID.GetKp(),dPID.GetKi(),dPID.GetKd());
     
      return;
    }

     if((rc+lc) >= 2*(counts - 17.96)){
     forceStop();
     *lc1 = 0;
     *rc1 = 0;
     dPID.SetTunings(dPID.GetKp(),dPID.GetKi(),dPID.GetKd());
     tempMode = 'F';
     return;
   }
    else{
      if((rc+lc) < 359.2){ //Acceleration for 9 cm
        scaledSpeedRedux = 160+((lc+rc)/(counts))*(mSpeed-160);
        motor_l(-(scaledSpeedRedux - scaled_op));
        motor_r(-(2 + scaledSpeedRedux + scaled_op));
        tempMode = 'a';
      }
      else if((rc+lc) <= 2*(counts-179.6)){
        motor_l(-mSpeed - scaled_op);
        motor_r(-mSpeed-2 + scaled_op);
        tempMode = 'c';
      }
      else{
        lrc = -dRange*((lc+rc)/counts - 1);
        dPID.Compute();
        scaledSpeedRedux = mSpeed - dOp;
        motor_l(-(scaledSpeedRedux - scaled_op));
        motor_r(-(2 + scaledSpeedRedux + scaled_op));
        tempMode = 'd';
      }
    }
  }
}
void Movement::forceStop(){
  while(LMinSpeed >= 60 && RMinSpeed >= 60){
    motor_l(LMinSpeed);
    motor_r(RMinSpeed+2);
    LMinSpeed-=0.01;
    RMinSpeed-=0.01;
  }
  LMinSpeed = 150;
  RMinSpeed = 150;
  stop();
}

void Movement::stop(){
  motor_l(0);
  motor_r(0);
}




