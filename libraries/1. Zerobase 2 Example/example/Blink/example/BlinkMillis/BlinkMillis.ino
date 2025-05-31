/*
 * LED nhấp nháy sử dụng millis()
 * 
 * Mô tả: Chương trình điều khiển LED nhấp nháy không sử dụng delay() mà dùng millis()
 * để theo dõi thời gian, cho phép Zerobase thực hiện các tác vụ khác đồng thời
 * 
 * Link bài viết hướng dẫn: [Thêm link bài viết hướng dẫn chi tiết]
 */

// Khai báo chân kết nối LED (chân số 2)
const int ledPin = 2;

// Biến lưu thời điểm lần cuối LED được thay đổi trạng thái
unsigned long previousMillis = 0;

// Trạng thái hiện tại của LED (LOW là tắt, HIGH là bật)
bool ledState = LOW;

void setup() {
  // Thiết lập chân ledPin là OUTPUT (ngõ ra) để điều khiển LED
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Đọc thời gian hiện tại tính từ khi Zerobase bắt đầu chạy (đơn vị: milli-giây)
  unsigned long currentMillis = millis();

  // Thời gian giữa mỗi lần đổi trạng thái LED (ở đây là 500ms)
  unsigned long interval = 500;  // 0.5 giây

  // Nếu đã đủ thời gian (500ms) kể từ lần đổi trạng thái trước
  if (currentMillis - previousMillis >= interval) {
    // Cập nhật thời điểm đổi trạng thái gần nhất
    previousMillis = currentMillis;

    // Đảo trạng thái LED: nếu đang bật thì tắt, nếu đang tắt thì bật
    ledState = !ledState;

    // Gửi tín hiệu ra chân điều khiển LED để bật hoặc tắt LED
    digitalWrite(ledPin, ledState);
  }
}