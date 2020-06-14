// #include <Wire.h>
#include <I2C.h>

  float temperature = 0;
  int defaultAddress = 0x5A;      //5A = 1011010 --> appending 0 for write bit to end makes 10110100 = B4 nevermind don't need this
  int objRegister = 0x07;
  uint8_t errMessage = 0;   //No errors for I2c.read
  int dataBits1 = 0;
  int dataBits2 = 0;
  uint8_t errBits = 0;
  int result;
  int numBytes = 3;


  int tomaxAdd = 0x00;
  int tominAdd = 0x01;
  int tomax = 0;
  int tomin = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  //50kHz (must be in range 10kHz-100kHz for SMBus
  I2c.begin();
  //I2c.timeOut(500);
  I2c.pullup(true);     //Unsure if need physical resistors with this as well
}


void loop() {
  // put your main code here, to run repeatedly:
  
  errMessage = I2c.read(defaultAddress, objRegister, numBytes);

  //errMessage = I2c.read(defaultAddress, , numBytes);
  
  
  if (errMessage != 0)
  {
    Serial.print("Error number: ");
    Serial.print(errMessage, DEC);
    Serial.print("\n");
  }
  else
  {
      dataBits1 = I2c.receive() << 8;
      dataBits2 = I2c.receive() << 8;
      result = dataBits1;
      result <<= 8;
      result |= dataBits2;
      errBits = I2c.receive() << 8;
      //Serial.print("Byte 1: %d \n", byte1);
      Serial.print("Data Bits: ");
      Serial.print(result, DEC);
      //Serial.print(dataBits2, DEC);
      Serial.print("Error Code: ");
      Serial.print(errBits, DEC);
      Serial.print("\n");
      //Serial.print("Byte 2: %d \n", byte2);
      //Serial.print("Byte 3: %d \n", byte3);
  }
  
  delay(1000);
  
  //I2c.available(); 
}
