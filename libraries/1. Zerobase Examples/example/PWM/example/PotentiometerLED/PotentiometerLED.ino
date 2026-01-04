/* Điều khiển độ sáng LED bằng biến trở (PWM)
 *
 * Mô tả: Chương trình này cho phép bạn điều chỉnh độ sáng của một đèn LED
 * bằng cách sử dụng một biến trở. Giá trị analog từ biến trở sẽ được đọc
 * và sau đó được chuyển đổi sang một dải giá trị phù hợp để điều khiển
 * độ rộng xung (PWM) của đèn LED, tạo ra hiệu ứng làm mờ (dimming).
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=122
 */

// Khai báo chân kết nối LED (hằng số, không thay đổi)
const int led = 3;

// Khai báo chân kết nối biến trở (hằng số, không thay đổi)
const int potPin = A1;

void setup() {
  // Cấu hình chân LED là OUTPUT (ngõ ra)
  pinMode(led, OUTPUT);

  // Cấu hình chân biến trở là INPUT_ANALOG (ngõ vào analog)
  pinMode(potPin, INPUT_ANALOG);
}

void loop() {
  // Đọc giá trị từ biến trở (giá trị ADC 10 bit từ 0 đến 1023)
  int sensorValue = analogRead(potPin);

  // Chuyển đổi giá trị ADC từ 10 bit (0-1023) sang 12 bit (0-4095)
  // Vì ADC của Zerobase chỉ có độ phân giải 10 bit, tức là giá trị đọc được chỉ nằm trong khoảng từ 0 đến 1023,
  // trong khi tín hiệu PWM đầu ra của vi điều khiển sử dụng độ phân giải 12 bit (từ 0 đến 4095).
  // Do đó, cần sử dụng hàm map để mở rộng giá trị ADC nhằm phù hợp với dải điều khiển PWM.
  sensorValue = map(sensorValue, 0, 1023, 0, 4095);

  // Xuất tín hiệu PWM đến LED với độ sáng tương ứng với giá trị biến trở
  analogWrite(led, sensorValue);
}