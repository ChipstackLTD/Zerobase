/* Chống dội nút nhấn với delay()
 *
 * Mô tả: Chương trình này điều khiển một LED bật/tắt mỗi khi nút nhấn được
 * bấm. Nó sử dụng kỹ thuật chống dội (debounce) bằng cách thêm một khoảng
 * delay() ngắn sau khi phát hiện nút nhấn để bỏ qua các tín hiệu nhiễu.
 * Trong thời gian delay(), Zerobase sẽ tạm dừng các tác vụ khác.
 *
 * Link bài viết hướng dẫn: [Thêm link bài viết hướng dẫn chi tiết]
 */

// Khai báo chân kết nối nút nhấn (button) là chân số 3
const int buttonPin = 3;

// Khai báo chân kết nối LED là chân số 2
const int ledPin = 2;

// Thời gian chống dội phím (debounce) tính bằng mili giây
const int debounceTime = 50;

void setup() {
  // Cấu hình chân nút nhấn là INPUT với điện trở kéo lên nội (INPUT_PULLUP)
  pinMode(buttonPin, INPUT_PULLUP);

  // Cấu hình chân LED là OUTPUT
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Kiểm tra nếu nút được nhấn (LOW vì dùng INPUT_PULLUP)
  if (digitalRead(buttonPin) == LOW) {
    // Đợi một khoảng thời gian để chống dội phím
    delay(debounceTime);

    // Kiểm tra lại trạng thái nút sau thời gian debounce
    if (digitalRead(buttonPin) == LOW) {
      // Đảo trạng thái hiện tại của LED (BẬT thành TẮT, TẮT thành BẬT)
      digitalWrite(ledPin, !digitalRead(ledPin));

      // Chờ cho đến khi nút được nhả ra
      while (digitalRead(buttonPin) == LOW)
        ;  // Chờ đến khi nút được nhả ra
    }
  }
}