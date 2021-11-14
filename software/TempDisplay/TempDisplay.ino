/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

#define LED10   5 // Temperature >= 10degC, flashing => < 10degC
#define LED20   6 // Temperature >= 20degC
#define LED30   7
#define LED40   8
#define LED50   9
#define LED60   12
#define LED70   11
#define LED80   10 // Temperature >= 80degC

#define aStarLed 13
// 10k NTC characterization data (10kOhm @ 25degC)
const float ntcAdcVal[15] = {10 ,13 ,23 ,40 ,65,103 ,127,156,227,318,432,562,706,855,1004};
const float ntcTemp[15]   = {-25,-20,-10,0.0,10,20  ,25 ,30 ,40 ,50 ,60 ,70 ,80 ,90 ,100.0};
const unsigned int numNtcVal = 15;

static boolean extState = false;
static boolean hbLedState = false;
static boolean sendTel = false;
unsigned int telCnt = 0;
unsigned int adcVal = 0;
unsigned int ind1 = 0;
unsigned int ind2 = 0;

float rdTemp = 0.0;

// Use Interpolation to find the temperature from ADC value 
float getNtcTemp(float adcRdVal)
{
  unsigned int i;
  float adcVal = 0.0;
  float slope = 1.0;
  if(adcRdVal < ntcAdcVal[0])
  {
    adcVal = ntcAdcVal[0];
  }
  else
  if (adcRdVal > ntcAdcVal[numNtcVal-1])
  {
    adcVal = ntcAdcVal[numNtcVal-1];
  }
  else
  {   
    adcVal = adcRdVal;
  }  
  for(i=0;i<numNtcVal;i++)
  {
    ind2 = i;
    ind1 = i;
    if(adcVal == ntcAdcVal[i])
    {
        break;
    }
    else    
    if(adcVal < ntcAdcVal[i])
    {
        ind1 = i-1;
        break;
    }
  }    
  if(ind1 == ind2)
  {
    return ntcTemp[ind1];
  }    
  else
  {
    slope = (ntcTemp[ind2] - ntcTemp[ind1])/(ntcAdcVal[ind2] - ntcAdcVal[ind1]);
    return (slope * (adcVal-ntcAdcVal[ind1]) + ntcTemp[ind1]);
  }    
}

// Timer 1 Interrupt Service Routine
ISR(TIMER1_COMPA_vect)
{
  static int cnt = 0;
  static int cntExt = 0;
  cnt++;
  if(cnt == 125 || cnt == 375 || cnt == 625 || cnt == 875)
  {
    extState = !extState;
  }
  if(cnt == 875)
  {
    // Indicate it is time to send telemetry
    sendTel = true;  
    // heart beat Led
    hbLedState = !hbLedState;
    digitalWrite (aStarLed, extState ? HIGH : LOW);
  }
  if(cnt > 1000)
  {
    cnt = 0;
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
  // Initialize analog input
  pinMode(A0,INPUT);
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED10, OUTPUT);
  pinMode(LED20, OUTPUT);
  pinMode(LED30, OUTPUT);
  pinMode(LED40, OUTPUT);
  pinMode(LED50, OUTPUT);
  pinMode(LED80, OUTPUT);
  pinMode(LED70, OUTPUT);
  pinMode(LED60, OUTPUT);
  digitalWrite(LED10, LOW);
  digitalWrite(LED20, LOW);
  digitalWrite(LED30, LOW);
  digitalWrite(LED40, LOW);
  digitalWrite(LED50, LOW);
  digitalWrite(LED80, LOW);
  digitalWrite(LED70, LOW);
  digitalWrite(LED60, LOW);

  // set up Timer 1
  TCCR1A = 0;                         // normal operation
  TCCR1B = bit(WGM12) | bit(CS10);    // CTC, no pre-scaling
  OCR1A =  15999;                     // compare counter value with A register value
  TIMSK1 = bit (OCIE1A);              // interrupt on Compare A Match

  // Set up serial comms
  Serial.begin(115200);

  // Set up ADC
  analogReference(INTERNAL);
}

// the loop function runs over and over again forever
void loop() {
  
  if(sendTel)
  {
    sendTel = false;
    telCnt++;
    adcVal = analogRead(A0);
    rdTemp = getNtcTemp(adcVal);
    Serial.print("SolarMain,CenterTemp,");
    Serial.println(rdTemp);
  }
  // Temp > 80
  if((rdTemp >= 80) && (rdTemp <= 90))
  {
     digitalWrite(LED80, HIGH);
  }
  else
  if(rdTemp >90)
  {
     digitalWrite(LED80, extState);
  }
  else
  {
    digitalWrite(LED80, LOW);
  }
  // Temp > 70
  if(rdTemp >= 70)
  {
     digitalWrite(LED70, HIGH);
  }
  else
  {
     digitalWrite(LED70, LOW);
  }  
  // Temp > 60
  if(rdTemp >= 60)
  {
     digitalWrite(LED60, HIGH);
  }
  else
  {
     digitalWrite(LED60, LOW);
  }
  // Temp > 50
  if(rdTemp >= 50)
  {
     digitalWrite(LED50, HIGH);
  }
  else
  {
     digitalWrite(LED50, LOW);
  }
  // Temp > 40  
  if(rdTemp >= 40)
  {
     digitalWrite(LED40, HIGH);
  }
  else
  {
     digitalWrite(LED40, LOW);
  }  
 // Temp > 30 
  if(rdTemp >= 30)
  {
     digitalWrite(LED30, HIGH);
  }
  else
  {
     digitalWrite(LED30, LOW);
  }  
 // Temp > 20 
  if(rdTemp >= 20)
  {
     digitalWrite(LED20, HIGH);
  }
  else
  {
     digitalWrite(LED20, LOW);
  }  

// Temp > 10 
  if(rdTemp >= 10)
  {
     digitalWrite(LED10, hbLedState);
  }
  else
  {
     digitalWrite(LED10, extState);
  }  
}
