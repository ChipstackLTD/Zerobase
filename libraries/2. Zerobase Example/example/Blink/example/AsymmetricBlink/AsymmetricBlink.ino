/*
 * LED nhấp nháy cơ bản
 * 
 * Mô tả: Chương trình điều khiển LED nhấp nháy với chu kỳ bật 200ms, tắt 800ms
 * 
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=83
 */

// Khai báo chân kết nối LED (chân số 2)
const int ledPin = 2;

void setup() {
  // Thiết lập chân ledPin là OUTPUT (ngõ ra) để điều khiển LED
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Bật LED (xuất mức HIGH ra chân ledPin)
  digitalWrite(ledPin, HIGH);
  // Giữ trạng thái bật trong 200 mili-giây
  delay(200);  // Bật 200ms

  // Tắt LED (xuất mức LOW ra chân ledPin)
  digitalWrite(ledPin, LOW);
  // Giữ trạng thái tắt trong 800 mili-giây
  delay(800);  // Tắt 800ms
}