//http://www.benripley.com/diy/arduino/three-ways-to-read-a-pwm-signal-with-arduino/
//PWM_PIN_LR Mitte=1500Ms Links=1000Ms Rechts=2000Ms
//PWM_PIN_T Mitte=1500Ms Zurück=1000Ms Vor=2000Ms

//define when RONs Board
#define RON

//define Threshold
#define Threshold 30
// Arduino pins for the shift register
#define MOTORLATCH 12
#define MOTORCLK 4
#define MOTORENABLE 7
#define MOTORDATA 8


#define PWM_MOTOR_L 6
#define PWM_MOTOR_R 5
#ifndef RON
#define PWM_PIN_LR 0
#define PWM_PIN_T 13
#endif //not RON
#ifdef RON
#define PWM_PIN_LR 14
#define PWM_PIN_T 15
#endif //RON

#define MOTOR_R_A 5
#define MOTOR_R_B 7
#define MOTOR_L_A 0
#define MOTOR_L_B 6

int pwm_value_LR;
int pwm_value_T;
int speed_r;
int speed_l;


void setup() {
  pinMode(PWM_PIN_LR, INPUT);
  pinMode(PWM_PIN_T, INPUT);
  pinMode(PWM_MOTOR_L, OUTPUT);
  pinMode(PWM_MOTOR_R, OUTPUT);
  pinMode(MOTOR_R_A, OUTPUT);
  pinMode(MOTOR_R_B, OUTPUT);
  pinMode(MOTOR_L_A, OUTPUT);
  pinMode(MOTOR_L_B, OUTPUT);
  Serial.begin(115200);
}

void loop() {
  pwm_value_T = pulseIn(PWM_PIN_T, HIGH)-1500;
  pwm_value_LR = pulseIn(PWM_PIN_LR, HIGH)-1500;
  speed_r = pwm_value_T + pwm_value_LR;
  speed_l = pwm_value_T - pwm_value_LR;
  Serial.println(speed_l);
  Serial.println(speed_r);
  bool forward;
  if (pwm_value_T > Threshold)
    forward = true;
  else if (pwm_value_T < -Threshold)
    forward = false;
  
  if(speed_r > Threshold && forward){
     shiftWrite(MOTOR_R_A, HIGH);
     shiftWrite(MOTOR_R_B, LOW);
     analogWrite(PWM_MOTOR_R, min(speed_r / 2, 255));
 
     } else if(speed_r < -Threshold && !forward) {
      shiftWrite(MOTOR_R_A, LOW);
      shiftWrite(MOTOR_R_B, HIGH);
      analogWrite(PWM_MOTOR_R, min(-speed_r / 2, 255));
     } else {
        analogWrite(PWM_MOTOR_R, 0);
      }

   if(speed_l > Threshold && forward){
     shiftWrite(MOTOR_L_A, HIGH);
     shiftWrite(MOTOR_L_B, LOW);
     analogWrite(PWM_MOTOR_L, min(speed_l / 2, 255));
 
     } else if(speed_l < -Threshold && !forward) {
      shiftWrite(MOTOR_L_A, LOW);
      shiftWrite(MOTOR_L_B, HIGH);
      analogWrite(PWM_MOTOR_L, min(-speed_l / 2, 255));
      
     } else {
        analogWrite(PWM_MOTOR_L, 0);
      }
  
}

// ---------------------------------
// shiftWrite
//
// The parameters are just like digitalWrite().
//
// The output is the pin 0...7 (the pin behind
// the shift register).
// The second parameter is HIGH or LOW.
//
// There is no initialization function.
// Initialization is automatically done at the first
// time it is used.
//
void shiftWrite(int output, int high_low)
{
  static int latch_copy;
  static int shift_register_initialized = false;

  // Do the initialization on the fly,
  // at the first time it is used.
  if (!shift_register_initialized)
  {
    // Set pins for shift register to output
    pinMode(MOTORLATCH, OUTPUT);
    pinMode(MOTORENABLE, OUTPUT);
    pinMode(MOTORDATA, OUTPUT);
    pinMode(MOTORCLK, OUTPUT);

    // Set pins for shift register to default value (low);
    digitalWrite(MOTORDATA, LOW);
    digitalWrite(MOTORLATCH, LOW);
    digitalWrite(MOTORCLK, LOW);
    // Enable the shift register, set Enable pin Low.
    digitalWrite(MOTORENABLE, LOW);

    // start with all outputs (of the shift register) low
    latch_copy = 0;

    shift_register_initialized = true;
  }

  // The defines HIGH and LOW are 1 and 0.
  // So this is valid.
  bitWrite(latch_copy, output, high_low);

  // Use the default Arduino 'shiftOut()' function to
  // shift the bits with the MOTORCLK as clock pulse.
  // The 74HC595 shiftregister wants the MSB first.
  // After that, generate a latch pulse with MOTORLATCH.
  shiftOut(MOTORDATA, MOTORCLK, MSBFIRST, latch_copy);
  delayMicroseconds(5);    // For safety, not really needed.
  digitalWrite(MOTORLATCH, HIGH);
  delayMicroseconds(5);    // For safety, not really needed.
  digitalWrite(MOTORLATCH, LOW);
}
