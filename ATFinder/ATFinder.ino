/****************************************************** 
 * Author: Juan Antonio Castro García
 * Date: 12/03/2017 (DD/MM/YYYY)
 * License: LGPL v3.0
 * Contact: jcastrodte AT gmail DOT com
 ******************************************************/

/******************************************************
 * Wiring to HC 05 (6 pins):
 *  Arduino -------- HC-05
 *  pin10 ---------- TXD
 *  pin11 ---------- RXD
 *  pin8  ---------- EN
 *  pin9  ---------- VCC
 *  GND   ---------- GND 
 *
 *
 ******************************************************
 * Wiring to HC 06 (4 pins):
 *  Arduino -------- HC-06
 *  pin10 ---------- TXD
 *  pin11 ---------- RXD
 *  pin9  ---------- VCC
 *  GND   ---------- GND 
 */

/****************************************************** 
 *  Setup options:
 ******************************************************/
/* 
 * Set the data rate for a serial transmission for communicating 
 * with the computer. Note: it is an integer.
 * 0=> 300 bauds, 1=>1200, 2=>2400, 3=>4800, 4=>9600 (default), 5=>19200,
 * 6=>38400, 7=>57600, 8=>115200, 9=>230400, 10=>250000
 */
int baudos_ard = 6;

/*
 * Set the data rate for a bluetooth transmission. Note: it is a char.
 * 1=>1200 bauds, 2=>2400, 3=>4800, 4=>9600 (default), 5=>19200, 6=>38400, 
 * 7=>57600, 8=>115200
 */
char baudios     = '6';  

// True: configure with our variables. False: ask for Bluetooth configuration. 
boolean configurar = true;


/*
 *  If 'configurar' is true, these options will be loading to the Bluetooth...
 */
// Bluetooth module name.
char ssid[]   = "ArdSlave"; 
 
// Bluetooth password.
char password[] = "1234";   

// Role setting. '1' for master mode, '0' for slave mode.
char rol = '0';



/********************************************************* 
 * The code does not need to be modified from here on out
 *********************************************************/
unsigned int pin_key = 8;
unsigned int pin_vcc = 9;
unsigned int pin_tx  = 10;
unsigned int pin_rx  = 11;

#include <SoftwareSerial.h>

// Used pins for serial connection with bluetooth module.
SoftwareSerial BT(pin_tx, pin_rx); // SoftwareSerial's RX and TX.

// Data rate supported by Bluetooth
unsigned long baud [] = {1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200};
// Data rate supported by Arduino
unsigned long baud_ard [] = {300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 
                             115200, 230400, 250000};

// Auxiliary variables
int cont = -1;
boolean encontrado = false;
unsigned long mtime=0;
unsigned long slot = 5000; // 5 seconds.
unsigned long slot2 = 1000; // 1 second.
char ant=0;
char act=0;
  
void setup()
{
  Serial.begin(baud_ard[baudos_ard]);
  pinMode(pin_key, OUTPUT);        
  pinMode(pin_vcc, OUTPUT);        
  digitalWrite(pin_key, HIGH);
  delay (500) ;              
  digitalWrite (pin_vcc, HIGH); 
} 

void loop()
{ 
  if(mtime < millis() && !encontrado)
  {
    cont ++ ;
    Serial.println();
    Serial.print("Attempt to communicate with the HC module at ");
    Serial.println(baud[cont]);
    BT.begin(baud[cont]);
    Serial.print("\tWaiting for confirmation: ");
    mtime = millis() + slot;
    BT.write("AT");
  }
  else if(mtime < millis() && encontrado && cont < 6)
  {
    configura(cont);
    cont ++;
    mtime = millis() + slot2;
  }

  if (BT.available())
  {
    ant = act;
    act = BT.read();
    Serial.write(act);
    if(ant == 'O' && act == 'K' && !encontrado) 
    {
      Serial.println("\nConfirmed!");
      if(configurar == true)
      {
        Serial.println("Loading configuration...");
      }
      else
      {
        Serial.println("Loading scan mode...");
      }
      encontrado = true;
      cont = 0;
      mtime = millis() + slot2;
    }
  }


  if(encontrado && cont >= 6)
  {
    if(Serial.available())
    {
      act = Serial.read();
      BT.write(act);
    }
    if(BT.available())
    {
      act = BT.read();
      Serial.write(act);
    }
  }
  
}

void configura(int i)
{
  if(configurar == false)
    {
      if(i == 0)
      {
        // Ask for the Bluetooth's name:
        Serial.println();
        Serial.println("AT+NAME?");
        BT.print("AT+NAME?"); 
      } 
      else if(i == 1)
      {
        // Ask for the bauds rate:
        Serial.println();
        Serial.println("AT+UART?");
        BT.print("AT+UART?");
      }
      else if(i == 2)
      {
        Serial.println();
        // Ask for the role:
        Serial.println("AT+ROLE?");
        BT.print("AT+ROLE?"); 
      }
      else if(i == 3)
      {
        Serial.println();
        // Ask for the password:
        Serial.println("AT+PSWD?");
        BT.print("AT+PSWD?"); 
      }
      else if(i == 4)
      {
        Serial.println();
        // Ask for the physical address:
        Serial.println("AT+ADDR?");
        BT.print("AT+ADDR?"); 
      }
    }
    else
    {
      if(i == 0)
      {
        // Set the Bluetooth's name:
        Serial.println();
        Serial.print("AT+NAME ");
        Serial.println(ssid);
        BT.print("AT+NAME"); 
        BT.print(ssid);
      }
      else if(i == 1)
      {
        // Set the bauds rate:
        Serial.println();
        Serial.print("AT+BAUD ");
        Serial.println(baudios);
        BT.print("AT+BAUD"); 
        BT.print(baudios);
      }
      else if(i == 2)
      {
        // Set the role:
        Serial.println();
        Serial.print("AT+ROLE ");
        Serial.println(rol);
        BT.print("AT+ROLE"); 
        BT.print(rol); 
      }
      else if(i == 3)
      {
        Serial.println();
        Serial.print("AT+PIN ");
        Serial.println(password);
        // Set the password:
        BT.print("AT+PIN"); 
        BT.print(password); 
      }
      
    }
    if(i == 5)
    {

      Serial.println();
      if(configurar)
      {
        Serial.println("Configured!");
      }
      else
      {
        Serial.println("Read completed!");
      }
      Serial.println();
      Serial.println("Start of manual configuration by AT commands (optional):");
      
    }
}

   
