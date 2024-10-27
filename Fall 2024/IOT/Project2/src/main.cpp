#include <Arduino.h>

#define RED_PIN 32
#define YELLOW_PIN 33
#define GREEN_PIN 25

#define BUZZER_PIN 26
#define BUTTON_PIN 27

// Hard coded enumerator
#define RED_PIN 32
#define YELLOW_PIN 33
#define GREEN_PIN 25
#define BUZZER_PIN 26
#define BUTTON_PIN 27

// 定义状态
#define RED_STATE 0
#define RED_YELLOW_STATE 1
#define YELLOW_RED_STATE 2
#define GREEN_STATE 3
#define RED_MILLIS 10000
#define YELLOW_MILLIS 2000
#define GREEN_MIN_MILLIS 5000
#define BUZZER_GREEN_ON 500
#define BUZZER_GREEN_OFF 1500
#define BUZZER_RED_ON 250
#define BUZZER_RED_OFF 250

int tl_state; // 交通灯状态
unsigned long tl_timer; // 交通灯定时器
unsigned long buzzer_timer; // 蜂鸣器定时器
bool button_pressed = false; // 按钮按下标志
unsigned long button_press_time = 0; // 按钮按下时的时间
bool waiting_for_green_min = false; // 等待绿灯至少亮5秒

void IRAM_ATTR handleButtonPress() {
    // 如果按钮尚未按下并且当前状态为绿灯，则设置按钮按下标志
    button_pressed = true; // 设置按钮按下标志
    button_press_time = millis(); // 记录按钮按下时的时间
}

void setup() {
  Serial.begin(9600);

  // 配置引脚为输出
  pinMode(RED_PIN, OUTPUT);
  pinMode(YELLOW_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  // 为按钮按下绑定中断处理程序
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), handleButtonPress, RISING);

  // 初始化状态和定时器
  tl_state = RED_STATE;
  tl_timer = millis() + RED_MILLIS;
  buzzer_timer = millis();
  waiting_for_green_min = false;
}

void loop() {
  // 交通灯状态机
  switch (tl_state) {
    case RED_STATE:
      // 打开红灯
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, LOW);

      if (millis() > tl_timer) {
        // 关闭红灯，切换到红黄灯状态
        digitalWrite(RED_PIN, LOW);
        tl_state = RED_YELLOW_STATE;
        tl_timer = millis() + YELLOW_MILLIS;
      }
      break;

    case RED_YELLOW_STATE:
      // 打开红黄灯
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(YELLOW_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);

      if (millis() > tl_timer) {
        // 关闭红黄灯，切换到绿灯状态
        digitalWrite(RED_PIN, LOW);
        digitalWrite(YELLOW_PIN, LOW);
        tl_state = GREEN_STATE;
        waiting_for_green_min = false; // 重置最少绿灯时间的标志
        button_pressed = false; // 重置按钮标志
      }
      break;

    case YELLOW_RED_STATE:
      // 打开黄灯
      digitalWrite(RED_PIN, HIGH);
      digitalWrite(YELLOW_PIN, HIGH);
      digitalWrite(GREEN_PIN, LOW);

      if (millis() > tl_timer) {
        // 关闭黄灯，切换回红灯状态
        digitalWrite(YELLOW_PIN, LOW);
        tl_state = RED_STATE;
        tl_timer = millis() + RED_MILLIS;
      }
      break;

    case GREEN_STATE:
      // 打开绿灯
      digitalWrite(RED_PIN, LOW);
      digitalWrite(YELLOW_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);

      Serial.print("button_pressed");
      Serial.println(button_pressed);
      Serial.print("waiting_for_green_min");
      Serial.println(waiting_for_green_min);

      // 判断是否按下按钮
      if (button_pressed) {
        if (!waiting_for_green_min) {
          // 如果还未等待绿灯至少亮5秒，则检查当前是否超过5秒
          if (millis() - button_press_time >= GREEN_MIN_MILLIS) {
            waiting_for_green_min = true;
            tl_state = YELLOW_RED_STATE; // 切换到黄灯状态
            tl_timer = millis() + YELLOW_MILLIS;
            digitalWrite(GREEN_PIN, LOW); // 关闭绿灯
          }
        }
      }
      break;
  }

  // 蜂鸣器状态机
  switch (tl_state) {
    case GREEN_STATE:
      if (millis() > buzzer_timer) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(BUZZER_GREEN_ON); // 蜂鸣器响500毫秒
        digitalWrite(BUZZER_PIN, LOW);
        buzzer_timer = millis() + BUZZER_GREEN_OFF; // 停1500毫秒
      }
      break;

    case RED_STATE:
      if (millis() > buzzer_timer) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(BUZZER_RED_ON); // 蜂鸣器响250毫秒
        digitalWrite(BUZZER_PIN, LOW);
        buzzer_timer = millis() + BUZZER_RED_OFF; // 停250毫秒
      }
      break;

    default:
      digitalWrite(BUZZER_PIN, LOW); // 确保其他状态下蜂鸣器关闭
      break;
  }
}
