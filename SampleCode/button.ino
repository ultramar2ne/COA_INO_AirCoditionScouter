int sw = 10;       // 스위치(SW) 핀 설정
int led = 6;       // LED 핀 설정
 
int state = LOW;      // LED 상태
int reading;          // SW 상태
int previous = LOW;   // SW 이전 상태
 
long time = 0;        // LED가 ON/OFF 토글된 마지막 시간
long debounce = 100;  // Debounce 타임 설정
 
void setup()
{
  pinMode(sw, INPUT_PULLUP); // SW 를 설정, 아두이노 풀업저항 사용
  pinMode(led, OUTPUT);      // LED 설정
}
 
void loop()
{
  reading = digitalRead(sw);  // SW 상태 읽음
 
  //SW 가 눌려졌고 스위치 토글 눌림 경과시간이 Debounce 시간보다 크면 실행
  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    if (state == HIGH)    // LED 가 HIGH 면 LOW 로 바꿔준다.
      state = LOW;
    else                  // LED 가 LOW 면 HIGH 로 바꿔준다.
      state = HIGH;
 
    time = millis();
  }
 
  digitalWrite(led, state);
 
  previous = reading;
}
