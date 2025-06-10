// Board Name : ESP32-WROOM-DA Module

// coin vibrator pins (9)
int vib_pin[] = {4, 25, 33, 32, 26, 27, 14, 12, 13};

// switch pins (3)
int switch_pin[] = {18, 21, 19};

// additional gpio pin headers (4)
int gpio_pin[] = {2, 25, 22, 23};

void setup() {
  Serial.begin(9600);

  // set vibrator pins as output
  for(int i=0;i<9;i++)pinMode(vib_pin[i], OUTPUT);

  // set switch pins as output
  for(int i=0;i<3;i++) pinMode(switch_pin[i], INPUT);
}

void loop() {
  all_vib_test();
  switch_test();
}

// function to test coin vibrator pins - switches the states continuously
void all_vib_test(){
  for(int i=0;i<9;i++) digitalWrite(vib_pin[i], HIGH);
  delay(1000);
  for(int i=0;i<9;i++) digitalWrite(vib_pin[i], LOW);
  delay(1000);
}

// sets a single pin to HIGH
void single_vib_test(int i){
  int check_vib = vib_pin[i];
  digitalWrite(check_vib, HIGH);
  delay(1000);
}

// reads the state of each pin and displays in serial monitor
void switch_test(){
  for(int i=0;i<3;i++){
    int state = digitalRead(switch_pin[i]);
    Serial.print(state);
    Serial.print(" ");
  }
  Serial.println();
  delay(1000);
}
