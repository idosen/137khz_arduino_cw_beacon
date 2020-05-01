/*
 Arduino cw beacon 137khz
 9A5ADI
*/

#define ANTENNA_PIN PB3 //Arduino Nano/Uno D11
#define N_MORSE  (sizeof(morsetab)/sizeof(morsetab[0]))

#define SPEED  (19)
#define DOTLEN  (1200/SPEED)
#define DASHLEN  (3*(1200/SPEED))

int LEDpin = 7;
struct t_mtab { char c, pat; } ;

struct t_mtab morsetab[] = {
    {'.', 106},
  {',', 115},
  {'?', 76},
  {'/', 41},
  {'A', 6},
  {'B', 17},
  {'C', 21},
  {'D', 9},
  {'E', 2},
  {'F', 20},
  {'G', 11},
  {'H', 16},
  {'I', 4},
  {'J', 30},
  {'K', 13},
  {'L', 18},
  {'M', 7},
  {'N', 5},
  {'O', 15},
  {'P', 22},
  {'Q', 27},
  {'R', 10},
  {'S', 8},
  {'T', 3},
  {'U', 12},
  {'V', 24},
  {'W', 14},
  {'X', 25},
  {'Y', 29},
  {'Z', 19},
  {'1', 62},
  {'2', 60},
  {'3', 56},
  {'4', 48},
  {'5', 32},
  {'6', 33},
  {'7', 35},
  {'8', 39},
  {'9', 47},
  {'0', 63},
  {'-', 97}
} ;


void dash()
{
  digitalWrite(LEDpin, HIGH) ;
  DDRB |= (1 << ANTENNA_PIN);
  delay(DASHLEN);
  DDRB &= ~(1 << ANTENNA_PIN);
  digitalWrite(LEDpin, LOW) ;
  delay(DOTLEN) ;
}

void dit()
{
  digitalWrite(LEDpin, HIGH) ;
  DDRB |= (1 << ANTENNA_PIN);
  delay(DOTLEN);
  DDRB &= ~(1 << ANTENNA_PIN);
  digitalWrite(LEDpin, LOW) ;
  delay(DOTLEN);
}

void send(char c)
{
  int i ;
  if (c == ' ') {
    Serial.print(c) ;
    delay(7*DOTLEN) ;
    return ;
  }
  for (i=0; i<N_MORSE; i++) {
    if (morsetab[i].c == c) {
      unsigned char p = morsetab[i].pat ;
      Serial.print(morsetab[i].c) ;

      while (p != 1) {
          if (p & 1)
            dash() ;
          else
            dit() ;
          p = p / 2 ;
      }
      delay(2*DOTLEN) ;
      return ;
    }
  }
  /* if we drop off the end, then we send a space */
  Serial.print("?") ;
}

void sendmsg(char *str)
{
  while (*str)
    send(*str++) ;
  Serial.println("");
}


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  pinMode(LEDpin, OUTPUT) ;

  //Carrier Frequency generation
  uint32_t fTransmit = 137; //KHz
  DDRB |= (1 << ANTENNA_PIN);
  TCCR2A = (0 << COM2A1) + (1 << COM2A0); //Toggle OC0A on Compare Match
  TCCR2A |= (1 << WGM21) + (0 << WGM20); //CTC
  TCCR2B = (0 << CS22) + (0 << CS21) + (1 << CS20); //No Prescaling
  OCR2A = F_CPU / (2000 * fTransmit) - 1;
  
  char strbuf[255];
  sprintf(strbuf, "Will broadcast at %d KHz", (F_CPU / (2 * (1 + OCR2A)) / 1000));
  Serial.println(strbuf);

  
}

ISR(TIMER1_OVF_vect) {
  uint8_t adcl = ADCL;
  uint8_t adch = ADCH;
  OCR1A = (adch << 8) + adcl;
  DDRB |= (1 << ANTENNA_PIN);
}

ISR(TIMER1_COMPA_vect) {
  DDRB &= ~(1 << ANTENNA_PIN);
}

void loop() {
  DDRB |= (1 << ANTENNA_PIN);
  digitalWrite(LEDpin, HIGH) ;
  delay(5000);
  DDRB &= ~(1 << ANTENNA_PIN);
  delay(500);
  sendmsg("V V V  V V V DE 9A5ADI/B LOC JN95JG ") ;
  delay(1000);
}
