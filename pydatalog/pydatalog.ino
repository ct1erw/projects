//// AvgDAQ
/// Makes the potentially dangerous assumption that the analog pins are numbered in sequence, starting at A0

// Original code: http://rwsarduino.blogspot.pt/2014/12/python-plots-from-serial-input.html

//// Constants
int d = 1;          // delay in microseconds for ADC average
int navg = 10;      // number of samples to average
int nan = 6;        // number of analog channels
int nrpt = 500;     // time between lines of data in ms
int ndisp = 20000;  // maximum number of lines to display

void setup() {
  // configure ADC pins as inputs
  for(int i = 0; i < nan; i++) {
    pinMode(A0 + i, INPUT);
  }
  
  // initialize digital pin 13 as an output for the LED
  pinMode(13, OUTPUT);    
  
  // configure serial
  Serial.begin(57600);

  // configure ADC  
  analogReference(DEFAULT);      // 5 Volts
  //analogReference(EXTERNAL);   // based on the input to AREF
  
  // print banner
  Serial.print("\n\nAvgDAQ\n");
}

void loop() {
  static int linesShown = 0;
  unsigned long sums[10];    // doesn't want to work with NAN as a dimension
  
  getAvgDAQ(sums);
  if(linesShown < ndisp){ 
    showSums(sums);
    linesShown++;
  }
  
  // blink LED
  digitalWrite(13, LOW);   // turn the LED off
  
  // delay
  while(millis()%nrpt);
  
  // blink LED
  digitalWrite(13, HIGH);    // turn the LED on
}

void getAvgDAQ(unsigned long *sums) {
  
  // miliseconds since boot
  sums[nan] = (unsigned long) millis();

  // clear the array
  for(int i = 0;i < nan;i++) { 
    sums[i] = 0;
  }

  // read the ADC and average
  for(int i = 0;i < navg;i++) {
    for(int j = 0;j < nan;j++) { 
      sums[j] += analogRead(A0+j); 
      delayMicroseconds(d);
    }
  }
  
  // convert to mV, divide by ADC resolution, divide by average
  for(int i=0;i < nan;i++){ 
    sums[i] *= 5000;    // convert to mV for 5V internal ref
    sums[i] /= 1024;    // 10 bits ADC
    sums[i] /= navg;    // average over NAVG samples
  }

}

void showSums(unsigned long *sums) {
  char scratch[80];
  
  // print miliseconds since boot
  Serial.print(sums[nan]);
  
  // print the values
  scratch[0]=0;    // when in doubt code dangerously, writing a string over itself ;-)
  for(int i = 0;i < nan;i++) {
    sprintf(scratch,"%s,%5d",scratch,(int) sums[i]);
  }
  sprintf(scratch,"%s\n",scratch);
  Serial.print(scratch);

/* check this code. Some kind of bargraph???
  for(int i=0;i<80;i++) { 
    scratch[i] = '.';
  }
  scratch[79] = 0;
  
  for(int i=0;i<nan;i++){
    long j = sums[i] / 20;
    if(j > 78) j = 78;
    scratch[j] = '0'+i;
  }
  Serial.println(scratch);
*/
}

