/*
 * Điều khiển LED bằng cảm biến Hall
 * 
 * Mô tả: Chương trình đọc tín hiệu từ cảm biến Hall và điều khiển LED
 * Khi có từ trường gần cảm biến, LED sẽ sáng, ngược lại LED sẽ tắt
 * 
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=95
 */

const int hallPin = 2;  // Chân D2 đọc tín hiệu từ cảm biến Hall
const int ledPin = 3;   // Chân D3 điều khiển LED

void setup() {
  pinMode(hallPin, INPUT_PULLUP);  // Cảm biến Hall là ngõ vào với điện trở kéo lên
  pinMode(ledPin, OUTPUT);         // LED là ngõ ra
}

void loop() {
  int hallState = digitalRead(hallPin);  // Đọc trạng thái cảm biến Hall

  if (hallState == LOW) {        // Có từ trường (tùy loại cảm biến, có thể là LOW)
    digitalWrite(ledPin, HIGH);  // Bật LED
  } else {
    digitalWrite(ledPin, LOW);  // Tắt LED
  }

  delay(200);  // Chờ một chút để tránh nhiễu
}