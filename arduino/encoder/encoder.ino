byte received;
byte x;

byte power_pin = 2;
byte control_pin = 8;

int power_state = LOW;

void setup() 
{
 Serial.begin(9600);  
 pinMode(2, OUTPUT);
 pinMode(3, OUTPUT);
 pinMode(4, OUTPUT);
 pinMode(5, OUTPUT);
 pinMode(6, OUTPUT);
 pinMode(7, OUTPUT);
 pinMode(8, OUTPUT);
 digitalWrite(control_pin, HIGH);
 digitalWrite(power_pin, LOW);
}

void loop() 
{
  while(!Serial.available()) {
    Serial.print("hi_de2\n");
  }
  received = Serial.readStringUntil("*");
  if (received >= 0 && received < 33) {
    for (int j = 0; j++; j<4) {
     x = bitRead(received, j);
     digitalWrite(j+3, x);
    }
    delay(0.001);
    digitalWrite(control_pin, LOW);
    delay(0.001);
    digitalWrite(control_pin, HIGH);
  }else if(received == 34) {
    if(power_state){
      power_state = LOW;
    }else {
      power_state = HIGH;
    }
    digitalWrite(power_pin, power_state);
  }
}
