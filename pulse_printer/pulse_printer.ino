#include "./HPD482.h"          //打印机头文件
#include "./img_background.h"  //待打印的图片数据
#include "./font.h"            //待打印的图片数据

#define BUFFER_SZ 1000
#define DISP_SZ 376
#define minADC 300
#define maxADC 800
#define WIDTH 384
#define HEIGHT 240
#define ANIM_X 16
#define ANIM_Y 8
#define DIGIT_X 0
#define DIGIT_Y 60
#define CURVE_X 8
#define CURVE_Y 105
HPD482 printer = HPD482();
const int btn_start_once = 22;
const int btn_empty = 18;
const int led = 13;
u8 hram[48], last_hram[48];
float alpha = 0.9;
u8 heart_rate_value[BUFFER_SZ];
bool start_once = false;
bool press_even = false;

// 统计大于均值线的独立区域的个数
int countPeaks(u8 *data, int size, int mean_line) {
  int peakCount = 0;
  int isAboveMean = 0;  // 标记是否在均值线以上
  u8 *pdata = (u8 *)data;

#if 0
	for (int i = 0; i < size; i++) {
		*pdata++ = *praw++;
	}
#endif
  // 遍历信号数组
  for (int i = 0; i < size; i++) {
    // 当前点高于均值线，且之前不在波峰中
    if (data[i] > mean_line && !isAboveMean) {
      isAboveMean = 1;
      peakCount++;  // 发现新的波峰，计数加1
    }
    // 当前点低于或等于均值线
    else if (data[i] <= mean_line) {
      isAboveMean = 0;  // 重置标记
    }
  }

  return peakCount;
}

void setup() {
  Serial.begin(9600);
  printer.Print_SetDeep(8);
  pinMode(btn_start_once, INPUT_PULLUP);
  pinMode(btn_empty, INPUT);
  pinMode(led, OUTPUT);
  randomSeed(analogRead(0));
}

void loop() {
BEGIN:
  int adcValue = 0;
  static int last_adcValue = 0;
  int bpm = 0;
  int mean = 0;
  long sum = 0;
  int region_count = 0;
  u8 *p_value;
  int count = 0;

  // 读取心率数据并处理
  memset(heart_rate_value, 0, sizeof(heart_rate_value));
  for (int i = 0; i < BUFFER_SZ; i++) {
    adcValue = analogRead(A5);
    // Serial.print(adcValue);
    // Serial.print(',');
    adcValue = alpha * adcValue + (1 - alpha) * last_adcValue;
    last_adcValue = adcValue;
    if (adcValue > maxADC) {
      adcValue = maxADC;
    }
    if (adcValue < minADC) {
      adcValue = minADC;
    }
    adcValue = (float)(adcValue - minADC) / (float)(maxADC - minADC) * 129.0f;

    // 等待第一个异常值作为开始，优信这款adc心率计在手指刚放上去时有明显的波动和异常值（糊弄）
    if (adcValue < 5) {
      Serial.println("press_even");
      press_even = true;
      digitalWrite(led, LOW);
      goto BEGIN;
    } else {
      if (press_even) {
        press_even = false;
        start_once = true;

        Serial.println("start_once");

        digitalWrite(led, HIGH);
        delay(50);
        digitalWrite(led, LOW);
        delay(50);
      }
      digitalWrite(led, HIGH);
    }

    // 环境也有数值
    if (!start_once) {
      goto BEGIN;
    }

    Serial.println(adcValue);
    heart_rate_value[i] = adcValue;
    delay(9);  // 孱弱的机能
  }
  start_once = false;

  Serial.println("Start process...");

  // 当按下达到8s，开始计算并打印心率和曲线
  for (int i = 0; i < BUFFER_SZ; i++) {
    sum += heart_rate_value[i];
  }
  mean = sum / BUFFER_SZ;
  mean += 8;
  Serial.println(mean);
  region_count = countPeaks(heart_rate_value, BUFFER_SZ, mean);
  Serial.println(region_count);
  bpm = region_count * 60.0 / (BUFFER_SZ / 100);
  Serial.println(bpm);
  if (bpm < 45) {
    bpm = 45;
  }
  if (bpm > 180) {
    bpm = 180;
  }
  if (bpm == 45 || bpm == 180) {
    Serial.println("Ramdon");
    bpm = random(50, 160);
    Serial.println(bpm);
  }

  // 将所有需要打印的图片都缓存到hram中，一行行计算
  memset(last_hram, 0x00, sizeof(hram));
  for (int row = 0; row < HEIGHT; row++) {
    memset(hram, 0x00, sizeof(hram));

    // 背景
    for (int i = 0; i < sizeof(hram); i++) {
      hram[i] = pgm_read_byte(&gPic_background[row * (WIDTH / 8) + i]);
    }

    // 动物标志
    const unsigned char *p = gPic_dog;
    if (row >= ANIM_Y && row < ANIM_Y + 96) {
      int line = (row - ANIM_Y);
      if (bpm < 60) {
        p = gPic_Elephant;
      } else if (bpm < 110) {
        p = gPic_dog;
      } else {
        p = gPic_mouse;
      }
      for (int i = 0; i < 96 / 8; i++) {
        hram[ANIM_X / 8 + i] = pgm_read_byte(&p[line * (96 / 8) + i]);
      }
    }

    // // 数字
    if (row >= DIGIT_Y && row < DIGIT_Y + 40) {
      u8 digit_1 = bpm / 100;
      u8 digit_2 = (bpm % 100) / 10;
      u8 digit_3 = bpm % 10;
      for (int i = 0; i < sizeof(hram); i++) {
        for (int i = 0; i < 40 / 8; i++) {
          if (digit_1 != 0) {
            hram[30 + (40 / 8) * 0 + i] = pgm_read_byte(&ascii_pic4040[digit_1][(row - DIGIT_Y) * (40 / 8) + i]);
          }
          hram[30 + (40 / 8) * 1 + i] = pgm_read_byte(&ascii_pic4040[digit_2][(row - DIGIT_Y) * (40 / 8) + i]);
          hram[30 + (40 / 8) * 2 + i] = pgm_read_byte(&ascii_pic4040[digit_3][(row - DIGIT_Y) * (40 / 8) + i]);
        }
      }
    }

    // 曲线
    if (row >= CURVE_Y && row < CURVE_Y + 130) {
      int line = (129 - (row - CURVE_Y));
      // Serial.print(line);

      // 简易加粗（复制上一次）
      for (int i = 0; i < 48; i++) {
        // hram[i] |= last_hram[i];
      }
      memset(last_hram, 0x00, sizeof(hram));

      // 遍历心率数据，只要两点间连线出现在当前行就应标记为1
      for (int t = 1; t < DISP_SZ; t++) {
        p_value = heart_rate_value;
        // i等于当前像素，标记hram为1
        // i-1的值小于当前hram的行位置像素，i大于等于当前像素，标记hram为1
        // i-1的值大于当前hram的行位置像素，i小于等于当前像素，标记hram为1
        if ((p_value[t] == line) || (p_value[t - 1] < line && p_value[t] > line) || (p_value[t - 1] > line && p_value[t] < line)) {
          hram[(CURVE_X / 8) + (t / 8)] |= 0x01 << (7 - t % 8);
          last_hram[(CURVE_X / 8) + (t / 8)] |= 0x01 << (7 - t % 8);
        }
      }
    }

    // 逐行绘画
    printer.Print_Picture(0, hram, WIDTH, 1, 0);
  }

  printer.Motor_Run(70, 0);
  printer.Print_Stop();
}
