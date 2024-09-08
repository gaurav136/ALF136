/*

  This code is to be used with the Robojunkies Explorer Kit. More details here- https://robojunkies.com/product/explorer-robotics-kit/
  This is a Maze Solving Program like the competitions held at IIT Techfest- Meshmerize.
  Requires a black background and white line maze.

  ==>This program was tested and refined for a maze containing 90 degrees only!!!<==

  Developed by Team Robojunkies.

   Motor Right Dir- 7
   Motor Right PWM- 9
   Motor Left Dir- 8
   Motor Left PWM- 10
   RGB- 6,5,3

   Line sensor on A0,A1,A2,A3,A4
   A0-left & A4 - right

*/
const int startButton = 11;

bool l = 0;
bool r = 0;
bool s = 0;
bool u = 0;
int e = 0;
int paths = 0;

bool endFound = 0;

int blackValue = 900;
int whiteValue = 100;
int threshold = 660;
//int threshold = (blackValue + whiteValue) * 0.5;
int FT = 250;
int P, D, I, previousError, PIDvalue, error;
int lsp = 100;
int rsp = 100;
int lfspeed = 80;
int turnspeed;
float Kp = 0.04;
float Kd = 0.05;
float Ki = 0 ;

String str;

void setup() {
  Serial.begin(9600);
  pinMode(startButton, INPUT_PULLUP);
  pinMode (7, OUTPUT);
  pinMode (8, OUTPUT);
  pinMode (3, OUTPUT); //blue
  pinMode (5, OUTPUT); //green
  pinMode (6, OUTPUT); //red
  //  lfspeed = 50000 / analogRead(7); //arbitrary conversion to convert analogRead to speed. Need to check if this works for all voltage levels
  turnspeed = lfspeed * 0.6;
}

void loop() {
  while (digitalRead(startButton) == 0)
  { //Do nothing while waiting for button press
  }
  delay(1000);

  while (endFound == 0)
  {
    linefollow();
    checknode();

    botstop();
    delay(100);

    reposition ();
  }

  for (int x = 0; x < 4; x++)
  {
    str.replace("LULUS", "U");
    str.replace("LUSUL", "U");
    str.replace("LUL", "S");
    str.replace("SUL", "R");
    str.replace("LUS", "R");
    str.replace("RUL", "U");
  }
  int endpos = str.indexOf('E');

  while (digitalRead(startButton) == 0)
  { //Do nothing while waiting for button press
  }
  delay(1000);

  for (int i = 0; i <= endpos; i++)
  {
    char node = str.charAt(i);
    paths = 0;
    while (paths < 2)
    {
      linefollow();
      checknode();
      if (paths == 1)
      {
        reposition();
      }
    }
    switch (node)
    {
      case 'L':
        botstop();
        delay(50);
        botleft();
        break;

      case 'S':
        break;

      case 'R':
        botstop();
        delay(50);
        botright();
        break;

      case 'E':
        for (int i = 0; i < 50; i++)
        {
          botinchforward ();
        }
        red();
        botstop();
        delay(1000);
        break;
    }//_________end of switch
  }//_________end of for loop

}

void showSensorData()
{
  Serial.print("Sensor 0-  ");
  Serial.print(analogRead(0));
  Serial.print("  Sensor 1-  ");
  Serial.print(analogRead(1));
  Serial.print("  Sensor 2-  ");
  Serial.print(analogRead(2));
  Serial.print("  Sensor 3-  ");
  Serial.print(analogRead(3));
  Serial.print("  Sensor 4-  ");
  Serial.println(analogRead(4));
}

void linefollow()
{ //green () ;
  paths = 0;
  while ((analogRead(0) > threshold ) && (analogRead(4) > threshold ) && (analogRead(2) < threshold))
  {
    PID();
  }
  lightsoff();
}




void PID()
{
  int error = analogRead(1) - analogRead(3);

  P = error;
  I = I + error;
  D = error - previousError;

  PIDvalue = (Kp * P) + (Ki * I) + (Kd * D);
  previousError = error;

  lsp = lfspeed - PIDvalue;
  rsp = lfspeed + PIDvalue;

  if (lsp > 200) {
    lsp = 200;
  }
  if (lsp < 0) {
    lsp = 0;
  }
  if (rsp > 200) {
    rsp = 200;
  }
  if (rsp < 0) {
    rsp = 0;
  }

  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  analogWrite(9, lsp);
  analogWrite(10, rsp);
}


void checknode ()
{
  yellow();
  l = 0;
  r = 0;
  s = 0;
  u = 0;
  e = 0;
  paths = 0;

  // checks whethere bot is on node and the number of exits possible


  if (analogRead(4) < threshold) r = 1;
  if (analogRead(0) < threshold) l = 1;
  if ((analogRead(0) > threshold && (analogRead(4) > threshold) && (analogRead(2) > threshold))) {
    u = 1;
  }
  if ((analogRead(2) < threshold) && (analogRead(3) < threshold) && (analogRead(4) < threshold)) {
    e = 1;
  }

  if (u == 0)
  {
    for (int i = 0; i < FT; i++)
    {
      magenta();
      //botinchforward ();
      PID();
      if (analogRead (4) < threshold) r = 1;
      if (analogRead (0) < threshold) l = 1;
    }

    for (int i = 0; i < FT; i++)
    { cyan();
      //botinchforward ();
      PID();
      if (analogRead (2) < threshold) s = 1;
      if ((e == 1) && (analogRead(3) < threshold) && (analogRead(4) < threshold) && (analogRead(2) < threshold)) e = 2;
    }
  }
  if (u == 1)
  {
    for (int i = 0; i < 50; i++)
    {
      botinchforward ();
    }
  }

  paths = l + s + r;

}





void reposition()
{
  blue();
  if (e == 2)
  {
    str += 'E';
    endFound = 1;
    red();
    botstop();
    delay(2000);
  }
  else if (l == 1)
  {
    if (paths > 1) str += 'L';
    botleft(); //take left
  }

  else if (s == 1)
  {
    if (paths > 1) str += 'S';
  }
  else if (r == 1)
  {
    if (paths > 1) str += 'R';
    botright(); //take right
  }

  else if (u == 1)
  {
    magenta();
    str += 'U';
    botuturn(); //take left
  }
  lightsoff();

}



void botleft ()
{
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
  analogWrite(9, lfspeed);
  analogWrite(10, lfspeed);
  delay(200);
  while (analogRead(2) > threshold)
  {
    digitalWrite(7, HIGH);
    digitalWrite(8, LOW);
    analogWrite(9, lfspeed);
    analogWrite(10, lfspeed );
  }
  analogWrite(9, 0);
  analogWrite(10, 0);
  delay(50);
}

void botright ()
{
  digitalWrite(7, LOW);
  digitalWrite(8, HIGH);
  analogWrite(9, lfspeed );
  analogWrite(10, lfspeed);
  delay(200);
  while (analogRead(2) > threshold)
  {
    digitalWrite(7, LOW);
    digitalWrite(8, HIGH);
    analogWrite(9, lfspeed );
    analogWrite(10, lfspeed);
  }
  analogWrite(9, 0);
  analogWrite(10, 0);
  delay(50);
}

void botstraight ()
{
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  analogWrite(9, lfspeed);
  analogWrite(10, lfspeed);
}

void botinchforward ()
{
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  analogWrite(9, turnspeed);
  analogWrite(10, turnspeed);
  delay(10);
}
void botstop ()
{
  digitalWrite(7, HIGH);
  digitalWrite(8, HIGH);
  analogWrite(9, 0);
  analogWrite(10, 0);
}
void botuturn ()
{
  digitalWrite(7, HIGH);
  digitalWrite(8, LOW);
  analogWrite(9, lfspeed * 0.8 );
  analogWrite(10, lfspeed);
  delay(400);
  while (analogRead(2) > threshold)
  {
    digitalWrite(7, HIGH);
    digitalWrite(8, LOW);
    analogWrite(9, lfspeed * 0.8);
    analogWrite(10, lfspeed);
  }
  analogWrite(9, 0);
  analogWrite(10, 0);
  delay(50);
}





void magenta ()

{
  analogWrite (3, 200); //BLUE
  analogWrite(5, 0); //Green      // MAGENTA
  analogWrite(6, 150); //red
}

void yellow ()
{
  analogWrite (3, 0); //BLUE
  analogWrite(5, 200); //Green      // Yellow
  analogWrite(6, 150); //red
}
void cyan()
{
  analogWrite (3, 200); //BLUE
  analogWrite(5, 200); //Green      // Cyan
  analogWrite(6, 0); //red
}
void green ()

{
  digitalWrite (3, LOW);
  digitalWrite(5, HIGH);          // GREEN
  digitalWrite(6, LOW);
}

void red ()
{
  digitalWrite (3, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, HIGH);           //RED
}
void blue ()
{
  digitalWrite (3, HIGH);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);           //BLUE
}
void lightsoff()
{
  digitalWrite (3, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
}








