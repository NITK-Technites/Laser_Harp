#define C4 0x30
#define Cs4 0x31
#define D4 0x32
#define E4 0x34
#define F4 0x35
#define G4 0x37
#define A4a 0x39
#define B4 0x3B

#define C5 0x3C
#define D5 0x3E
#define E5 0x40
#define F5 0x41
#define G5 0x43
#define A5a 0x45
#define As5 0x46
#define B5 0x47
#define C6 0x48
#define Cs6 0x49

#define laser_pin 7
#define num_readings 50

uint8_t laser_string[8] = {C5, D5, E5, F5, G5, A5a, B5, C6};
uint8_t sel_pin[2] = {2, 3};
uint8_t analog_pin[2] = {A0, A1};
uint16_t sensor_data[8] = {0};
uint16_t threshold[8] = 512;
boolean prev_sensor_data = {0};

boolean calib = 0;
void setup() {
  // put your setup code here, to run once:
  init_pins();
  if (!calib)
  {
    calibrate_harp();
  }


  Serial.begin(115200);
}

void loop() {
  read_sensors();
  for (int i = 0; i < 8; i++)
  {
    if (prev_sensor_data[i] == 0 && sensor_data[i] == 1)
    {
      midi((0x90), laser_string[i], 64);

    }
    if (prev_sensor_data[i] == 1 && sensor_data[i] == 0)
    {
      midi((0x80), laser_string[i], 64);
    }
    prev_sensor_data[i] = sensor_data[i];
  }
}

void midi(int cmd, int data1, int data2)
{
  Serial.write(cmd);
  Serial.write(data1);
  Serial.write(data2);
}

void read_sensors()
{
  for (int i = 0; i < 4; i++)
  {
    digitalWrite(sel_pin[0], i & 0x01 );
    digitalWrite(sel_pin[1], (i >> 1) & 0x01); //bitmasking
    delay(1);
    sensor_data[i] = analogRead(analog_pin[0]);
    sensor_data[i + 4] = analogRead(analog_pin[1]);
    if (calib)
    {
      if (sensor_data[i] > threshold[i])
        sensor_data[i] = 1;
      else
        sensor_data[i] = 0;
      if (sensor_data[i + 4] > threshold[i + 4])
        sensor_data[i + 4] = 1;
      else
        sensor_data[i + 4] = 0;
    }
  }
}
void init_pins()
{
  pinMode(laser_pin, OUTPUT);
  for (int i = 0; i < 2; i++)
  {
    pinMode(sel_pin[i], OUTPUT);
    pinMode(analog_pin[i], INPUT);
  }
}
void calibrate_harp()
{
  uint16_t min_reading[8] = {0};
  uint16_t max_reading[8] = {0};
  for (int i = 0; i < 8; i++)
  {
    min_reading[i] = 1023;
    max_reading[i] = 0;

  }
  digitalWrite(laser_pin, LOW);
  for (int j = 0; j < num_readings; j++)
  {
    read_sensors();
    for ( int i = 0; i < 8; i++)
    {
      if (sensor_data[i] < min_reading[i])
        min_reading[i] = sensor_data[i];
    }

  }
  digitalWrite(laser_pin, HIGH);
  for (int j = 0; j < num_readings; j++)
  {
    read_sensors();
    for ( int i = 0; i < 8; i++)
    {
      if (sensor_data[i] > max_reading[i])
        max_reading[i] = sensor_data[i];
    }

  }
  for (int i = 0; i < 8; i++)
  {
    threshold[i] = (min_reading[i] + max_reading[i]) / 2;
  }

  calib = 1;
}
