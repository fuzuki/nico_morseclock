/*
 * LED morse clock
 *
 *
 */
 
#include <Time.h>  
#include <avr/sleep.h>

#define TIME_MSG_LEN  11   // time sync to PC is HEADER followed by unix time_t as ten ascii digits
#define TIME_HEADER  'T'   // Header tag for serial time sync message
#define TIME_REQUEST  7    // ASCII bell character requests a time sync message 

#define M0 "-----"
#define M1 ".----"
#define M2 "..---"
#define M3 "...--"
#define M4 "....-"
#define M5 "....."
#define M6 "-...."
#define M7 "--..."
#define M8 "---.."
#define M9 "----."

#define LED_PIN 8
#define TILT_SW 2

#define DotLength 400

volatile int flag;
volatile int ledFlag;
volatile time_t current;
char *morse[10];

void setup()  {
  Serial.begin(9600);
  setSyncProvider( requestSync);  //set function to call when sync required
  //Serial.println("Waiting for sync message");
  
  pinMode(LED_PIN, OUTPUT);
  pinMode(TILT_SW, INPUT);
  digitalWrite(TILT_SW,HIGH);//pullup

   morse[0] = M0;
   morse[1] = M1;
   morse[2] = M2;
   morse[3] = M3;
   morse[4] = M4;
   morse[5] = M5;
   morse[6] = M6;
   morse[7] = M7;
   morse[8] = M8;
   morse[9] = M9;

  flag = 0;
  ledFlag = 0;

  set_sleep_mode(SLEEP_MODE_IDLE);
  attachInterrupt(0, flagUp,FALLING);
}

void loop(){
  if(Serial.available()){
    processSyncMessage();
  }

  if(flag){
    if(ledFlag){
      return;
    }else{
      ledFlag = 1;
      //時刻表示
      delay(3000);
      outTime();
      
      delay(10000);
      ledFlag = 0;
      flag = 0;
    }
  }

  sleep_mode();
  delay(1000);
}

void processSyncMessage() {
  // if time sync available from serial port, update time and return true
  while(Serial.available() >=  TIME_MSG_LEN ){  // time message consists of a header and ten ascii digits
    char c = Serial.read() ; 
    Serial.print(c);  
    if( c == TIME_HEADER ) {       
      time_t pctime = 0;
      for(int i=0; i < TIME_MSG_LEN -1; i++){   
        c = Serial.read();          
        if( c >= '0' && c <= '9'){   
          pctime = (10 * pctime) + (c - '0') ; // convert digits to a number    
        }
      }   
      setTime(pctime);   // Sync Arduino clock to the time received on the serial port
      Serial.println("Time set.");
    }else{
      //
    }
  }
}

time_t requestSync()
{
  Serial.write(TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}
/**
 * 
 */
void flagUp(){
  flag = 1;
}

/**
 *
 *
 */
void outTime(){
  int i,h,m;
  
  h = hour();
  h = (h + 9) % 24;
  m = minute();
  //Serial.println("");
  //if(Serial.available()){
  //  Serial.println(h);
  //  Serial.println(m);
  //}
  morseNum(h/10);
  delay(DotLength * 3);
  morseNum(h%10);
  delay(DotLength * 7);
  morseNum(m/10);
  delay(DotLength * 3);
  morseNum(m%10);

}


/**
 * bar length = dot length * 3
 * blank between dots or bars = dotlenght 
 * blank between codes = dot * 3
 * blank between words = dot * 7
 */
void morseNum(int n){
  int i;
  digitalWrite(LED_PIN,LOW);
  if(n < 0 || n > 9){
    return;
  }
  for(i=0;i<5;i++){
    //
    delay(DotLength);
    if(morse[n][i] == '.'){
      digitalWrite(LED_PIN,HIGH);
      delay(DotLength);
      digitalWrite(LED_PIN,LOW);
      //delay(DotLength);
    }else if(morse[n][i] == '-'){
      digitalWrite(LED_PIN,HIGH);
      delay(DotLength*3);
      digitalWrite(LED_PIN,LOW);
      //delay(DotLength);
    }
  }
}

