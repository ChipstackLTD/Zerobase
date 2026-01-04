/*
* Điều khiển góc Servo với LCD I2C và nút nhấn
* 
* Mô tả: Chương trình điều khiển góc quay của servo motor thông qua 2 nút nhấn trên ZeroBase.
* Nút tăng để tăng góc servo (mỗi lần 10 độ), nút giảm để giảm góc servo (mỗi lần 10 độ).
* Góc servo được giới hạn trong khoảng 0-180 độ và hiển thị thời gian thực trên màn hình LCD I2C.
* 
* Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=154
*/

#include <LiquidCrystal_I2C.h>  // Thư viện điều khiển màn hình LCD giao tiếp I2C
#include <ZBServo.h>            // Thư viện điều khiển servo của Zerobase

// Khởi tạo LCD địa chỉ 0x27 (tuỳ màn hình có thể là 0x3F), với kích thước 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Khởi tạo đối tượng servo
ZBServo myServo;

// Khai báo các chân kết nối nút nhấn và servo
const int buttonUpPin = 2;    // Nút tăng góc servo
const int buttonDownPin = 3;  // Nút giảm góc servo
const int servoPin = 10;      // Chân điều khiển servo

// Biến lưu trữ góc servo, bắt đầu từ 90 độ (trung vị)
int angle = 90;

void setup() {
  // Khởi động màn hình LCD
  lcd.init();
  lcd.backlight();  // Bật đèn nền LCD

  lcd.clear();               // Xoá màn hình hiển thị
  lcd.setCursor(4, 0);       // Đặt con trỏ tại cột 4, hàng 0
  lcd.print("Zerobase");     // In dòng chữ "Zerobase"
  lcd.setCursor(3, 1);       // Đặt con trỏ tại cột 3, hàng 1
  lcd.print("Starting...");  // In dòng chữ "Starting..."
  delay(2000);               // Dừng 2 giây để hiển thị

  lcd.clear();  // Xoá màn hình

  lcd.setCursor(4, 0);    // Đặt con trỏ
  lcd.print("ZB Servo");  // In tiêu đề

  // Hiển thị góc servo ban đầu
  lcd.setCursor(0, 1);
  lcd.print("Servo Angle: ");
  lcd.print(angle);

  // Gán servo vào chân điều khiển
  myServo.attach(servoPin);
  myServo.write(angle);  // Di chuyển servo tới góc ban đầu

  // Cấu hình các chân nút bấm là đầu vào và bật điện trở kéo lên nội bộ
  pinMode(buttonUpPin, INPUT_PULLUP);
  pinMode(buttonDownPin, INPUT_PULLUP);
}

void loop() {
  // Đọc trạng thái nút tăng góc
  bool upPressed = digitalRead(buttonUpPin) == LOW;

  // Đọc trạng thái nút giảm góc
  bool downPressed = digitalRead(buttonDownPin) == LOW;

  // Nếu nút tăng được nhấn
  if (upPressed) {
    angle += 10;                       // Tăng góc thêm 10 độ
    angle = constrain(angle, 0, 180);  // Giới hạn góc trong khoảng 0-180 độ
    updateServo();                     // Cập nhật servo và hiển thị
    delay(100);                        // Chống rung nút nhấn
  }

  // Nếu nút giảm được nhấn
  if (downPressed) {
    angle -= 10;                       // Giảm góc đi 10 độ
    angle = constrain(angle, 0, 180);  // Giới hạn góc trong khoảng 0-180 độ
    updateServo();                     // Cập nhật servo và hiển thị
    delay(100);                        // Chống rung nút nhấn
  }
}

// Hàm cập nhật vị trí servo và hiển thị lên LCD
void updateServo() {
  myServo.write(angle);  // Di chuyển servo tới góc mới

  lcd.setCursor(4, 0);    // Đặt con trỏ LCD
  lcd.print("ZB Servo");  // In tiêu đề

  lcd.setCursor(0, 1);          // Đặt con trỏ LCD
  lcd.print("Servo Angle: ");   // In dòng chữ
  lcd.print(angle);             // In giá trị góc servo
  lcd.print("              ");  // In thêm khoảng trắng để xoá dữ liệu cũ
}