#include "movement.h"
#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

Movement::Movement(int MLP1_,int MLP2_,int MRP1_,int MRP2_,volatile uint64_t* lc1_,volatile uint64_t* rc1_, int* lIR_, int* rIR_)
         :sPID (&irError,&sOp,&irSetPoint,DEF_SKP,DEF_SKI,DEF_SKD,REVERSE),
          dPID (&lrc,&dOp,&dSetPoint,DEF_DKP,DEF_DKI,DEF_DKD,DIRECT),
          alPID(&lc,&lmop,&angle_pid_setPoint,DEF_AKP,DEF_AKI,DEF_AKD,DIRECT),
          arPID(&rc,&rmop,&angle_pid_setPoint,DEF_AKP,DEF_AKI,DEF_AKD,DIRECT) {
  
  ledcSetup(0, FREQ, RESOLUTION);
  ledcSetup(1, FREQ, RESOLUTION);
  ledcSetup(2, FREQ, RESOLUTION);
  ledcSetup(3, FREQ, RESOLUTION);
  
  MLP1 = MLP1_;
  MLP2 = MLP2_;
  MRP1 = MRP1_;
  MRP2 = MRP2_;

  ledcAttachPin(MLP1, 0);
  ledcAttachPin(MLP2, 1);
  ledcAttachPin(MRP1, 2);
  ledcAttachPin(MRP2, 3);

  lc1 = lc1_;
  rc1 = rc1_;

  lIR = lIR_;
  rIR = rIR_;


  
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

void Movement::motor_l(double a) {
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

void Movement::motor_r(double a) {
  if(a>=0)a+=3;
  else a-=3;
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
    lc = *lc1;
    if (lc > angle_pid_setPoint) {
      lmop = LMinSpeed;
      motor_l(LMinSpeed);
      if (LMinSpeed > 100)LMinSpeed -= 0.1;
    }
    else {
      alPID.Compute();
      lmop = 150 + lmop / (255.0 / 105.0);
      if (lc < angle_pid_setPoint / 2)
        lmop = 150 + 255 - lmop;
      motor_l(-lmop);
    }

    rc = *rc1;
    if (rc > angle_pid_setPoint) {
      rmop = RMinSpeed;
      motor_r(-RMinSpeed);
      if (RMinSpeed > 100)RMinSpeed -= 0.1;
    }
    else {
      arPID.Compute();
      rmop = 150 + rmop / (255.0 / 105.0);
      if (rc < angle_pid_setPoint / 2)
        rmop = 150 + 255 - rmop;
      motor_r(rmop);
    }

    if(RMinSpeed <= 140 && LMinSpeed <= 140){
       *lc1 = 0;
       *rc1 = 0;
       LMinSpeed = 150;
       RMinSpeed = 150;
       stop();
       break;
     }
   }
}

void Movement::slowlturn(){
  *lc1 = 0;
  *rc1 = 0;
  while(true){

    lc = *lc1;
    rc = *rc1;
    if(lc > (146) && rc > (146)){
      //forceStop();
      LMinSpeed = 160;
      RMinSpeed = 160;
      while(LMinSpeed >= 60 && RMinSpeed >= 60){
        motor_l(-LMinSpeed);
        motor_r(RMinSpeed);
        LMinSpeed-=0.01;
        RMinSpeed-=0.01;
      }
      LMinSpeed = 160;
      RMinSpeed = 160;
      //
      stop();
      lc = 0;
      rc = 0;
      *lc1 = 0;
      *rc1 = 0;
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
  *lc1 = 0;
  *rc1 = 0;
  while(true){

    lc = *lc1;
    rc = *rc1;
    if(lc > (153) && rc > (153)){
      //forceStop();
      LMinSpeed = 160;
      RMinSpeed = 160;
      while(LMinSpeed >= 60 && RMinSpeed >= 60){
        motor_l(LMinSpeed);
        motor_r(-RMinSpeed);
        LMinSpeed-=0.01;
        RMinSpeed-=0.01;
      }
      LMinSpeed = 160;
      RMinSpeed = 160;
      //
      stop();
      lc = 0;
      rc = 0;
      *lc1 = 0;
      *rc1 = 0;
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

void Movement::sPIDTune(){
  l = analogRead(IRLeftPinm);
  r = analogRead(IRRightPinm);

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

  if (stopped == true) {
   stop();
  }
  else{
   motor_l(-(mSpeed - scaled_op - scaledSpeedRedux));
   motor_r(-(mSpeed+ 2 + scaled_op - scaledSpeedRedux)); //DON'T REMOVE THAT 2!!!!
  }
}

void Movement::forwardTune(){
  l = analogRead(IRLeftPinm);
  r = analogRead(IRRightPinm);

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

  lc = *lc1;rc = *rc1; //Distance PID
  lrc = -(dRange - ((lc+rc)*dRange)/(2*counts));
  dPID.Compute();
  scaledSpeedRedux = dRange-dOp;//(dist-dOp*(dist)/opLim);

  if (stopped == true) {
   stop();
  }
  else if (stopped == false) {
     if((rc+lc) >= 2*counts){
     stop();
     *lc1 = 0;
     *rc1 = 0;
     stopped = true;
   }
   else{
     motor_l(-(mSpeed - scaled_op - scaledSpeedRedux));
     motor_r(-(mSpeed+ 2 + scaled_op - scaledSpeedRedux)); //DON'T REMOVE THAT 2!!!!
   }
  }
}

// void Movement::forwardTuneNew(){
//   l = analogRead(IRLeftPinm);
//   r = analogRead(IRRightPinm);

//   if(l > L_IR_THRESHOLD && r > R_IR_THRESHOLD){
//     irError = 0;
//     tempMode = 'n';
//     sPID.SetMode(MANUAL);
//     sOp = 0;
//     scaled_op = 0;
//   }
//   else if(l > L_IR_THRESHOLD){
//     irError = (R_IR_SETPOINT - r)/isf;
//     tempMode = 'r';
//     sPID.SetMode(AUTOMATIC);
//   }
//   else if(r > R_IR_THRESHOLD){
//     irError = (l - L_IR_SETPOINT)/isf;
//     tempMode = 'l';
//     sPID.SetMode(AUTOMATIC);
//   }
//   else{
//     irError = (l - r)/3.995; //-3995 +3995 -> -1000 +1000 //Straight PID
//     tempMode = 'b';
//     sPID.SetMode(AUTOMATIC);
//   }

//   sPID.Compute();
//   scaled_op = sOp/sf;//sf

//   lc = *lc1;rc = *rc1; //Distance PID
//   lrc = -(dRange - ((lc+rc)*dRange)/(2*counts)); //the '-' is because of 'setpoint - input' inside library
//   dPID.Compute();

//   if((rc+lc) >= counts*2/3.0)
//     scaledSpeedRedux = dRange - dOp;//(dist-dOp*(dist)/opLim);
//   else
//     scaledSpeedRedux = dOp - 10;//(dist-dOp*(dist)/opLim);  

//   if (stopped == true) {
//    stop();
//   }
//   else if (stopped == false) {
//      if((rc+lc) >= 2*counts){
//      stop();
//      *lc1 = 0;
//      *rc1 = 0;
//      stopped = true;
//    }
//    else{
//      motor_l(-(mSpeed - scaled_op - scaledSpeedRedux));
//      motor_r(-(mSpeed+ 2 + scaled_op - scaledSpeedRedux)); //DON'T REMOVE THAT 2!!!!
//    }
//   }
// }



void Movement::forwardTuneNew(){
  // l = analogRead(IRLeftPinm);
  // r = analogRead(IRRightPinm);
  

  if (stopped == true) {
   stop();
  }
  else if (stopped == false) {
     if((rc+lc) >= counts*(1.8)){
     forceStop();
     stop();
     *lc1 = 0;
     *rc1 = 0;
     stopped = true;
     dPID.SetTunings(dPID.GetKp(),dPID.GetKi(),dPID.GetKd());
     tempMode = 'f';
   }
  else{
      if((rc+lc) < counts){
        scaledSpeedRedux = 160+((lc+rc)/(counts))*(mSpeed-160);
        tempMode = 'a';
      }
      else{
        lrc = -dRange*((lc+rc)/counts - 1);//(dist/2.0)*((counts-(lc+rc))/(counts));
        dPID.Compute();
        scaledSpeedRedux = mSpeed - dOp;
        tempMode = 'd';
      }

      motor_l(-(scaledSpeedRedux));
      motor_r(-(2 + scaledSpeedRedux)); //DON'T REMOVE THAT 2!!!!
   }
  }
}
void Movement::slowfwd(){
   while(true){
    lc = *lc1;rc = *rc1;

    int a = analogRead(IRLeftPinm);
    int b = analogRead(IRRightPinm); 
    l = linearize(a);
    r = linearize(b);

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

    if(wallFront()){
      if(tempMode != 'F'){
      forceStop();
      tempMode = 'F';
    }
      lc = 0;
      rc = 0;
      *lc1 = 0;
      *rc1 = 0;
     //dPID.SetTunings(dPID.GetKp(),dPID.GetKi(),dPID.GetKd());
     //stopped = true;
      return;
    }

     if((rc+lc) >= 2*(counts)){
     forceStop();
      lc = 0;
      rc = 0;
      *lc1 = 0;
      *rc1 = 0;
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
void Movement::slowfwdspidtune(){
    lc = *lc1;rc = *rc1;
    double l = linearize(analogRead(IRLeftPinm))-2;
    double r = linearize(analogRead(IRRightPinm));

    if(stopped == false){
    if(l > L_IR_THRESHOLD && r > R_IR_THRESHOLD){
      irError = 0;
      tempMode = 'n';
      if(sPID.GetMode() == AUTOMATIC)
        sPID.SetMode(MANUAL);
      sOp = 0;
      scaled_op = 0;
    }
    else if(l > L_IR_THRESHOLD && r < R_IR_THRESHOLD){
      if(r <= 8.55){
        r = -1.687292*(r-8.55);
      }
      else{
        r = -2.152046*(r-8.55);
      }
      irError = r; //r sensor
      tempMode = 'r';
      if(sPID.GetMode() == MANUAL)
        sPID.SetMode(AUTOMATIC);
    }
    else if(r > R_IR_THRESHOLD && l < L_IR_THRESHOLD){
      if(l <= 8.55){
        l = 2.44*(l-8.55);
      }
      else{
        l = 1.6355*(l-8.55);
      }
      irError = l;

      tempMode = 'l';
      if(sPID.GetMode() == MANUAL)
        sPID.SetMode(AUTOMATIC);
    }
    else{
      irError = (l - r); //-both sensors
      tempMode = 'b';
      if(sPID.GetMode() == MANUAL)
        sPID.SetMode(AUTOMATIC);
    }

    irError *= sf;
    sPID.Compute();
    scaled_op = sOp/sf;
     
    motor_l(-(mSpeed - scaled_op));
    motor_r(-(mSpeed + scaled_op));
    
  }
  else if(stopped == true){
    if(tempMode != 'F')
     forceStop();
    lc = 0;
    rc = 0;
    *lc1 = 0;
    *rc1 = 0;
    tempMode = 'F';
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
void Movement::acc9cm(){

}
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
void Movement::turnRightTune(){
  
}

void Movement::turnLeftTune(){
  if(stopped == false){

    lc = *lc1;
    if (lc > angle_pid_setPoint) {
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

    rc = *rc1;
    if (rc > angle_pid_setPoint) {
      rmop = RMinSpeed;
      motor_r(RMinSpeed);
      if (RMinSpeed > 100)RMinSpeed -= 0.1;
    }
    else {
      arPID.Compute();
      rmop = 150 + rmop / (255.0 / 105.0);
      if (rc < angle_pid_setPoint / 2)
        rmop = 150 + 255 - rmop;
      motor_r(-rmop);
    }

    if(LMinSpeed <= 100 && RMinSpeed <= 100){
       *lc1 = 0;
       *rc1 = 0;
       LMinSpeed = 150;
       RMinSpeed = 150;
       stopped = true;
     }
   }
   else{
    stop();
   }
}

void Movement::forceStop(){
  LMinSpeed = 180;
  RMinSpeed = 180;
  while(LMinSpeed >= 0 && RMinSpeed >= 0){
    motor_l(LMinSpeed);
    motor_r(RMinSpeed);
    LMinSpeed-= 0.01;
    RMinSpeed-= 0.01;
  }
  // LMinSpeed = 160;
  // RMinSpeed = 160;
  stop();
}

void Movement::stop(){
  motor_l(0);
  motor_r(0);
}

void Movement::acceleration(){
  *lc1 = 0;
  *rc1 = 0;
  dPID.SetTunings(0.1567398,0,0);

  while(true){
    lrc = (*lc1 + *rc1);

    if(lrc>=359){ //8 cm
      *lc1 = 0;
      *rc1 = 0;
      return;  
    }
    else if(lrc>=319){
      motor_l(-210);  
      motor_r(-210); 
    }
    else{
      tempMode = 'a';
      dPID.Compute();
      motor_l(-160+dOp);  
      motor_r(-160+dOp);  
    }
  }
}

void Movement::accwpid(){
  *lc1 = 0;
  *rc1 = 0;
     
  while(true){
    lrc = (*lc1 + *rc1);
    double l = linearize(analogRead(IRLeftPinm))-2;
    double r = linearize(analogRead(IRRightPinm));

    if(lrc>=359){ //8 cm
      *lc1 = 0;
      *rc1 = 0;
      return;  
    }
    else if(lrc>=319){
      motor_l(-170);  
      motor_r(-170);  
    }
    else{
      tempMode = 'a';
      double sspeed = map((*lc1 + *rc1)*10.0,0.0,3192.6,1550,1850)/10.0;
      if(l > L_IR_THRESHOLD && r > R_IR_THRESHOLD){
      irError = 0;
      tempMode = 'n';
      if(sPID.GetMode() == AUTOMATIC)
        sPID.SetMode(MANUAL);
      sOp = 0;
      scaled_op = 0;
    }
    else if(l > L_IR_THRESHOLD && r < R_IR_THRESHOLD){
      if(r <= 8.55){
        r = -1.687292*(r-8.55);
      }
      else{
        r = -2.152046*(r-8.55);
      }
      irError = r; //r sensor
      tempMode = 'r';
      if(sPID.GetMode() == MANUAL)
        sPID.SetMode(AUTOMATIC);
    }
    else if(r > R_IR_THRESHOLD && l < L_IR_THRESHOLD){
      if(l <= 8.55){
        l = 2.44*(l-8.55);
      }
      else{
        l = 1.6355*(l-8.55);
      }
      irError = l;

      tempMode = 'l';
      if(sPID.GetMode() == MANUAL)
        sPID.SetMode(AUTOMATIC);
    }
    else{
      irError = (l - r); //-both sensors
      tempMode = 'b';
      if(sPID.GetMode() == MANUAL)
        sPID.SetMode(AUTOMATIC);
    } 

      irError *= sf;
      sPID.Compute();
      scaled_op = sOp/sf;

      motor_l(-sspeed+scaled_op);  
      motor_r(-sspeed-scaled_op);  
    }
  }
}

void Movement::constSpeed(){
  tempMode = 'c';
  *lc1 = 0;
  *rc1 = 0;
  while((*lc1 + *rc1) <= 315){ //309
    if(wallFront()){
      if(fw == true)
        return;
      forceStop();
      fw = true;
      break;
    }
    double l = linearize(analogRead(IRLeftPinm))-2;
    double r = linearize(analogRead(IRRightPinm));

    if(l > L_IR_THRESHOLD && r > R_IR_THRESHOLD){
      irError = 0;
      tempMode = 'n';
      if(sPID.GetMode() == AUTOMATIC)
        sPID.SetMode(MANUAL);
      sOp = 0;
      scaled_op = 0;
    }
    else if(l > L_IR_THRESHOLD && r < R_IR_THRESHOLD){
      if(r <= 8.55){
        r = -1.687292*(r-8.55);
      }
      else{
        r = -2.152046*(r-8.55);
      }
      irError = r; //r sensor
      tempMode = 'r';
      if(sPID.GetMode() == MANUAL)
        sPID.SetMode(AUTOMATIC);
    }
    else if(r > R_IR_THRESHOLD && l < L_IR_THRESHOLD){
      if(l <= 8.55){
        l = 2.44*(l-8.55);
      }
      else{
        l = 1.6355*(l-8.55);
      }
      irError = l;

      tempMode = 'l';
      if(sPID.GetMode() == MANUAL)
        sPID.SetMode(AUTOMATIC);
    }
    else{
      irError = (l - r); //-both sensors
      tempMode = 'b';
      if(sPID.GetMode() == MANUAL)
        sPID.SetMode(AUTOMATIC);
    }

    irError *= sf;
    sPID.Compute();
    scaled_op = sOp/sf;
     
    motor_l(-(180 - scaled_op));
    motor_r(-(180 + scaled_op));
  }
  *lc1 = 0;
  *rc1 = 0;
}

void Movement::deceleration(double a){//a = 1-1block; 7.7-0block
  *lc1 = 0;
  *rc1 = 0;

  while(true){
      double sspeed = map((*lc1 + *rc1)*10,0,a*399,1850,-1300)/10.0;
      if((*lc1 + *rc1) >= a*39.9){
        *lc1 = 0;
        *rc1 = 0;
        forceStop();
        return;  
      }
      else{
        tempMode = 'd';
        motor_l(-sspeed);
        motor_r(-sspeed);  
      }
    }
}

void Movement::decwpid(double a){//a = 1-1block; 7.7-0block
  *lc1 = 0;
  *rc1 = 0;
  long t = millis();
  while(true){
      double l = linearize(analogRead(IRLeftPinm))-2;
      double r = linearize(analogRead(IRRightPinm));
      double sspeed = map((*lc1 + *rc1)*10,0,a*399.0,1850,-1300)/10.0;
      if(millis()-t > 3000)
        break;
      if((*lc1 + *rc1) >= a*39.9){
        *lc1 = 0;
        *rc1 = 0;
        forceStop();
        return;  
      }
      else{
        tempMode = 'd';
        if(l > L_IR_THRESHOLD && r > R_IR_THRESHOLD){
          irError = 0;
          tempMode = 'n';
          if(sPID.GetMode() == AUTOMATIC)
            sPID.SetMode(MANUAL);
          sOp = 0;
          scaled_op = 0;
        }
        else if(l > L_IR_THRESHOLD && r < R_IR_THRESHOLD){
          if(r <= 8.55){
            r = -1.687292*(r-8.55);
          }
          else{
            r = -2.152046*(r-8.55);
          }
          irError = r; //r sensor
          tempMode = 'r';
          if(sPID.GetMode() == MANUAL)
            sPID.SetMode(AUTOMATIC);
        }
        else if(r > R_IR_THRESHOLD && l < L_IR_THRESHOLD){
          if(l <= 8.55){
            l = 2.44*(l-8.55);
          }
          else{
            l = 1.6355*(l-8.55);
          }
          irError = l;

          tempMode = 'l';
          if(sPID.GetMode() == MANUAL)
            sPID.SetMode(AUTOMATIC);
        }
        else{
          irError = (l - r); //-both sensors
          tempMode = 'b';
          if(sPID.GetMode() == MANUAL)
            sPID.SetMode(AUTOMATIC);
        }

        irError *= sf;
        sPID.Compute();
        scaled_op = sOp/sf;
     
        motor_l(-(sspeed - scaled_op));
        motor_r(-(sspeed + scaled_op));
      }
    }
}

void Movement::speedTest(){
  if(stopped){
    if(tempMode == 'r'){
      forceStop();
      tempMode = 's';
    }
    stop();
  }
  else{
    tempMode = 'r';
    motor_l(-mSpeed);
    motor_r(-mSpeed);
  }
}

bool Movement::wallLeft(){
  return (linearize(analogRead(IRLeftPinm)) < L_IR_THRESHOLD);
}
bool Movement::wallRight(){
  return (linearize(analogRead(IRRightPinm)) < R_IR_THRESHOLD);
}
bool Movement::wallFront(){
  if(fw == true){
    fw = false;
    return true;
  }
  else
    return (linearize(analogRead(IRFrontPinm)) < F_IR_THRESHOLD);
}
