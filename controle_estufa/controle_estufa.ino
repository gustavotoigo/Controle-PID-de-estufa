
#define PINO_ZC   2
#define PINO_DIM  9
#define PINO_LM35 A0
#define N         10

float temp,
      temp2;

int original,
    filtrado;
    
int numbers[N];

volatile float lum, lumm;

float error = 0,
       lastRPM;

//SINTONIA 1
float  kP = 5.4592,
       kI = 63.0804, 
       kD = 15.293;

//SINTONIA 2
/*
double kP = 4.0776,
       kI = 79.7305, 
       kD = 15.9326;
int controlSignal = 0;
*/
float  P = 0,                 //Proporcional
       I = 0,                 //integral
       D = 0,                 //derivativa
       PID = 0;               //soma P+I+D
       
float SetPoint = 40;       //Alterar aqui o setpoint

float lastProcess = 0,
      deltaTime = 0;
     
int flag = 1;

void setup() {
  pinMode(PINO_DIM, OUTPUT);
  pinMode(PINO_ZC, INPUT);
  pinMode(PINO_LM35, INPUT);

  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(2), ZeroCross, RISING);

}

void ZeroCross(){
  long t1 = 8200L * (100 - lum) / 100L;
  delayMicroseconds(t1);
  digitalWrite(PINO_DIM, HIGH);
  delayMicroseconds(6);
  digitalWrite(PINO_DIM, LOW);
}
    
void loop() {
  
 
  JumpFirstN();
  
 
  original = analogRead(PINO_LM35);                       //LE O VALOR DO SENSOR DE TEMPERATURA (0-1023)
  filtrado = MovingAverage();  

  
  temp = (float(filtrado)*5/(1023))/0.01;                 //TRANSOFORMA VALOR PARA TEMPERATURA (0-100)
  temp2 = (float(original)*5/(1023))/0.01;        
  
  error = SetPoint - temp;                                //CALCULA O ERRO DO CICLO 
  
  float deltaTime = (millis() - lastProcess) / 1000.0;    //CALCULA O VALOR DO DELTA DE TEMPO E CONVERTE PRA SEGUNDOS
  lastProcess = millis();                                 //ARMAZENA O VALOR DO INSTANTE

  P = error*kP;                                           //CALCULA O VALOR DE P                                               
    
  I = I + (error*kI) * deltaTime;                         //CALCULA O VALOR DE I
  
  D = (lastRPM - temp)*kD*deltaTime;                      //CALCULA O VALOR DE D
  lastRPM = temp;

  PID = P + I + D;                                        //SOMA PID
  
  noInterrupts();
  
  lumm = map(PID, -10000, 10000, 0, 85);
  //lumm = PID*85/10000;
  
    
  if(lumm < 3) 
    lum = 3;
  else
    lum = lumm;
  
  if(lum > 85) lum = 85;
  interrupts();

 
  Serial.print(lum);
  Serial.print(" ");
  Serial.print(error);
  Serial.print(" ");
  Serial.print(PID);
  Serial.print(" ");
  Serial.println(temp);

  //delay(100);

  PID = 0;
}


void JumpFirstN(void){
  if(flag == 1){
   for(uint8_t i = 0; i < 10; i++) 
    int filtrado = analogRead(PINO_LM35);
   flag = 0;
 }
}

float MovingAverage(){

   for(uint8_t i = N-1; i > 0; i--) 
      numbers[i] = numbers[i-1];
    
   numbers[0] = original;
    
   float acc = 0;
    
   for(uint8_t i = 0; i < N; i++) 
      acc += numbers[i];
    
   return acc/N;
}
