/* Điều khiển tốc độ và hướng động cơ DC bằng biến trở
 *
 * Mô tả: Chương trình này cho phép bạn điều khiển tốc độ và hướng quay của
 * một động cơ DC bằng cách sử dụng một biến trở. Khi vặn biến trở, động cơ
 * sẽ quay theo hai hướng khác nhau với tốc độ thay đổi từ 0 đến tối đa.
 * Chương trình cũng bao gồm một "vùng chết" (deadzone) ở giữa để dừng động cơ
 * khi biến trở ở vị trí trung tâm, giúp tránh nhiễu và hoạt động ổn định hơn.
 *
 * Link bài viết hướng dẫn: [Thêm link bài viết hướng dẫn chi tiết]
 */

// Khai báo các chân điều khiển motor và chân đọc biến trở

const int motorIn1 = 3;  // Chân điều khiển hướng 1 của motor (PWM 12-bit)
const int motorIn2 = 2;  // Chân điều khiển hướng 2 của motor (PWM 12-bit hoặc digital)
const int potPin = A3;   // Chân đọc giá trị biến trở (ADC 10-bit)

void setup() {
  // Cấu hình các chân I/O

  pinMode(motorIn1, OUTPUT);      // Thiết lập chân motorIn1 là OUTPUT để xuất tín hiệu PWM/digital
  pinMode(motorIn2, OUTPUT);      // Thiết lập chân motorIn2 là OUTPUT để điều khiển hướng quay khác
  pinMode(potPin, INPUT_ANALOG);  // Thiết lập chân potPin là INPUT_ANALOG để đọc giá trị ADC từ biến trở
}

void loop() {
  // Vòng lặp chính – thực hiện liên tục

  int sensorValue = analogRead(potPin);  // Đọc giá trị điện áp từ biến trở (0 - 1023 vì là ADC 10-bit)
  int pwmValue;                          // Biến dùng để lưu giá trị PWM sẽ xuất ra motor

  if (sensorValue < 515) {
    // Nếu giá trị biến trở nhỏ hơn 515 => điều khiển motor quay ngược

    pwmValue = map(sensorValue, 0, 515, 4095, 0);
    // Ánh xạ giá trị ADC 0–515 thành PWM từ 4095 (tốc độ tối đa) về 0 (tốc độ dừng)
    // Càng vặn nhỏ biến trở thì tốc độ càng cao (quay ngược)

    analogWrite(motorIn1, 0);         // Đặt motorIn1 = 0
    analogWrite(motorIn2, pwmValue);  // Xuất PWM tới motorIn2 để quay ngược

  } else if (sensorValue > 524) {
    // Nếu giá trị biến trở lớn hơn 524 => điều khiển motor quay xuôi

    pwmValue = map(sensorValue, 524, 1023, 0, 4095);
    // Ánh xạ giá trị ADC từ 524–1023 thành PWM 0–4095
    // Càng vặn lớn biến trở thì tốc độ càng cao (quay xuôi)

    analogWrite(motorIn1, pwmValue);  // Xuất xung PWM (12-bit) tới motorIn1 để quay xuôi
    analogWrite(motorIn2, 0);         // Đặt motorIn2 = 0 để tạo phân cực đúng chiều quay
  } else {
    // Nếu giá trị nằm trong khoảng 515–524 => vùng chết (deadzone) => dừng motor

    analogWrite(motorIn1, 0);  // Dừng xuất PWM
    analogWrite(motorIn2, 0);  // Dừng xuất PWM
  }

  delay(5);  // Delay nhỏ để tránh nhiễu, cho phép cập nhật tốc độ thường xuyên
}