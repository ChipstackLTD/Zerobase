/* Điều khiển Relay bằng Nút nhấn
 *
 * Mô tả: Chương trình này cho phép bạn điều khiển trạng thái bật/tắt của một
 * module relay bằng cách sử dụng một nút nhấn. Khi nút được nhấn, relay sẽ
 * được kích hoạt (bật). Khi nút không được nhấn, relay sẽ ở trạng thái không
 * kích hoạt (tắt). Cấu hình INPUT_PULLUP được sử dụng cho nút nhấn để đơn
 * giản hóa mạch điện.
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=102
 */

// Khai báo chân điều khiển relay, nối với chân số 12 trên vi điều khiển
const int relayPin = 12;

// Khai báo chân nút nhấn, nối với chân số 3 trên vi điều khiển
const int buttonPin = 3;

void setup() {
  // Thiết lập chân relayPin là OUTPUT để có thể điều khiển relay
  pinMode(relayPin, OUTPUT);

  // Thiết lập chân buttonPin là INPUT_PULLUP để sử dụng điện trở pull-up nội
  // Điều này có nghĩa là khi nút chưa nhấn, chân buttonPin sẽ ở mức HIGH (5V)
  // Khi nhấn nút, chân buttonPin sẽ bị kéo xuống mức LOW (0V)
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  // Đọc trạng thái của nút nhấn
  if (digitalRead(buttonPin) == LOW) {
    // Nếu nút nhấn được nhấn (buttonPin = LOW)

    // Đưa relayPin xuống mức LOW, điều này bật relay
    // Vì module relay 5V thường hoạt động với mức điều khiển LOW (mức kích LOW)
    digitalWrite(relayPin, LOW);
  } else {
    // Nếu nút nhấn không được nhấn (buttonPin = HIGH)

    // Đưa relayPin lên mức HIGH, điều này tắt relay
    digitalWrite(relayPin, HIGH);
  }
}