/** Message translation table **
 * Write:
 *   40 - "Is there a GUI out there?"
 *   42 - "Ok GUI, got you."
 *   50 - "I'm already paired."
 *   91 - "Power toggle succeeded."
 *   92 - "Power toggle failed."
 * Read:
 *   41 - "Hi Arduino, GUI here."
 *   88 - "End connection."
 *   90 - "Power the board on and off."
 *   05 - Action on FPGA buttons (1 data byte):
 **   00 - "Button 3 changed state."
 **   01 - "Button 2 changed state."
 **   02 - "Button 1 changed state."
 **   03 - "Button 0 changed state."
 **   04 - "Switch 17 changed state."
 **   05 - "Switch 16 changed state."
 **   06 - "Switch 15 changed state."
 **   07 - "Switch 14 changed state."
 **   08 - "Switch 13 changed state."
 **   09 - "Switch 12 changed state."
 **   10 - "Switch 11 changed state."
 **   11 - "Switch 10 changed state."
 **   12 - "Switch 9 changed state."
 **   13 - "Switch 8 changed state."
 **   14 - "Switch 7 changed state."
 **   15 - "Switch 6 changed state."
 **   16 - "Switch 5 changed state."
 **   17 - "Switch 4 changed state."
 **   18 - "Switch 3 changed state."
 **   19 - "Switch 2 changed state."
 **   20 - "Switch 1 changed state."
 **   21 - "Switch 0 changed state."
 *   127 - RESET
 **/

/** Currently using only 4 output data pins due to limited availability. **/

#define START_BYTE1 40
#define START_BYTE2 55
#define TOGGLE_FPGA_POWER 90
#define FPGA_BUTTON 05
#define CONFIRM_CONNECTION 41
#define POWER_TOGGGLE_SUCCEEDED 91
#define POWER_TOGGGLE_FAILED 92
#define END_CONNECTION 88
#define PAIRED 50
#define OK_GUI 42
#define RESET 127

byte power_pin = 2;
byte control_pin = 3;
int power_state = LOW;

int received;
byte bitread;
// bool is_sent = false;
bool is_connected = false;
bool is_power_blocked = false;

unsigned long beam_millis = millis(); // timestamp for last beam sent
// unsigned long sent_millis;            // ts for output clock state change
unsigned long input_millis;     // ts for last input received
unsigned long power_on_millis;  // ts for how long the board is on
unsigned long power_off_millis; // ts for last power toggle

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
  pinMode(9, OUTPUT);

  for (int j = 4; j < 10; j++)
    digitalWrite(j, LOW);
}


*/

void send(uint8_t *data, uint8_t type, uint8_t length)
{
  Serial.write(START_BYTE1);
  Serial.write(START_BYTE2);
  Serial.write(length);
  Serial.write(type);
  if (data != NULL && length != 0)
  {
    Serial.write(data, length);
  }
}

void received(uint8_t *data, uint8_t type, uint8_t length)
{
  switch (type)
  {
  case ECHO:
    send(data, ECHO, length);
    break;

  case CONFIRM_CONNECTION:
    if (!is_connected)
    {
      send(NULL, 42, 0);
      is_connected = true;
    }
    break;

  case TOGGLE_FPGA_POWER:
    if (is_power_blocked)
    {
      send(NULL, 92, 0);
      else if (power_state)
      {
        is_power_blocked = true;
        power_off_millis = millis();
        power_state = LOW;
        digitalWrite(power_pin, power_state);
        send(NULL, 91, 0);
        ;
      }
      else
      {
        is_power_blocked = true;
        power_off_millis = millis();
        power_state = HIGH;
        digitalWrite(power_pin, power_state);
        send(NULL, 91, 0);
        ;
      }
    }
    break;

  case RESET:

    for (int j = 0; j < 6; j++)
    {
      digitalWrite(j + 4, HIGH);
    }
    delay(1);
    digitalWrite(control_pin, HIGH);
    delay(1);
    digitalWrite(control_pin, LOW);
    // is_sent = true;
    // sent_millis = millis();
    break;

  case END_CONNECTION:

    is_connected = false;
    if (power_state)
    {
      is_power_blocked = true;
      power_off_millis = millis();
      power_state = LOW;
      digitalWrite(power_pin, power_state);
    }
    break;

  case FPGA_BUTTON:
    button = data[0];
    button_command(button);
    break;

  default:
    send(NULL, ERROR, 0);
    break;
  }
}

void button_command(button)
{
  for (int j = 0; j < 6; j++)
  {
    bitread = bitRead(received, j);
    digitalWrite(j + 4, bitread);
  }
  delay(1);
  digitalWrite(control_pin, HIGH);
  delay(1);
  digitalWrite(control_pin, LOW);
}

void loop()
{
  while (!Serial.available())
  {
    // Send beacon every 1 second
    if ((millis() - beam_millis) > 1000)
    {
      beam_millis = millis();
      if (is_connected)
        send(NULL, PAIRED, 0); // PAIRED = 50
      else
        send(NULL, CHECK_GUI, 0); // CHECK_GUI = 40
    }

    // Change output control signal state
    // if (is_sent && (millis() - sent_millis) > 100) {
    //  digitalWrite(control_pin, LOW);
    //  is_sent = false;
    //}

    // Unblock fpga power in 10 sec after last toggle
    if (is_power_blocked && (millis() - power_off_millis) > 10000)
      is_power_blocked = false;

    // Disconnect and turn off fpga if user is idle for 10 min
    if (is_connected && (millis() - input_millis) > 595000)
    {
      is_connected = false;
      if (power_state)
      {
        is_power_blocked = true;
        power_state = LOW;
        digitalWrite(power_pin, power_state);
        power_off_millis = millis();
      }
    }

    // Turn fpga off after 10 min of use
    if (power_state && (millis() - power_on_millis) > 600000)
    {
      is_power_blocked = true;
      power_state = LOW;
      digitalWrite(power_pin, power_state);
      power_off_millis = millis();
    }
  }

  input_millis = millis();

  uint8_t received_data[260];

  if (Serial.read() == 0xaa)
  {
    if (Serial.read() == 0x5f)
    {
      length = Serial.read();
      type = Serial.read();
      if (length > 0)
      {
        Serial.readBytes(received_data, length);
      }
      received(received_data, type, length);
    }
  }
}
