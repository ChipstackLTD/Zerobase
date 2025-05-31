/* Điều khiển Servo bằng Biến trở trên Zerobase 2
 *
 * Mô tả: Chương trình này cho phép bạn điều khiển góc quay của động cơ servo
 * bằng cách sử dụng một biến trở (potentiometer). Giá trị analog đọc được từ
 * biến trở sẽ được chuyển đổi (map) thành dải góc từ 0 đến 180 độ, sau đó
 * được gửi đến động cơ servo để điều chỉnh vị trí của nó. Đây là một ví dụ
 * cơ bản về điều khiển servo, phù hợp cho các ứng dụng cần điều chỉnh góc
 * một cách thủ công.
 *
 * Link bài viết hướng dẫn chi tiết: https://elearning.chipstack.vn/mod/page/view.php?id=146
 */

#include <ZBServo.h>  // Thư viện ZBServo giúp điều khiển servo dễ dàng trên Zerobase

const int servoPin = 19;  // Pin 19 được kết nối với chân tín hiệu của động cơ servo
const int potPin = A0;    // Pin A0 được kết nối với chân tín hiệu của biến trở (potentiometer)
int potValue = 0;         // Biến để lưu giá trị đọc được từ biến trở (từ 0 đến 4095 cho Zerobase 2)
int angle = 0;            // Biến để lưu góc quay mong muốn của servo (từ 0 đến 180 độ)

ZBServo servo;  // Khởi tạo đối tượng servo từ thư viện ZBServo

void setup() {
  // Gắn (attach) đối tượng servo vào pin đã chỉ định (pin 19).
  // Điều này cho phép thư viện ZBServo kiểm soát động cơ servo thông qua pin đó.
  servo.attach(servoPin);
}

void loop() {
  // Đọc giá trị analog từ biến trở được kết nối với pin A0.
  // Zerobase 2 có ADC 12-bit, nên giá trị đọc được sẽ từ 0 đến 4095.
  potValue = analogRead(potPin);

  // Chuyển đổi (map) giá trị đọc được từ biến trở (0-4095)
  // sang dải góc tiêu chuẩn của servo (0-180 độ).
  angle = map(potValue, 0, 4095, 0, 180);

  // Điều khiển động cơ servo quay đến góc đã chuyển đổi.
  // Hàm write() sẽ gửi tín hiệu PWM thích hợp để servo di chuyển đến vị trí này.
  servo.write(angle);

  // Tạm dừng một chút để servo có thời gian di chuyển đến vị trí mới.
  // Khoảng dừng 15ms thường đủ để servo phản ứng mượt mà.
  delay(15);
}