#ifndef peripherals
#define peripherals

class IR{
public:
	IR(){};
	const int IRLeftPin = 32,
			  IRDiagLPin = 0,
			  IRFrontPin = 35,
			  IRDiagRPin = 0,
			  IRRightPin = 34;
	int leftIrRaw, rightIrRaw, frontIrRaw, DiagLeftIrRaw, DiagRightItRaw;

	bool wallLeft();
	bool wallRight();
	bool wallFront();

	double leftIR();
	double rightIR();
	double frontIR();
private:
	const int L_IR_THRESHOLD = 8,
			  R_IR_THRESHOLD = 8,
			  F_IR_THRESHOLD = 4;

	const int L_IR_SETPOINT = 4.3,
			  R_IR_SETPOINT = 4.3;

	double linearize(int);
};
#endif