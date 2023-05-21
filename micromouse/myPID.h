#ifndef myPID
#define myPID
#define LIBRARY_VERSION	1.1.1

class PID
{


  public:

  //Constants used in some of the functions below
  #define AUTOMATIC	1
  #define MANUAL	0
  #define DIRECT  0
  #define REVERSE  1
  #define P_ON_M 0
  #define P_ON_E 1

  //commonly used functions **************************************************************************
    PID(double*, double*, double*, double, double, double, int);
    PID(double*, double*, double*, double, double, double, int, int); //   (overload for specifying proportional mode)        
             
    void SetMode(int Mode);               // * sets PID to either Manual (0) or Auto (non-0)
    bool Compute();                       
    void SetOutputLimits(double, double); // * clamps the output to a specific range. 0-255 by default, bu
    
                                          
  //available but not commonly used functions ********************************************************
    void SetTunings(double, double,       // * While most users will set the tunings once in the 
                    double);         	    //   constructor, this function gives the user the option
                                          //   of changing tunings during runtime for Adaptive control
    void SetTunings(double, double,       // * overload for specifying proportional mode
                    double, int);         	  
	  void SetControllerDirection(int);	  // * Sets the Direction, or "Action" of the controller. DIRECT
                  										  //   means the output will increase when error is positive. REVERSE
                  										  //   means the opposite.  it's very unlikely that this will be needed
                  										  //   once it is set in the constructor.
    void SetSampleTime(int);            // * sets the frequency, in Milliseconds, with which the PID calculation is performed.  default is 100		
                                                            
  //Display functions ****************************************************************
    double GetKp();						  // These functions query the pid for interal values.
    double GetKi();						  //  they were created mainly for the pid front-end,
    double GetKd();						  // where it's important to know what is actually 
    int GetMode();						  //  inside the PID.
    int GetDirection();					  //

  private:
  
  	void Initialize();
  	
  	double dispKp;				// * we'll hold on to the tuning parameters in user-entered 
  	double dispKi;				//   format for display purposes
  	double dispKd;				//
      
  	double kp;                  // * (P)roportional Tuning Parameter
    double ki;                  // * (I)ntegral Tuning Parameter
    double kd;                  // * (D)erivative Tuning Parameter
  
  	int controllerDirection;
  	int pOn;

    double *myInput;              // * Pointers to the Input, Output, and Setpoint variables
    double *myOutput;             //   This creates a hard link between the variables and the 
    double *mySetpoint;           //   PID, freeing the user from having to constantly tell us
                                  //   what these values are.  with pointers we'll just know.
			  
  	unsigned long lastTime;
  	double outputSum, lastInput;
  
  	unsigned long SampleTime;
  	double outMin, outMax;
  	bool inAuto, pOnE;
};
#endif
