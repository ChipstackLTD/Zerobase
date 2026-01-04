/* Đo khoảng cách bằng cảm biến siêu âm HC-SR04 và hiển thị lên LCD I2C
 *
 * Mô tả: Chương trình này sử dụng cảm biến siêu âm HC-SR04 để đo khoảng cách
 * đến vật thể và hiển thị giá trị đo được (bằng centimet) lên màn hình LCD 16x2
 * thông qua giao tiếp I2C. Đây là một ứng dụng phổ biến cho các dự án đo lường
 * hoặc phát hiện vật cản.
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=110
 */

#include <LiquidCrystal_I2C.h>  // Thư viện điều khiển LCD I2C

// Khởi tạo đối tượng LCD với địa chỉ I2C là 0x27 và kích thước 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Khai báo các chân kết nối với cảm biến siêu âm
const int trigPin = 11;  // Chân gửi tín hiệu (TRIG)
const int echoPin = 10;  // Chân nhận tín hiệu (ECHO)

void setup() {
  pinMode(trigPin, OUTPUT);  // Thiết lập trigPin là OUTPUT
  pinMode(echoPin, INPUT);   // Thiết lập echoPin là INPUT

  lcd.init();       // Khởi tạo LCD
  lcd.backlight();  // Bật đèn nền LCD

  // Hiển thị thông điệp khởi động
  lcd.setCursor(2, 0);       // Đặt con trỏ ở cột 2, dòng 0
  lcd.print("Zerobase!!!");  // In dòng chữ "Zerobase!!!"
  lcd.setCursor(2, 1);       // Đặt con trỏ ở cột 2, dòng 1
  lcd.print("Starting...");  // In dòng chữ "Starting..."
  delay(2000);               // Chờ 2 giây để người dùng đọc

  lcd.clear();             // Xóa màn hình LCD
  lcd.setCursor(4, 0);     // Đặt con trỏ ở cột 4, dòng 0
  lcd.print("Distance:");  // In tiêu đề "Distance:"
}

// Hàm đo khoảng cách bằng cảm biến siêu âm
long getDistance() {
  digitalWrite(trigPin, LOW);  // Đảm bảo tín hiệu bắt đầu ở mức thấp
  delayMicroseconds(2);        // Chờ 2 micro giây

  digitalWrite(trigPin, HIGH);  // Gửi xung siêu âm trong 10 micro giây
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);  // Kết thúc xung

  long duration = pulseIn(echoPin, HIGH);  // Đo thời gian phản hồi của sóng siêu âm
  // Tính khoảng cách dựa trên công thức: khoảng cách = (thời gian * tốc độ âm thanh) / 2
  // Tốc độ âm thanh trong không khí khoảng 343 mét/giây = 0.0343 cm/micro giây
  // Chia 2 vì sóng âm đi và về
  long distance = duration * 0.034 / 2;  // Tính khoảng cách (cm)
  return distance;                       // Trả về kết quả
}

void loop() {
  long distance = getDistance();  // Gọi hàm đo khoảng cách

  lcd.setCursor(6, 1);  // Đặt con trỏ tại cột 6, dòng 1
  lcd.print("      ");  // Xóa giá trị cũ bằng khoảng trắng để tránh số cũ còn sót
  lcd.setCursor(6, 1);  // Đặt lại con trỏ tại vị trí in
  lcd.print(distance);  // In khoảng cách đo được
  lcd.print(" cm");     // In đơn vị "cm"

  delay(500);  // Chờ 500ms trước khi đo lại
}