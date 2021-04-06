/*
 * This is on the Hbridge connecting to the arduino breakout
 */
#define dir1PinL   3   //Motor direction IN3
#define dir2PinL   4   //Motor direction IN4
#define speedPinL  5   // Needs to be a PWM pin to be able to control motor speed. ENB

#define dir1PinR    7  //Motor direction  IN1 
#define dir2PinR    2  //Motor direction  IN2
#define speedPinR   6   // Needs to be a PWM pin to be able to control motor speed. This goes to ENA

//Setting up the PS2 Controller
#include <PS2X_lib.h>
PS2X ps2x; //creates PS2 Controller Class
byte type = 0;
byte vibrate = 0;
int error = 0;

void setup() {
  init_GPIO();
  brake();
  error = ps2x.config_gamepad(13,11,10,12, false, false); //configures the controller with the ports and asking if vibration should be on; clock, command, attenton, data. CMD and ATN must be PMW
  type = ps2x.readType(); //reads to see what kind of controller this is
  delay(500);
  Serial.begin(9600); //this is here just to aid in debugging if needed.
}

void loop() { // put your main code here, to run repeatedly:
  // Speeds struggle below a certain point. So if the motors start to "jitter" you need to push forawrd on the sticks more.
 /////////////////////////////////////////////////////////////
  ps2x.read_gamepad(false, vibrate); //reads the gamepad

  int nJoyL = leftJoystick(); //calibrates left joystick
  int nJoyR = rightJoystick(); //calibrates right joystick
  /*
   * This is basically just saying if the left joystick is pushed forward, the rover will move forward.
   * If the joystick is pushed back, then the rover will move backwards.
   * The rover operates on tank control meaning that left joystick controls left side and right joystick controls right side.
   */
  if(nJoyL>50) {      
        digitalWrite(dir1PinL,LOW);      
        digitalWrite(dir2PinL,HIGH);      
        analogWrite(speedPinL, nJoyL); //replace "nJoyL" with 1023 if the rover doesn't move  
  }           
  if(nJoyL<-50) {      
        digitalWrite(dir1PinL,HIGH);      
        digitalWrite(dir2PinL,LOW);      
        analogWrite(speedPinL, abs(nJoyL)); //replace "abs(nJoyL)" with 1023 if the rover doesn't move  
  }      
  if (abs(nJoyL)<50) {      
        analogWrite(speedPinL, 0);           
  } 
  if(nJoyR>50) {      
        digitalWrite(dir1PinR, LOW);
        digitalWrite(dir2PinR, HIGH);     
        analogWrite(speedPinR, nJoyR); //replace "nJoyR" with 1023 if the rover doesn't move  
  }           
  if(nJoyR<-50) {      
        digitalWrite(dir1PinR,HIGH);      
        digitalWrite(dir2PinR,LOW);      
        analogWrite(speedPinR, abs(nJoyR)); //replace "abs(nJoyR)" with 1023 if the rover doesn't move         
  }      
  if (abs(nJoyR)<50) {      
        analogWrite(speedPinR, 0);           
  } 
    delay(50);  
}

/*
 * Basic subroutines called upon often
 */
 
/*
 * The left and right joystick functions are made in order to throw out any potential outliers. 
 * There is still a chance that some are found. Replace "4" in the for loop to a higher number 
 * if you desire to remove more outliers, however this comes at the price of lag and input delay.
 * If you want to decrease input delay, lower the "50" in the delay, or decrease the "4" in the for loop.
 * These same notes apply to the leftJoystick() function.
 * There is definately a better way to address the outlier issue (like using a z-score or the IQR rule), but this is the best I got currently.
 */
int rightJoystick() {
for(int i=0;i<=4;i++){ //<--- decrease/increase the "4" here to decrease/increase lag. Decreased lag comes at cost of potentially not ignoring outliers
  ps2x.read_gamepad(false, vibrate); //gets values from gamepad    
  int nJoyR = ps2x.Analog(PSS_RY); // read right stick                    
  nJoyR = map(nJoyR, 0, 255, 1023, -1023); //maps the joystick different for the sake of sanity
  delay(50); //<--- change this to decrease/increase lag. Decreased lag comes at cost of potentially not ignoring outliers
  ps2x.read_gamepad(false, vibrate); //gets values from gamepad 
  int nJoyR1 = ps2x.Analog(PSS_RY); // read right stick again saving it as a second value
  nJoyR1 = map(nJoyR1, 0, 255, 1023, -1023); //maps the joystick different for the sake of sanity

  //Reads joysticks twice to set up two variables. 
  //This then compares to see if there is too much of a difference between the two readings and then throws out the outlier
  
  if(abs(nJoyR-nJoyR1)<100){ 
    nJoyR=nJoyR;
  }else if((abs(nJoyR)<abs(nJoyR1)) && (abs(nJoyR-nJoyR1)>100)){
    nJoyR=nJoyR;
  }else if((abs(nJoyR)>abs(nJoyR1)) && (abs(nJoyR-nJoyR1)>100)){
    nJoyR=nJoyR1;
  }else{
    nJoyR=0;
  }
   return nJoyR;
  }
}

int leftJoystick(){
  for(int i=0;i<=4;i++){ //<--- decrease/increase the "4" here to decrease/increase lag. Decreased lag comes at cost of potentially not ignoring outliers
  ps2x.read_gamepad(false, vibrate); //gets values from gamepad
  int nJoyL = ps2x.Analog(PSS_LY); // read left stick                    
  nJoyL = map(nJoyL, 0, 255, 1023, -1023); //maps the joystick different for the sake of sanity         
  delay(50); //<--- change this to decrease/increase lag. Decreased lag comes at cost of potentially not ignoring outliers
  ps2x.read_gamepad(false, vibrate); //gets values from gamepad
  int nJoyL1 = ps2x.Analog(PSS_LY); // read left stick  
  nJoyL1 = map(nJoyL1, 0, 255, 1023, -1023); //maps the joystick different for the sake of sanity
  
  //Reads joysticks twice to set up two variables. 
  //This then compares to see if there is too much of a difference between the two readings and then throws out the outlier
   
  if(abs(nJoyL-nJoyL1)<100){ //was 200
    nJoyL=nJoyL;
  }else if((abs(nJoyL)<abs(nJoyL1)) && (abs(nJoyL-nJoyL1)>100)){
    nJoyL=nJoyL;
  }else if((abs(nJoyL)>abs(nJoyL1)) && (abs(nJoyL-nJoyL1)>100)){
    nJoyL=nJoyL1;
    nJoyL=nJoyL1;
  }else{
    nJoyL=0;
  }
   return nJoyL;
  }
}

//This stops are the rover
void brake(){
  digitalWrite(dir1PinL, LOW);
  digitalWrite(dir2PinL, LOW);
  digitalWrite(dir1PinR, LOW);
  digitalWrite(dir2PinR, LOW);
}

//This just sets the pins to output
void init_GPIO()
{
  pinMode(dir1PinL, OUTPUT); 
  pinMode(dir2PinL, OUTPUT); 
  pinMode(speedPinL, OUTPUT);  
 
  pinMode(dir1PinR, OUTPUT);
  pinMode(dir2PinR, OUTPUT); 
  pinMode(speedPinR, OUTPUT); 
  brake();
}

//More functions might be added for an autonomous program.
