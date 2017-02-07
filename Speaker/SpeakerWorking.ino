// Scan I2C bus for device responses

#define SDA_PORT PORTD
#define SDA_PIN 3
#define SCL_PORT PORTD
#define SCL_PIN 5
#define I2C_TIMEOUT 100
#define I2C_NOINTERRUPT 0
#define I2C_SLOWMODE 1
#define FAC 1
#define I2C_CPUFREQ (F_CPU/FAC)


/* Corresponds to A4/A5 - the hardware I2C pins on Arduinos
#define SDA_PORT PORTC
#define SDA_PIN 4
#define SCL_PORT PORTC
#define SCL_PIN 5
#define I2C_FASTMODE 1
*/

#include <SoftI2CMaster.h>
#include <avr/io.h>

//------------------------------------------------------------------------------
void CPUSlowDown(int fac) {
  // slow down processor by a fac
  CLKPR = _BV(CLKPCE);
  CLKPR = _BV(CLKPS1) | _BV(CLKPS0);
}


uint8_t volume;
void setup(void) 
{
  volume=4;

#if FAC != 1
  CPUSlowDown(FAC);
#endif

  Serial.begin(9600); // change baudrate to 2400 on terminal when low CPU freq!
  Serial.println(F("Intializing ..."));
  Serial.print("I2C delay counter: ");
  Serial.println(I2C_DELAY_COUNTER);
  if (!i2c_init())
    Serial.println(F("Initialization error. SDA or SCL are low"));
  else
    Serial.println(F("...done"));
}
boolean initializing = true;
boolean onMute = false;
String inputString = "";
boolean stringComplete = false;

void loop(void)
{
  serialEvent();
  if (initializing)
    startSpeakerOnChannel(1);
}

void serialEvent() 
{
  while (Serial.available()) {
    
    char inChar = (char)Serial.read();

    if (inChar != '\r' && inChar != '\n') 
    {
      inputString += inChar;
    }
    if (inChar == '\n') 
    {
      stringComplete = true;
    }
  }
  // print the string when a newline arrives:
  if (stringComplete) 
  {
    processSerialData();
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

void processSerialData()
{
  if(inputString == "svu")
  {
     increaseVolume();
  }
  else if(inputString == "svd")
  {
     decreaseVolume();
  }
  else if(inputString == "svm")
  {
     muteVolume();
  }
  else if(inputString == "scd")
  {
     startSpeakerOnChannel(2);
  }
  else if(inputString == "sca")
  {
     startSpeakerOnChannel(1);
  }
  
}

void increaseVolume()
{
  onMute=false;
  if(volume!=6)
  {
    volume++;
    sendSetting();
  }
}

void decreaseVolume()
{
  if(volume!=0)
  {
    volume--;
    sendSetting();
  }
}

void muteVolume()
{
  onMute=!onMute;
  sendSetting();
}

void sendSetting()
{
  uint8_t add = 0x94;
  int found = false;

  Serial.println("Read 8-bit Addr : ");
  // try read
  do {
    if (i2c_start(add | I2C_READ)) {
      found = true;
      i2c_read(true);
      i2c_stop();
      Serial.println(add + I2C_READ, HEX);
    } else i2c_stop();
    add += 2;
  } while (add);
  
  Serial.println("Update Volume");
  // try write
  add = 148;
  do {
    if (i2c_start(add | I2C_WRITE)) {
      found = true;
      i2c_write(add + I2C_WRITE);
      sendVolume();
      i2c_stop();
      Serial.println(add + I2C_WRITE, HEX);
    } else i2c_stop();
    i2c_stop();
    add += 2;
  } while (add);
}

void sendVolume()
{
  i2c_write(B10010100);
  i2c_write(B11100000);
  if(onMute)
  {
    i2c_write(214);
    return;
  }
  i2c_write(214-volume);
}

void setDefaultSettings(int channel)
{
  if(channel==1)
  {
    Serial.println("Enabled AUX Input");
    i2c_write(B10010100);
    i2c_write(B11001001);
  }
  else if(channel==2)
  {
    i2c_write(B10010100);
    i2c_write(B11001111);
  }

  delay(100);
  i2c_write(B10010100);
  i2c_write(B11100000);
  i2c_write(210);
} 
 
//Channel 1 = aux and channel 2 = dvd
void startSpeakerOnChannel(int channel)
{
  uint8_t add = 0x94;
  int found = false;
  Serial.println("Scanning ...");

  Serial.println("Read 8-bit Addr : ");
  // try read
  do {
    if (i2c_start(add | I2C_READ)) {
      found = true;
      i2c_read(true);
      i2c_stop();
      Serial.println(add + I2C_READ, HEX);
    } else i2c_stop();
    add += 2;
  } while (add);

  Serial.println("Write 8-bit Addr : ");
  // try write
  add = 148;
  do {
    if (i2c_start(add | I2C_WRITE)) {
      found = true;
      i2c_write(add + I2C_WRITE);
      setDefaultSettings(channel);
      i2c_stop();
      Serial.println(add + I2C_WRITE, HEX);
    } else i2c_stop();
    i2c_stop();
    add += 2;
  } while (add);

  initializing = false;

  delay(1000 / FAC);
}


