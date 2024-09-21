#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display settings
#define SCREEN_WIDTH 128      // OLED显示宽度，单位像素
#define SCREEN_HEIGHT 64      // OLED显示高度，单位像素
#define OLED_RESET    -1      // 重置引脚（-1表示不使用）
#define SCREEN_ADDRESS 0x3C   // OLED的I2C地址

// 初始化OLED显示
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// 可调节的眼睛参数
const int ref_eye_height = 20;           // 参考眼睛高度
const int ref_eye_width = 20;            // 参考眼睛宽度
const int ref_space_between_eye = 10;    // 两只眼睛之间的间距
const int ref_corner_radius = 5;         // 眼睛圆角的半径

// 当前眼睛状态
float left_eye_height = ref_eye_height;
float left_eye_width = ref_eye_width;
float left_eye_x = (SCREEN_WIDTH / 2) - (ref_eye_width / 2) - (ref_space_between_eye / 2);
float left_eye_y = SCREEN_HEIGHT / 2;
float right_eye_x = left_eye_x + ref_eye_width + ref_space_between_eye;
float right_eye_y = SCREEN_HEIGHT / 2;
float right_eye_height = ref_eye_height;
float right_eye_width = ref_eye_width;

// 动画延迟时间
const int short_delay = 1000;   // 1秒
const int medium_delay = 2000;  // 2秒
const int long_delay = 3000;    // 3秒

// 函数声明
void draw_eyes(bool update = true, float scale_left = 1.0, float scale_right = 1.0);
void center_eyes(bool update = true);
void blink(int speed = 5);
void sleep_eyes();
void wakeup_eyes();
void happy_eye();
void saccade(int direction_x, int direction_y, int total_steps, int amplitude_x, int amplitude_y);
float calculate_scale(float eye_x, float eye_width, int direction, int screen_width, float min_scale, float max_scale, float scale_start_distance);

// 设置函数
void setup() {
  // 初始化串口通信（可选）
  Serial.begin(115200);

  // 初始化OLED显示
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {  
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // 不继续，永久循环
  }

  // 清空显示缓冲区
  display.clearDisplay();

  // 显示启动信息
  display.setTextSize(1);                // 正常1:1像素比例
  display.setTextColor(SSD1306_WHITE);   // 以白色绘制文本
  display.setCursor(0, 0);               // 从左上角开始
  display.println(F("Github Link:"));
  display.println(F("  "));
  display.println(F("ideamark/desk-emoji"));
  display.println(F("  "));
  display.display();
  delay(5000); // 显示信息5秒

  // 初始化眼睛位置居中
  center_eyes();
}

// 主循环函数
void loop() {
  // 随机决定向左还是向右移动
  int direction = random(0, 2) == 0 ? 1 : -1;

  // 随机决定移动的幅度
  int amplitude_x = random(25, 35); // 随机在15到25像素之间
  int amplitude_y = random(10, 20);  // 随机在5到10像素之间

  // 随机决定总步数，使得动画多样化
  int total_steps = random(15, 25); // 随机在15到25步之间

  // 执行移动动画
  saccade(direction, 0, total_steps, amplitude_x, amplitude_y);
  delay(1000); // 保持1秒

  // 执行反方向移动
  saccade(-direction, 0, total_steps, amplitude_x, amplitude_y);
  delay(1000); // 保持1秒

  // 其他动画
  blink(5);
  delay(short_delay);

  happy_eye();
  delay(medium_delay);

  sleep_eyes();
  delay(medium_delay);

  wakeup_eyes();
  delay(short_delay);

  // 重复序列
}

// 绘制OLED上的眼睛，增加缩放因子
void draw_eyes(bool update, float scale_left, float scale_right) {
  display.clearDisplay();

  // 绘制左眼，应用缩放因子
  int scaled_left_width = left_eye_width * scale_left;
  int scaled_left_height = left_eye_height * scale_left;
  int x = left_eye_x - scaled_left_width / 2;
  int y = left_eye_y - scaled_left_height / 2;
  display.fillRoundRect(x, y, scaled_left_width, scaled_left_height, ref_corner_radius, SSD1306_WHITE);

  // 绘制右眼，应用缩放因子
  int scaled_right_width = right_eye_width * scale_right;
  int scaled_right_height = right_eye_height * scale_right;
  x = right_eye_x - scaled_right_width / 2;
  y = right_eye_y - scaled_right_height / 2;
  display.fillRoundRect(x, y, scaled_right_width, scaled_right_height, ref_corner_radius, SSD1306_WHITE);

  if (update) {
    display.display();
  }
}

// 将眼睛居中显示
void center_eyes(bool update) {
  // 重置眼睛尺寸为参考值
  left_eye_height = ref_eye_height;
  left_eye_width = ref_eye_width;
  right_eye_height = ref_eye_height;
  right_eye_width = ref_eye_width;

  // 计算居中位置
  left_eye_x = (SCREEN_WIDTH / 2) - (ref_eye_width / 2) - (ref_space_between_eye / 2);
  left_eye_y = SCREEN_HEIGHT / 2;
  right_eye_x = left_eye_x + ref_eye_width + ref_space_between_eye;
  right_eye_y = SCREEN_HEIGHT / 2;

  draw_eyes(update, 1.0, 1.0);
}

// 眨眼动画
void blink(int speed) {
  // 关闭眼睛
  for (int i = 0; i < speed; i++) {
    left_eye_height -= 2;
    right_eye_height -= 2;
    draw_eyes(true, 1.0, 1.0);
    delay(20);
  }

  // 完全闭合
  left_eye_height = 0;
  right_eye_height = 0;
  draw_eyes(true, 1.0, 1.0);
  delay(100);

  // 打开眼睛
  for (int i = 0; i < speed; i++) {
    left_eye_height += 2;
    right_eye_height += 2;
    draw_eyes(true, 1.0, 1.0);
    delay(20);
  }

  // 恢复到参考高度
  left_eye_height = ref_eye_height;
  right_eye_height = ref_eye_height;
  draw_eyes(true, 1.0, 1.0);
}

// 睡眠状态：将眼睛绘制为水平线
void sleep_eyes() {
  display.clearDisplay();

  // 绘制左眼为一条水平线
  display.drawLine(left_eye_x - left_eye_width / 2, left_eye_y, 
                  left_eye_x + left_eye_width / 2, left_eye_y, SSD1306_WHITE);

  // 绘制右眼为一条水平线
  display.drawLine(right_eye_x - right_eye_width / 2, right_eye_y, 
                  right_eye_x + right_eye_width / 2, right_eye_y, SSD1306_WHITE);

  display.display();
}

// 醒来动画：逐步恢复眼睛高度
void wakeup_eyes() {
  // 逐步恢复眼睛高度
  for (int h = 2; h <= ref_eye_height; h += 2) {
    left_eye_height = h;
    right_eye_height = h;
    draw_eyes(true, 1.0, 1.0);
    delay(20);
  }
}

// 显示微笑表情
void happy_eye() {
  center_eyes(false);

  // 绘制圆角眼睛
  draw_eyes(false, 1.0, 1.0);

  // 在眼睛下方绘制笑脸（三角形）
  int offset = ref_eye_height / 2;

  // 左眼笑脸
  display.fillTriangle(
    left_eye_x - left_eye_width / 2 - 1, left_eye_y + offset,
    left_eye_x + left_eye_width / 2 + 1, left_eye_y + 5 + offset,
    left_eye_x - left_eye_width / 2 - 1, left_eye_y + left_eye_height + offset,
    SSD1306_BLACK
  );

  // 右眼笑脸
  display.fillTriangle(
    right_eye_x + right_eye_width / 2 + 1, right_eye_y + offset,
    right_eye_x - right_eye_width / 2 - 1, right_eye_y + 5 + offset,
    right_eye_x + right_eye_width / 2 + 1, right_eye_y + right_eye_height + offset,
    SSD1306_BLACK
  );

  display.display();
  delay(1000); // 保持笑脸状态1秒

  // 恢复眼睛到正常状态
  center_eyes(true);
}

// 根据眼睛位置计算缩放因子
float calculate_scale(float eye_x, float eye_width, int direction, int screen_width, float min_scale, float max_scale, float scale_start_distance) {
  float scale = min_scale;
  if (direction > 0) { // 向右移动
    float distance_to_edge = screen_width - (eye_x + eye_width / 2);
    if (distance_to_edge < scale_start_distance) {
      scale = min_scale + (max_scale - min_scale) * (scale_start_distance - distance_to_edge) / scale_start_distance;
      if (scale > max_scale) scale = max_scale;
    }
  } else if (direction < 0) { // 向左移动
    float distance_to_edge = eye_x - eye_width / 2;
    if (distance_to_edge < scale_start_distance) {
      scale = min_scale + (max_scale - min_scale) * (scale_start_distance - distance_to_edge) / scale_start_distance;
      if (scale > max_scale) scale = max_scale;
    }
  }
  return scale;
}

// 平滑移动眼睛的函数，增加探索边界的随机性和动态幅度
void saccade(int direction_x, int direction_y, int total_steps, int base_amplitude_x, int base_amplitude_y) {
  // 计算每一步的基础移动幅度
  float step_x = (float)(base_amplitude_x * direction_x) / total_steps;
  float step_y = (float)(base_amplitude_y * direction_y) / total_steps;

  // 定义缩放参数
  float min_scale = 1.0;
  float max_scale = 1.3;
  float scale_start_distance = 30.0; // 在离边缘20像素内开始缩放

  for (int i = 0; i < total_steps; i++) {
    // 引入随机性：在垂直方向加入随机偏移
    int random_y_direction = random(-1, 2); // -1, 0, 或 1
    float random_step_y = step_y + (random_y_direction * 1.0); // 每步最多偏移1像素

    // 计算新的位置
    float new_left_eye_x = left_eye_x + step_x;
    float new_right_eye_x = right_eye_x + step_x;
    float new_left_eye_y = left_eye_y + random_step_y;
    float new_right_eye_y = right_eye_y + random_step_y;

    // 检查是否超出屏幕边界
    if (new_left_eye_x - left_eye_width / 2 < 0 || new_right_eye_x + right_eye_width / 2 > SCREEN_WIDTH ||
        new_left_eye_y - left_eye_height / 2 < 0 || new_left_eye_y + left_eye_height / 2 > SCREEN_HEIGHT ||
        new_right_eye_y - right_eye_height / 2 < 0 || new_right_eye_y + right_eye_height / 2 > SCREEN_HEIGHT) {
      break; // 如果超出边界，则停止移动
    }

    // 更新眼睛的位置
    left_eye_x = new_left_eye_x;
    right_eye_x = new_right_eye_x;
    left_eye_y = new_left_eye_y;
    right_eye_y = new_right_eye_y;

    // 计算缩放因子
    float left_scale = calculate_scale(left_eye_x, left_eye_width, direction_x, SCREEN_WIDTH, min_scale, max_scale, scale_start_distance);
    float right_scale = calculate_scale(right_eye_x, right_eye_width, direction_x, SCREEN_WIDTH, min_scale, max_scale, scale_start_distance);

    // 绘制更新后的眼睛位置和大小
    draw_eyes(true, left_scale, right_scale);
    delay(10); // 调整延迟时间以实现平滑移动
  }
}
