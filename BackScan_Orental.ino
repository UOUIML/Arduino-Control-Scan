
int  incomingByte = 0; // for incoming serial data

bool isFrontScan_flag = false;
bool isGrinding_flag   = false;
bool isBackMove_flag = false;
bool isFrontMove_flag = false;
bool isReturnHome     = false;


int PullGrindingPin = 13;
int DirGrindingPin  = 9;
const byte limitSwitchLeft = 3;// outputA digital pin2
const byte limitSwitchRight = 5;//outoutB digital pin3
int timeDelayGrinding = 1;

int DirScanForward = 12;
int DirScanBackward = 11;
int isRelayPin     = 10;

volatile int32_t temp = 0;
volatile int32_t counter = 0;
const volatile int32_t MaximumCounter = 50;

const byte PullCounter = 2;
#define readA bitRead(PIND,PullCounter)//faster than digitalRead()


void GrindingTrigger(){
  digitalWrite(PullGrindingPin, HIGH);   
  delayMicroseconds(2);              
  digitalWrite(PullGrindingPin, LOW);   
}

void JogForward(){
  digitalWrite(DirScanBackward, HIGH);
  delayMicroseconds(10); 
  digitalWrite(DirScanForward, LOW);
  delayMicroseconds(10);     
}

void JogBackward(){
  digitalWrite(DirScanForward, HIGH);
  delayMicroseconds(10);     
  digitalWrite(DirScanBackward, LOW);
  delayMicroseconds(10);
}

void StopMotor(){
  digitalWrite(DirScanForward, HIGH);
  delayMicroseconds(10);     
  digitalWrite(DirScanBackward, HIGH);
  delayMicroseconds(10);
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin (9600);
  pinMode(PullCounter, INPUT_PULLUP);
  pinMode(DirScanForward, OUTPUT);
  pinMode(DirScanBackward, OUTPUT);
  pinMode(isRelayPin, OUTPUT); 
  
  pinMode(DirGrindingPin, OUTPUT);
  pinMode(PullGrindingPin, OUTPUT); 
  pinMode(limitSwitchLeft, INPUT);
  pinMode(limitSwitchRight, INPUT);
  

  //B rising pulse from encodenren activated ai1(). AttachInterrupt 1 is DigitalPin nr 3 on moust Arduino.
  attachInterrupt(digitalPinToInterrupt(PullCounter), PullMotorCounter, RISING);
  

  // set motor off
  digitalWrite(DirScanForward, HIGH);
  digitalWrite(DirScanBackward, HIGH);
  digitalWrite(isRelayPin, LOW);
   
  digitalWrite(PullGrindingPin, LOW); 
  digitalWrite(DirGrindingPin, LOW);  
 
}

void loop() {
  // put your main code here, to run repeatedly:
//  if ( counter != temp ) {
//      Serial.println (counter);
//      temp = counter;
//  }
    
   if (Serial.available() > 0){
      incomingByte = Serial.read();
     if(incomingByte == 'G'){
        Serial.write("T\n");
        isReturnHome    = false;
        
        isGrinding_flag = true;
        isFrontScan_flag = false;
        
        isBackMove_flag = false;
        isFrontMove_flag = false;
        digitalWrite(isRelayPin, HIGH);
        delay(100);
      }
      else if(incomingByte == 'Q'){
        Serial.write("Q\n");
        counter = 0;
        temp    = 0;
        isReturnHome = true;

        isGrinding_flag = false;
        isFrontScan_flag = true;
        
        isBackMove_flag = false;
        isFrontMove_flag = false;

        digitalWrite(isRelayPin, LOW);
        delay(100); 
        JogForward();
      }
      else if(incomingByte == 'B'){
        Serial.write("B\n");
        counter = 0;
        temp    = 0;
        isReturnHome    = false;
        
        isGrinding_flag = false;
        isFrontScan_flag = false;
        
        isBackMove_flag = true;
        isFrontMove_flag = false;
        
        digitalWrite(isRelayPin, HIGH);
        delay(50); 
        JogBackward();
      }
      else if(incomingByte == 'F'){
        Serial.write("F\n");
        counter = 0;
        temp    = 0;
        isGrinding_flag = false;
        isFrontScan_flag = false;
        isBackMove_flag = false;
        isFrontMove_flag = true;
        isReturnHome    = false;
        
        digitalWrite(isRelayPin, HIGH);
        delay(50);    
        JogForward();
      }
      else{
        Serial.write("S\n");
        isFrontScan_flag = false;
        isGrinding_flag = false;
        isFrontMove_flag = false;
        isBackMove_flag = false;
        isReturnHome    = false;
        counter = 0;
        temp    = 0;
        digitalWrite(isRelayPin, HIGH);
        delay(50); 
        StopMotor();
      }
      
   }

   if(isGrinding_flag == true){
      if(digitalRead(limitSwitchLeft) == LOW && digitalRead(limitSwitchRight) == HIGH){
         digitalWrite(DirGrindingPin,HIGH);
         
      }
      else if(digitalRead(limitSwitchLeft) == HIGH && digitalRead(limitSwitchRight) == LOW){
         digitalWrite(DirGrindingPin,LOW);
        
      }
       digitalWrite(PullGrindingPin, HIGH);   
       delayMicroseconds(timeDelayGrinding);              
       digitalWrite(PullGrindingPin, LOW);
       delayMicroseconds(timeDelayGrinding);
   }

}

void PullMotorCounter() {

  if(isFrontScan_flag){
    if (readA == HIGH){
      counter ++;
    }
  }

  // checking counter
  if(counter == MaximumCounter){
      counter = 0;

      if(isReturnHome == true){
         digitalWrite(isRelayPin, HIGH);
         delay(100);
         JogBackward();
         isReturnHome = false;
      }
      else if(isReturnHome == false){
         StopMotor();
      }
  }
}



