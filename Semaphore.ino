#define button PC0 //button pin 
//semaphore1
#define R1 PD5
#define Y1 PD6
#define G1 PD7
//semaphore 2
#define R2 PD2
#define Y2 PD3
#define G2 PD4
//semaphore 3
#define R3 PB0
#define Y3 PB1
#define G3 PB2

unsigned short int count;
enum state {GREEN, YELLOW, RED, INTER};
state S1 = INTER, S2 = INTER, S3 = INTER;
unsigned short int old, flg;


void setup() 
{
  //set led and button pins as optput
  DDRD|=(1<<R1) | (1<<Y1) | (1<<G1) |(1<<R2) | (1<<Y2) | (1<<G2);
  DDRB|=(1<<R3) | (1<<Y3) | (1<<G3);
  DDRC&=~(1<<button);

  cli(); //disable interrupts

  timer1_init();

  sei(); //enable interrupts
  
}

void loop() 
{
  if((PINC&(1<<button)) && !old)
  {
    flg^=1;
  }
  old = PINC&(1<<button);
  
  if(!flg)
  {
    switch_semaphores();
  } 
}

void timer1_init()
{
 //set timer1 interrupt at 1Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 15624;// = (16*10^6) / (1*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect){  
   count++;

   if(flg)
   {
    semaphores_off();
   }
   
   switch(count)
   {
     case 1: S2 = RED;
             S1 = S3 = GREEN;
             break;

     case 8: S1 = S3 = YELLOW;
             break;

     case 11: S1 = S3 = RED;
              S2 = GREEN;
              break;

     case 18: S2 = YELLOW;
              break;

     case 21: count = 0;    
   }
}

void switch_semaphores()
{
  switch(S1)
  {
    case GREEN: PORTD|=(1<<G1);
                PORTD&=~((1<<Y1) | (1<<R1));
                break;

    case YELLOW: PORTD|=(1<<Y1);
                 PORTD&=~((1<<G1) | (1<<R1));
                 break;

    case RED: PORTD|=(1<<R1);
               PORTD&=~((1<<G1) | (1<<Y1));
               break;
  }

  switch(S2)
  {
    case GREEN: PORTD|=(1<<G2);
                PORTD&=~((1<<Y2) | (1<<R2));
                break;

    case YELLOW: PORTD|=(1<<Y2);
                 PORTD&=~((1<<G2) | (1<<R2));
                 break;

    case RED: PORTD|=(1<<R2);
               PORTD&=~((1<<G2) | (1<<Y2));
               break;
  }

  switch(S3)
  {
    case GREEN: PORTB|=(1<<G3);
                PORTB&=~((1<<Y3) | (1<<R3));
                break;

    case YELLOW: PORTB|=(1<<Y3);
                 PORTB&=~((1<<G3) | (1<<R3));
                 break;

    case RED : PORTB|=(1<<R3);
               PORTB&=~((1<<G3) | (1<<Y3));
               break;
  }
}

void semaphores_off()
{
  PORTD&=~((1<<G1) | (1<<R1) | (1<<G2) | (1<<R2));
  PORTB&=~((1<<G3) | (1<<R3));

  PORTD^=(1<<Y1) | (1<<Y2);
  PORTB^=(1<<Y3);
}
