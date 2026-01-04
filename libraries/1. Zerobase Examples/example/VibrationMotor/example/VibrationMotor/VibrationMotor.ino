/* Điều khiển Motor Rung (Vibration Motor Control)
 *
 * Mô tả: Chương trình này điều khiển một motor rung, tạo ra một chuỗi rung động
 * theo chu kỳ. Motor sẽ rung 3 lần liên tiếp, mỗi lần 2 giây, với khoảng dừng
 * 600ms giữa các lần rung. Sau 3 lần rung, chương trình sẽ tạm dừng 5 giây
 * trước khi lặp lại chu kỳ. Điều này hữu ích cho các ứng dụng cảnh báo, phản
 * hồi xúc giác hoặc các hiệu ứng rung khác.
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=101
 */

// Khai báo chân số 2 là chân xuất tín hiệu cho motor rung
const int vibOutPin = 2;

void setup() {
  // Thiết lập chân vibOutPin là ngõ ra (output) để điều khiển motor rung
  pinMode(vibOutPin, OUTPUT);
}

void loop() {
  // Bật motor rung (HIGH)
  digitalWrite(vibOutPin, HIGH);
  // Giữ trạng thái rung trong 2000ms (2 giây)
  delay(2000);

  // Tắt motor rung (LOW)
  digitalWrite(vibOutPin, LOW);
  // Dừng 600ms trước lần rung tiếp theo
  delay(600);

  // Lặp lại chu kỳ rung lần 2
  digitalWrite(vibOutPin, HIGH);
  delay(2000);

  digitalWrite(vibOutPin, LOW);
  delay(600);

  // Lặp lại chu kỳ rung lần 3
  digitalWrite(vibOutPin, HIGH);
  delay(2000);

  digitalWrite(vibOutPin, LOW);
  delay(600);

  // Dừng 5 giây sau khi hoàn thành 3 lần rung trước khi bắt đầu lại chu kỳ mới
  delay(5000);
}