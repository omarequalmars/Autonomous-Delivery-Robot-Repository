#include "DCMotorHBridge.h"
#include "MRAC_Left.h"
#include "MRAC_Right.h"

#define Left_motor_enc1 2
#define Left_motor_enc2 3

#define Right_motor_enc1 18
#define Right_motor_enc2 19

// F = Front
// B = Back
// R = Right
// L = Left
#define FR_PWM A0
#define FR_dir 4

#define FL_PWM A1
#define FL_dir 5

#define BL_PWM A2
#define BL_IN1 6
#define BL_IN2 7

#define BR_PWM A3
#define BR_IN1 8
#define BR_IN2 9
// makes sure to know what is the PPR of the encoder
float PPR_old = 4*370.0;
float PPR_new = 1900;
#define Freq_samp 200.0
#define T_samp 0.005

// Lyapunov Params
#define initial_gain 4
#define maximum_gain_P 10
#define maximum_gain_I 10
#define learning_rate 0.0001
#define time_constant 0.1
// PI Coefficients
#define P 3
#define I 2
#define antiwindup_yes 1
#define antiwindup_no 0

int left_Cstate = 0;
int left_Cstate_1 = 0;
volatile int left_ticks = 0;

int right_Cstate = 0;
int right_Cstate_1 = 0;
volatile int right_ticks = 0;

int current_Rticks = 0;
int speed_R = 0;
int last_Rticks = 0;
volatile int PWM_right = 0;

int current_Lticks = 0;
int speed_L = 0;
int last_Lticks = 0;
volatile int PWM_left = 0;

float setpoint_R = 0;
float setpoint_L = 0;

void setup() {
  Serial.begin(115200);

  pinMode(Left_motor_enc1, INPUT);
  pinMode(Right_motor_enc1, INPUT);

  pinMode(FL_PWM, OUTPUT);
  pinMode(FL_dir, OUTPUT);

  pinMode(FR_PWM, OUTPUT);
  pinMode(FR_dir, OUTPUT);

  pinMode(BL_PWM, OUTPUT);
  pinMode(BL_IN1, OUTPUT);
  pinMode(BL_IN2, OUTPUT);

  pinMode(BR_PWM, OUTPUT);
  pinMode(BR_IN2, OUTPUT);
  pinMode(BR_IN1, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(Left_motor_enc1), L_pulses, CHANGE);
  attachInterrupt(digitalPinToInterrupt(Left_motor_enc2), L_pulses, CHANGE);

  attachInterrupt(digitalPinToInterrupt(Right_motor_enc1), R_pulses, CHANGE);
  attachInterrupt(digitalPinToInterrupt(Right_motor_enc2), R_pulses, CHANGE);

    cli();      //stop interrupts
  TCCR2A = 0; // set entire TCCR2A register to 0
  TCCR2B = 0; // same for TCCR2B
  TCNT2 = 177; //255-((16*10^6)/(PreSc*F_samp))
  //
  //
  //
  // 177 for 200 Hz sampling rate
  // 240 for 1000 Hz sampling rate
  // 1000 Hz sampling rate is not advisable as not all calculations in the timer
  // may have time to complete, this will cause malfunction in the control system
  // when changing timer configurations, remember to change sampling frequency and sampling time
  // as used in calculations, which will be found as #defines at the top of the code
  // also make sure to change the TCNT2 value in the bottom of the ISR 
  // 
  //
  //
  TCCR2A |= 0; // turn on Normal mode
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // Set CS22,CS21,CS20 bits for 1024 prescaler
  TIMSK2 |= (1 << TOIE2);  // enable timer Overflow interrupt
  sei(); //allow interrupts
  Serial.print("PWM Right");
  Serial.print(" ");
  Serial.print("PWM Left");
  Serial.print(" ");
  Serial.print("Right Speed");
  Serial.print(" ");
  Serial.println("Left Speed");
}

void loop() {

}

ISR(TIMER2_OVF_vect) {
  current_Rticks = right_ticks;
  current_Lticks = left_ticks;
  
  // Speed is in RPM
  speed_L = (current_Rticks - last_Rticks)*Freq_samp/PPR_old*60;
  speed_R = -(current_Lticks - last_Lticks)*Freq_samp/PPR_new*60;

  // Speed is in RPM
  setpoint_R = 50;
  setpoint_L = 50;

  PWM_left = UnderControl_L(speed_L,setpoint_L, initial_gain, maximum_gain_P, maximum_gain_I,learning_rate,time_constant,T_samp);
  PWM_right = UnderControl_R(speed_R,setpoint_R, initial_gain, maximum_gain_P, maximum_gain_I,learning_rate,time_constant,T_samp);

  Freebird(PWM_left, FL_PWM, FL_dir);
  MoveitMoveit(PWM_left, BL_IN1, BL_IN2, BL_PWM);

  Freebird(-PWM_right, FR_PWM, FR_dir);
  MoveitMoveit(PWM_right, BR_IN1, BR_IN2, BR_PWM);

  Serial.print(PWM_right);
  Serial.print(" ");
  Serial.print(PWM_left);
  Serial.print(" ");
  Serial.print(speed_R);
  Serial.print(" ");
  Serial.println(speed_L);
  
  last_Rticks = current_Rticks;
  last_Lticks = current_Lticks;
  
  TCNT2 = 177;
}

void R_pulses() {
  right_Cstate = ((digitalRead(Right_motor_enc1) << 1) | (digitalRead(Right_motor_enc2)));
  if (right_Cstate_1 == 0 && right_Cstate == 1) {
    right_ticks--;
  } else if (right_Cstate_1 == 1 && right_Cstate == 3) {
    right_ticks--;
  } else if (right_Cstate_1 == 3 && right_Cstate == 2) {
    right_ticks--;
  } else if (right_Cstate_1 == 2 && right_Cstate == 0) {
    right_ticks--;
  } else if (right_Cstate_1 == 1 && right_Cstate == 0) {
    right_ticks++;
  } else if (right_Cstate_1 == 3 && right_Cstate == 1) {
    right_ticks++;
  } else if (right_Cstate_1 == 2 && right_Cstate == 3) {
    right_ticks++;
  } else if (right_Cstate_1 == 0 && right_Cstate == 2) {
    right_ticks++;
  }
  else;
  right_Cstate_1 = right_Cstate;
}

void L_pulses() {
  left_Cstate = ((digitalRead(Left_motor_enc1) << 1) | (digitalRead(Left_motor_enc2)));
  if (left_Cstate_1 == 0 && left_Cstate == 1) {
    left_ticks--;
  } else if (left_Cstate_1 == 1 && left_Cstate == 3) {
    left_ticks--;
  } else if (left_Cstate_1 == 3 && left_Cstate == 2) {
    left_ticks--;
  } else if (left_Cstate_1 == 2 && left_Cstate == 0) {
    left_ticks--;
  } else if (left_Cstate_1 == 1 && left_Cstate == 0) {
    left_ticks++;
  } else if (left_Cstate_1 == 3 && left_Cstate == 1) {
    left_ticks++;
  } else if (left_Cstate_1 == 2 && left_Cstate == 3) {
    left_ticks++;
  } else if (left_Cstate_1 == 0 && left_Cstate == 2) {
    left_ticks++;
  }
  else;
  left_Cstate_1 = left_Cstate;
}
