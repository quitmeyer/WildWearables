/*
Ethosmock code
Embodied coding of animal actions
2015 example

*/

#define B1 4
#define B2 5
#define B3 6
#define B4 7
#define B5 8
#define B6 9
#define B7 10
#define B8 11
#define LED1 13

int bDown;
int pinVal = 0;
unsigned long time;


void setup() {
  Serial.begin(9600);
  delay(1000);
  bDown = -1;
  pinMode(B1, INPUT_PULLUP);
  digitalWrite(B1, HIGH);
  pinMode(B2, INPUT_PULLUP);
  pinMode(B3, INPUT_PULLUP);
  pinMode(B4, INPUT_PULLUP);
  pinMode(B5, INPUT_PULLUP);
  pinMode(B6, INPUT_PULLUP);
  pinMode(B7, INPUT_PULLUP);
  pinMode(B8, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);

}

void loop() {
  digitalWrite(4,HIGH);
  for (int i = 4; i <= 11; i++)
  {
    pinVal = digitalRead(i);
//    Serial.print(i);
//    Serial.print(", ");
//    Serial.println(pinVal);
    if (pinVal == LOW)
    {
      bDown = i;
      flashLED(bDown);
      writeText(bDown);
    }
    bDown = -1;
  }
}

void flashLED(int numToFlash)
{
  for (int j = 1; j <= numToFlash; j++)
  {
    digitalWrite(LED1, HIGH);
    delay(50);
    digitalWrite(LED1, LOW);
    delay(50);
  }
}

void writeText(int bPressed)
{
  time = millis();
  Serial.print(bPressed);
  Serial.print(", ");
  Serial.println(time);
}
