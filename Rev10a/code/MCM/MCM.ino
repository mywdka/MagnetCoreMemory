#define  PULSE  6 
#define  DELAY  10

#define SYNC_PIN 0x07

#define EN_ROW1  0x05
#define EN_ROW2  0x04
#define EN_COL1  0x03
#define EN_COL2  0x02

#define R1  0x06
#define R1c 0x05
#define R2  0x03
#define R2c 0x04

#define C1  0x03
#define C1c 0x02
#define C2  0x00
#define C2c 0x01

#define  EN    0x04
#define  SYNC  0x80
#define  WR    0x28
#define  RD    0x50

#define READ  0x00
#define WRITE 0x01

void setup() {
  Serial.begin(19200);
  DDRC = _BV(PC2) | _BV(PC3) | _BV(PC4) | _BV(PC5); /* enable outputs */
  DDRD = _BV(PD3) | _BV(PD4) | _BV(PD5) | _BV(PD6) | _BV(PD7); /* row driver outputs */
  DDRB = _BV(PB0) | _BV(PB1) | _BV(PB2) | _BV(PB3); /* column driver outputs */
  DDRB = DDRB | 0xFE;  // D8 is input
}

void sDelay (byte cnt) {
  for (byte ii = 0; ii < cnt; ii++) {
    __asm__ ("nop\n\t");    // NOP delays 62.5 ns
    __asm__ ("nop\n\t");
  }
}

byte bead (byte row, byte col, byte dir)
{
  byte enable = 0x00;
  
  if (row == 0) {
    row = (dir == READ) ? _BV(R1) : _BV(R1c);
    enable = _BV(EN_ROW1);
  } else if (row == 1) {
    row = (dir == READ) ? _BV(R2c) : _BV(R2);
    enable = _BV(EN_ROW2);
  }
  
  if (col == 0) {
    col = (dir == READ) ? _BV(C1) : _BV(C1c);
    enable |= _BV(EN_COL1);
  } else if (col == 1) {
    col = (dir == READ) ? _BV(C2c) : _BV(C2);
    enable |= _BV(EN_COL2);
  }
  
  PORTD = _BV(SYNC_PIN);
  PORTD |= row;
  PORTB = col;
  PORTC = enable;
  
  __asm__ ("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
  __asm__ ("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
  __asm__ ("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"); 
  __asm__ ("nop\n\t");
  // sample sense amp output after ~1,200 ns delay
  byte smpl = PINB & 0x01;
  __asm__ ("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");  // 
  __asm__ ("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");  // 
  
  PORTC = 0x00;
  PORTD = 0x00;
  PORTB = 0x00;

  sDelay(DELAY);

  return smpl;
}

byte pulse (byte lines) {
  PORTD = (lines + EN);
  __asm__ ("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
  __asm__ ("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
  __asm__ ("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"); 
  __asm__ ("nop\n\t");
  // sample sense amp output after ~1,200 ns delay
  byte smpl = PINB & 0x01;
  __asm__ ("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");  // 
  __asm__ ("nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");  // 
  PORTD = 0x00;
  sDelay(DELAY);
  return smpl;
}

void loop() {
  
  bead(0, 0, WRITE);
  bead(1, 1, WRITE);
  bead(1, 1, READ);
  bead(1, 1, READ);
  bead(0, 0, READ);
  bead(0, 0, READ);

  
  delay (1000);
  /*
  byte test = 0;
  Serial.print("RD: ");
  test = pulse(WR + SYNC);
  Serial.print(test, DEC);
  test = pulse(RD);
  Serial.print(test, DEC);
  test = pulse(RD);
  Serial.println(test, DEC);
  delay(1000);
  */
}
