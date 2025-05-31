/* Điều khiển độ sáng LED bằng Biến trở (Potentiometer) trên Zerobase 2
 *
 * Mô tả: Chương trình này đọc giá trị analog từ một biến trở và sử dụng giá trị đó
 * để điều khiển độ sáng của một đèn LED thông qua tín hiệu PWM (Pulse Width Modulation).
 * Khi bạn xoay biến trở, giá trị đọc được sẽ thay đổi, làm cho độ sáng của LED cũng
 * thay đổi theo, tạo ra hiệu ứng mờ dần (dimming).
 *
 * Link bài viết hướng dẫn chi tiết: https://elearning.chipstack.vn/mod/page/view.php?id=134
 */

#include <ZBPrint.h>

// Khai báo chân kết nối LED (hằng số, không thay đổi)
const int led = 3;

// Khai báo chân kết nối biến trở (hằng số, không thay đổi)
const int potPin = A1;

void setup() {
  Serial.begin(9600);

  // Cấu hình chân LED là OUTPUT (ngõ ra)
  pinMode(led, OUTPUT);

  // Cấu hình chân biến trở là INPUT_ANALOG (ngõ vào analog)
  pinMode(potPin, INPUT_ANALOG);
}

void loop() {
  // Đọc giá trị từ biến trở (giá trị ADC 12 bit từ 0 đến 4095)
  int sensorValue = analogRead(potPin);
  Serial.print("Value: ");
  Serial.println(sensorValue);

  // Xuất tín hiệu PWM đến LED với độ sáng tương ứng với giá trị biến trở
  analogWrite(led, sensorValue);
}