#include <Arduino.h>

#define stepPin1 22
#define dirPin1 23
#define enPin1 24
#define stepPin2 25
#define dirPin2 26
#define enPin2 27
#define stepPin3 28
#define dirPin3 29
#define enPin3 30
#define stepPin4 31
#define dirPin4 32
#define enPin4 33
#define CW1 1
#define CCW1 0
#define CW2 1
#define CCW2 0
#define CW3 1
#define CCW3 0
#define CW4 1
#define CCW4 0
#include <Encoder.h>
String packdata = "";
const int phaseA1 = 2;
const int phaseB1 = 3;
const int phaseA2 = 20;
const int phaseB2 = 21;
int cp_state = 0;
int cp_off = 11;
int cp_on = 12;
int cp = 10;
int last_cp_state = 0;
Encoder enc1(phaseA1, phaseB1);
Encoder enc2(phaseA2, phaseB2);
float start_move2 = 0;
float start_move3 = 0;
int SenserCount = 0;
int MoveCount = 0;
bool started = false;
int variable_state = 0;
int start_position = 0;
const int stop = 48;
const int start = 49;
int SenserMax = 0;
int MoveMax = 0;
int distance2 = 0;
123
int distance3 = 0;
bool finished = false;
float circle1 = 29.635;
float circle2 = 32.901;
const int stepsPerRevolution1 = 400;
const int stepsPerRevolution2 = 400;
const int stepsPerRevolution3 = 400;
const int stepsPerRevolution4 = 400;
const int limit_switch1 = 34;
const int limit_switch2 = 35;
const int limit_switch3 = 36;
const int limit_switch4 = 37;
const int limit_switch5 = 38;
const int limit_switch6 = 39;
const int limit_switch7 = 40;
const int limit_switch8 = 41;
const int MAX = 3000;
unsigned long pre_time;
void sendDone() {
Serial.println("done");
Serial.flush();
delay(50);
}
void Move_x(int step) {
for (int x = 0; x < step; x++) {
if (digitalRead(limit_switch3) == LOW && digitalRead(dirPin2) == CCW2)
break;
if (digitalRead(limit_switch4) == LOW && digitalRead(dirPin2) == CW2)
break;
digitalWrite(stepPin2, HIGH);
delay(2);
digitalWrite(stepPin2, LOW);
delay(2);
}
delay(500);
sendDone();
}
void Move_y(int step) {
for (int y = 0; y < step; y++) {
if (digitalRead(limit_switch1) == LOW && digitalRead(dirPin1) == CCW1)
break;
if (digitalRead(limit_switch2) == LOW && digitalRead(dirPin1) == CW1)
break;
digitalWrite(stepPin1, HIGH);
delay(10);
digitalWrite(stepPin1, LOW);
delay(10);
}
delay(500);
124
sendDone();
}
void Move_X(int step) {
for (int x = 0; x < step; x++) {
if (digitalRead(limit_switch7) == LOW && digitalRead(dirPin4) == CCW4)
break;
if (digitalRead(limit_switch8) == LOW && digitalRead(dirPin4) == CW4)
break;
digitalWrite(stepPin4, HIGH);
delay(2);
digitalWrite(stepPin4, LOW);
delay(2);
}
delay(500);
sendDone();
}
void Move_Y(int step) {
for (int y = 0; y < step; y++) {
if (digitalRead(limit_switch5) == LOW && digitalRead(dirPin3) == CCW3)
break;
if (digitalRead(limit_switch6) == LOW && digitalRead(dirPin3) == CW3)
break;
digitalWrite(stepPin3, HIGH);
delay(10);
digitalWrite(stepPin3, LOW);
delay(10);
}
delay(500);
sendDone();
}
void go_home() {
digitalWrite(dirPin2, CCW2);
Move_x(MAX);
digitalWrite(dirPin1, CCW1);
Move_y(MAX);
digitalWrite(dirPin4, CCW4);
Move_X(MAX);
digitalWrite(dirPin3, CCW3);
Move_Y(MAX);
enc1.write(0);
enc2.write(0);
sendDone();
}
void stopMotor1() {
digitalWrite(stepPin1, LOW);
}
void stopMotor2() {
digitalWrite(stepPin3, LOW);
125
delay(3000);
start_position = 1;
}
bool waitForToken(const String& target, unsigned long timeoutMs) {
String buf = "";
unsigned long t0 = millis();
while (millis() - t0 < timeoutMs) {
while (Serial1.available()) {
char c = Serial1.read();
if (c == '#') {
buf += c;
buf.trim();
if (buf == target) return true;
buf = "";
} else {
buf += c;
}
}
return false;
}
return false;
}
void scan1() {
digitalWrite(dirPin1, CW1);
String buf = "";
unsigned long t0 = millis();
while (true) {
while (Serial1.available()) {
char c = Serial1.read();
if (c == '#') {
buf += c;
buf.trim();
if (buf == "@pull#") {
stopMotor1();
return;
}
buf = "";
} else {
buf += c;
}
}
digitalWrite(stepPin1, HIGH);
delay(10);
digitalWrite(stepPin1, LOW);
delay(10);
if (millis() - t0 > 1000000) { // 10s
stopMotor1();
Serial.println("scan1 TIMEOUT");
return;
126
}
}
}
void scan2() {
digitalWrite(dirPin3, CW3);
String buf = "";
unsigned long t0 = millis();
while (true) {
while (Serial1.available()) {
char c = Serial1.read();
if (c == '#') {
buf += c;
buf.trim();
if (buf == "@press#") {
stopMotor2();
return;
}
buf = "";
} else {
buf += c;
}
}
digitalWrite(stepPin3, HIGH);
delay(10);
digitalWrite(stepPin3, LOW);
delay(10);
if (millis() - t0 > 1000000) {
stopMotor2();
Serial.println("scan2 TIMEOUT");
return;
}
}
}
void handleCP() {
if (digitalRead(start) == LOW) cp_state = 1;
if (digitalRead(stop) == LOW) cp_state = 2;
if (cp_state != last_cp_state) {
last_cp_state = cp_state;
if (cp_state == 1) {
digitalWrite(cp_on, HIGH);
digitalWrite(cp, HIGH);
digitalWrite(cp_off, LOW);
} else {
digitalWrite(cp_off, HIGH);
digitalWrite(cp_on, LOW);
digitalWrite(cp, LOW);
}
127
}
}
void parseA2(String s) {
s.replace("@A2,", "");
s.replace("#", "");
int p1 = s.indexOf(',');
int p2 = s.indexOf(',', p1 + 1);
int p3 = s.indexOf(',', p2 + 1);
int p4 = s.indexOf(',', p3 + 1);
int p5 = s.indexOf(',', p4 + 1);
int p6 = s.indexOf(',', p5 + 1);
int v2 = s.substring(0, p1).toInt();
int v3 = s.substring(p1 + 1, p2).toInt();
int v4 = s.substring(p2 + 1, p3).toInt();
float v5 = s.substring(p3 + 1, p4).toFloat();
float v6 = s.substring(p4 + 1, p5).toFloat();
float v7 = s.substring(p5 + 1, p6).toFloat();
float v8 = s.substring(p6 + 1).toFloat();
SenserMax = v2;
MoveMax = SenserMax - 1 ;
distance2 = v3*50;
distance3 = v4*50;
circle1 = v5/2;
circle2 = v6/2;
start_move2 = v7*50;
start_move3 = v8*50;
}
void setup() {
Serial.begin(9600);
Serial1.begin(9600);
pinMode(stepPin1, OUTPUT);
pinMode(dirPin1, OUTPUT);
pinMode(enPin1, OUTPUT);
pinMode(stepPin2, OUTPUT);
pinMode(dirPin2, OUTPUT);
pinMode(enPin2, OUTPUT);
pinMode(stepPin3, OUTPUT);
pinMode(dirPin3, OUTPUT);
pinMode(enPin3, OUTPUT);
pinMode(stepPin4, OUTPUT);
pinMode(dirPin4, OUTPUT);
pinMode(enPin4, OUTPUT);
pinMode(start, OUTPUT);
pinMode(stop, OUTPUT);
pinMode(cp_on, OUTPUT);
pinMode(cp_off, OUTPUT);
pinMode(cp, OUTPUT);
digitalWrite(enPin1, LOW);
digitalWrite(enPin2, LOW);
128
digitalWrite
(enPin3, LOW
)
;
digitalWrite
(enPin4, LOW
)
;
pre_time = millis()
;
pinMode
(start, INPUT_PULLUP
)
;
pinMode
(stop, INPUT_PULLUP
)
;
pinMode
(limit_switch1, INPUT_PULLUP
)
;
pinMode
(limit_switch2, INPUT_PULLUP
)
;
pinMode
(limit_switch3, INPUT_PULLUP
)
;
pinMode
(limit_switch4, INPUT_PULLUP
)
;
pinMode
(limit_switch5, INPUT_PULLUP
)
;
pinMode
(limit_switch6, INPUT_PULLUP
)
;
pinMode
(limit_switch7, INPUT_PULLUP
)
;
pinMode
(limit_switch8, INPUT_PULLUP
)
;
}
void loop()
{
handleCP()
;
if
(Serial
.available())
{
String line = Serial
.readStringUntil
(
'
\
n
'
)
;
line
.trim()
;
if
(line == "h"
)
{
go_home()
;
return
;
}
if
(line == "n"
)
{
digitalWrite
(dirPin2, CW2
)
;
float mm = 30.085
- circle1;
float steps = mm * 50
;
Move_x
(steps
)
;
delay
(1000
)
;
while
(Serial1
.available()) Serial1
.read()
;
Serial1
.println
("@LEFT#"
);
scan1()
;
digitalWrite
(dirPin4, CW4
)
;
float mm2 = 32.881
- circle2;
float steps2 = mm2 * 50
;
Move_X
(steps2
)
;
delay
(1000
)
;
while
(Serial1
.available()) Serial1
.read()
;
Serial1
.println
("@RIGHT#"
);
scan2()
;
return
;
}
if
(line
.startsWith
("@A2"))
{
parseA2
(line
)
;
return
;
}
}
if
(!started && variable_state ==
1
)
{
delay
(50
)
;
129
if (variable_state == 1) {
started = true;
Serial.println("Đã nhấn nút. Bắt đầu quá trình...");
}
}
if (started && variable_state == 0) {
delay(50);
if (variable_state == 0) {
started = false;
}
}
if(start_position == 1){
digitalWrite(dirPin1, CW1);
Move_y(start_move2);
delay(1000);
digitalWrite(dirPin3, CW3);
Move_Y(start_move3);
delay(2000);
variable_state = 1;
start_position = 0;
}
if (started && (SenserCount < SenserMax || MoveCount < MoveMax)) {
if (SenserCount < SenserMax) {
Serial1.println("@SAMPLE#");
Serial.print("Yêu cầu khối cảm biến chạy lần thứ: ");
Serial.println(SenserCount + 1);
while (true) {
if (Serial1.available()) {
String resp = Serial1.readStringUntil('#');
resp.trim();
resp += "#";
if (resp == "@SAMPLE_DONE#") {
break;
}
}
}
SenserCount++;
}
delay(500);
if (MoveCount < MoveMax) {
Serial.print("khối chuyển động trục y chạy lần thứ: ");
Serial.println(MoveCount + 1);
digitalWrite(dirPin1, CW1);
Move_y(distance2);
delay(1000);
Serial.print("khối chuyển động trục Y chạy lần thứ: ");
Serial.println(MoveCount + 1);
digitalWrite(dirPin3, CW3);
Move_Y(distance3);
130
delay(1000);
MoveCount++;
}
delay(500);
}
if (!finished && started && SenserCount >= SenserMax && MoveCount >=
MoveMax ) {
Serial.println("Đã hoàn tất quá trình. Kết thúc.");
finished = true;
}
if (finished) {
return;
}
}