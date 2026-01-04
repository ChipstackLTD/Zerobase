/*
* Máy đếm số với LCD I2C và nút nhấn
* 
* Mô tả: Chương trình tạo máy đếm số hiển thị trên màn hình LCD I2C với ZeroBase.
* Sử dụng 2 nút nhấn: một nút để tăng số đếm và một nút để reset về 0.
* Có chức năng chống dội phím (debounce) để đảm bảo tính chính xác khi nhấn nút.
* 
* Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=105
*/

#include <LiquidCrystal_I2C.h>

// Khai báo chân kết nối của các nút bấm
const int btn_count_pin = 1;  // Nút đếm ở chân 1
const int btn_reset_pin = 0;  // Nút reset ở chân 0

// Thời gian chống dội phím (debounce)
const int debounce_time = 150;  // Đơn vị: mili giây

// Khởi tạo màn hình LCD I2C với địa chỉ 0x27, 16 cột và 2 hàng
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Biến lưu giá trị đếm
int count = 0;

// Biến trạng thái của các nút bấm
bool lastBtnCountState = HIGH;
bool lastBtnResetState = HIGH;

// Biến lưu thời gian lần cuối nhấn nút
unsigned long lastBtnCountTime = 0;
unsigned long lastBtnResetTime = 0;

void setup() {
  // Khởi động LCD
  lcd.init();
  lcd.backlight();

  // Thiết lập chế độ cho các chân nút bấm
  pinMode(btn_count_pin, INPUT_PULLUP);
  pinMode(btn_reset_pin, INPUT_PULLUP);

  // Hiển thị thông báo khởi động trên LCD
  lcd.setCursor(3, 0);
  lcd.print("Zerobase!!!");
  lcd.setCursor(3, 1);
  lcd.print("Starting...");
  delay(2000);

  // Xóa màn hình và cập nhật giá trị ban đầu
  lcd.clear();
  updateLCD();
}

void loop() {
  unsigned long currentMillis = millis();  // Lấy thời gian hiện tại

  // Đọc trạng thái nút đếm
  bool btnCountState = digitalRead(btn_count_pin);
  if (btnCountState == LOW && lastBtnCountState == HIGH && (currentMillis - lastBtnCountTime) > debounce_time) {
    count++;                           // Tăng giá trị đếm
    updateLCD();                       // Cập nhật màn hình LCD
    lastBtnCountTime = currentMillis;  // Lưu thời gian nhấn nút
  }
  lastBtnCountState = btnCountState;  // Cập nhật trạng thái nút

  // Đọc trạng thái nút reset
  bool btnResetState = digitalRead(btn_reset_pin);
  if (btnResetState == LOW && lastBtnResetState == HIGH && (currentMillis - lastBtnResetTime) > debounce_time) {
    count = 0;                         // Reset giá trị đếm
    updateLCD();                       // Cập nhật màn hình LCD
    lastBtnResetTime = currentMillis;  // Lưu thời gian nhấn nút
  }
  lastBtnResetState = btnResetState;  // Cập nhật trạng thái nút
}

// Cập nhật giá trị hiển thị trên LCD
void updateLCD() {
  lcd.setCursor(1, 0);
  lcd.print("Press to Count");  // Hiển thị hướng dẫn sử dụng

  lcd.setCursor(3, 1);
  lcd.print("Count: ");                // Hiển thị chữ "Count"
  lcd.print("                     ");  // Xóa dữ liệu cũ để tránh lỗi hiển thị

  lcd.setCursor(11, 1);
  lcd.print(count);  // Hiển thị giá trị đếm
}