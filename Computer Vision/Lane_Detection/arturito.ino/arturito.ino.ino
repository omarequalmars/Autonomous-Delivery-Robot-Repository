int ENa = 5;
int IN1 = 7;
int IN2 = 6;
int ENb = 9;
int IN3 = 10;
int IN4 = 11;
int speed =255;

void setup() {
  Serial.begin(9600); // Start the serial communication
  pinMode(ENa, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENb, OUTPUT);
}

void loop() {
  if (Serial.available() > 0) { // Check if there is any data available
    float curvature = Serial.parseFloat(); // Read the curvature value
    
    if (curvature > 0.15) { // Turn right
      turnRight();
    } else if (curvature < -0.15) { // Turn left
      turnLeft();
    } else { // Go straight
      goStraight();
    }
  }
}

void goStraight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENa, speed);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENb, speed);
}

void turnLeft() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENa, speed);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENb, 0);
}

void turnRight() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENa, 0);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENb, speed);
}