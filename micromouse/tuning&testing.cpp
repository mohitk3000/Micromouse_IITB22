// #include "movement.h"

// void Movement::speedTest(){
//   if(stopped){
//     stop();
//   }
//   else{
//     motor_l(-mSpeed);
//     motor_r(-mSpeed+2);
//   }
// }

// void Movement::turnLeftTune(){
//   if(stopped == false){

//     lc = *lc1;
//     if (lc > angle_pid_setPoint) {
//       lmop = LMinSpeed;
//       motor_l(-LMinSpeed);
//       if (LMinSpeed > 100)LMinSpeed -= 0.1;
//     }
//     else {
//       alPID.Compute();
//       lmop = 150 + lmop / (255.0 / 105.0);
//       if (lc < angle_pid_setPoint / 2)
//         lmop = 150 + 255 - lmop;
//       motor_l(lmop);
//     }

//     rc = *rc1;
//     if (rc > angle_pid_setPoint) {
//       rmop = RMinSpeed;
//       motor_r(RMinSpeed);
//       if (RMinSpeed > 100)RMinSpeed -= 0.1;
//     }
//     else {
//       arPID.Compute();
//       rmop = 150 + rmop / (255.0 / 105.0);
//       if (rc < angle_pid_setPoint / 2)
//         rmop = 150 + 255 - rmop;
//       motor_r(-rmop);
//     }

//     if(LMinSpeed <= 100 && RMinSpeed <= 100){
//        *lc1 = 0;
//        *rc1 = 0;
//        LMinSpeed = 150;
//        RMinSpeed = 150;
//        stopped = true;
//      }
//    }
//    else{
//     stop();
//    }
// }

// void Movement::turnRightTune(){
  
// }

// void Movement::slowfwdspidtune(){
//    //  tempMode = 's';
//    //  lc = *lc1;rc = *rc1;

//    //  l = analogRead(IRLeftPinm);
//    //  r = analogRead(IRRightPinm);

//    //   if(stopped == true && tempMode != 'F'){
//    //   forceStop();
//    //    lc = 0;
//    //    rc = 0;
//    //    *lc1 = 0;
//    //    *rc1 = 0;
//    //    tempMode = 'F';
//    //   //stopped = true;
//    //   return;
//    // }
//    //  else{
//    //      motor_l(-(mSpeed));
//    //      motor_r(-(2 + mSpeed));
//    //  }
//    /**/
//     lc = *lc1;rc = *rc1;
//     int a = analogRead(IRLeftPinm);
//     int b = analogRead(IRRightPinm); 
//     l = linearize(a);
//     r = linearize(b);

//     if(stopped == false){
//       tempMode = 'r';
//     if(l > L_IR_THRESHOLD && r > R_IR_THRESHOLD){
//       irError = 0;
//       tempMode = 'n';
//       if(sPID.GetMode() == AUTOMATIC)
//         sPID.SetMode(MANUAL);
//       sOp = 0;
//       scaled_op = 0;
//     }
//     else if(l > L_IR_THRESHOLD && r < R_IR_THRESHOLD){
//       irError = (R_IR_SETPOINT - r)*23;
//       // if(irError<=0)irError *= 17;
//       // else irError *= 52;
//       tempMode = 'r';
//       if(sPID.GetMode() == MANUAL)
//         sPID.SetMode(AUTOMATIC);
//     }
//     else if(r > R_IR_THRESHOLD && l < L_IR_THRESHOLD){
//       irError = (l - L_IR_SETPOINT)*23;
//       // if(irError>0)irError *= 52;
//       // else irError *= 17;
//       tempMode = 'l';
//       if(sPID.GetMode() == MANUAL)
//         sPID.SetMode(AUTOMATIC);
//     }
//     else{
//       irError = (l - r)*sf; //-3995 +3995 -> -1000 +1000 //Straight PID
//       tempMode = 'b';
//       if(sPID.GetMode() == MANUAL)
//         sPID.SetMode(AUTOMATIC);
//     }

//     sPID.Compute();
//     scaled_op = sOp/sf;//sf


     
//     motor_l(-(mSpeed - scaled_op));
//     motor_r(-(2 + mSpeed + scaled_op));
    
//   }
//   else if(stopped == true){
//     if(tempMode != 'F')
//      forceStop();
//       lc = 0;
//       rc = 0;
//       *lc1 = 0;
//       *rc1 = 0;
//      tempMode = 'F';
//    }
// }

// void Movement::forwardTune(){
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
//   lrc = -(dRange - ((lc+rc)*dRange)/(2*counts));
//   dPID.Compute();
//   scaledSpeedRedux = dRange-dOp;//(dist-dOp*(dist)/opLim);

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

// // void Movement::forwardTuneNew(){
// //   l = analogRead(IRLeftPinm);
// //   r = analogRead(IRRightPinm);

// //   if(l > L_IR_THRESHOLD && r > R_IR_THRESHOLD){
// //     irError = 0;
// //     tempMode = 'n';
// //     sPID.SetMode(MANUAL);
// //     sOp = 0;
// //     scaled_op = 0;
// //   }
// //   else if(l > L_IR_THRESHOLD){
// //     irError = (R_IR_SETPOINT - r)/isf;
// //     tempMode = 'r';
// //     sPID.SetMode(AUTOMATIC);
// //   }
// //   else if(r > R_IR_THRESHOLD){
// //     irError = (l - L_IR_SETPOINT)/isf;
// //     tempMode = 'l';
// //     sPID.SetMode(AUTOMATIC);
// //   }
// //   else{
// //     irError = (l - r)/3.995; //-3995 +3995 -> -1000 +1000 //Straight PID
// //     tempMode = 'b';
// //     sPID.SetMode(AUTOMATIC);
// //   }

// //   sPID.Compute();
// //   scaled_op = sOp/sf;//sf

// //   lc = *lc1;rc = *rc1; //Distance PID
// //   lrc = -(dRange - ((lc+rc)*dRange)/(2*counts)); //the '-' is because of 'setpoint - input' inside library
// //   dPID.Compute();

// //   if((rc+lc) >= counts*2/3.0)
// //     scaledSpeedRedux = dRange - dOp;//(dist-dOp*(dist)/opLim);
// //   else
// //     scaledSpeedRedux = dOp - 10;//(dist-dOp*(dist)/opLim);  

// //   if (stopped == true) {
// //    stop();
// //   }
// //   else if (stopped == false) {
// //      if((rc+lc) >= 2*counts){
// //      stop();
// //      *lc1 = 0;
// //      *rc1 = 0;
// //      stopped = true;
// //    }
// //    else{
// //      motor_l(-(mSpeed - scaled_op - scaledSpeedRedux));
// //      motor_r(-(mSpeed+ 2 + scaled_op - scaledSpeedRedux)); //DON'T REMOVE THAT 2!!!!
// //    }
// //   }
// // }

// void Movement::forwardTuneNew(){
//   // l = analogRead(IRLeftPinm);
//   // r = analogRead(IRRightPinm);
  

//   if (stopped == true) {
//    stop();
//   }
//   else if (stopped == false) {
//      if((rc+lc) >= counts*(1.8)){
//      forceStop();
//      stop();
//      *lc1 = 0;
//      *rc1 = 0;
//      stopped = true;
//      dPID.SetTunings(dPID.GetKp(),dPID.GetKi(),dPID.GetKd());
//      tempMode = 'f';
//    }
//   else{
//       if((rc+lc) < counts){
//         scaledSpeedRedux = 160+((lc+rc)/(counts))*(mSpeed-160);
//         tempMode = 'a';
//       }
//       else{
//         lrc = -dRange*((lc+rc)/counts - 1);//(dist/2.0)*((counts-(lc+rc))/(counts));
//         dPID.Compute();
//         scaledSpeedRedux = mSpeed - dOp;
//         tempMode = 'd';
//       }

//       motor_l(-(scaledSpeedRedux));
//       motor_r(-(2 + scaledSpeedRedux)); //DON'T REMOVE THAT 2!!!!
//    }
//   }
// }