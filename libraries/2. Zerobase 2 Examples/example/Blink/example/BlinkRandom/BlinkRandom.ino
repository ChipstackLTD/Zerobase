/*
 * LED nhấp nháy ngẫu nhiên sử dụng millis()
 * 
 * Mô tả: Chương trình điều khiển LED nhấp nháy với thời gian ngẫu nhiên từ 100ms đến 1000ms
 * không sử dụng delay() mà dùng millis() để theo dõi thời gian, cho phép Zerobase thực hiện 
 * các tác vụ khác đồng thời. LED sẽ bật/tắt với khoảng thời gian ngẫu nhiên mỗi lần.
 * 
 * Link bài viết hướng dẫn: [Thêm link bài viết hướng dẫn chi tiết]
 */

// Khai báo chân kết nối LED (chân số 2)
const int ledPin = 2;

// Biến lưu thời điểm lần cuối LED được thay đổi trạng thái
unsigned long previousMillis = 0;

// Trạng thái hiện tại của LED (LOW là tắt, HIGH là bật)
bool ledState = LOW;

// Ngưỡng dưới cho thời gian nhấp nháy ngẫu nhiên (100ms)
const int lowRandom = 100;
// Ngưỡng trên cho thời gian nhấp nháy ngẫu nhiên (1000ms)
const int highRandom = 1000;

void setup() {
  // Thiết lập chân ledPin là OUTPUT (ngõ ra) để điều khiển LED
  pinMode(ledPin, OUTPUT);
}

void loop() {
  // Biến lưu thời gian hiện tại, được cập nhật mỗi vòng lặp
  static unsigned long currentMillis = millis();

  // Khoảng thời gian ngẫu nhiên giữa hai lần thay đổi trạng thái LED (100ms–1000ms)
  static unsigned long interval = random(lowRandom, highRandom);

  // Nếu đã đủ thời gian chờ (interval) kể từ lần cuối thay đổi
  if (millis() - previousMillis >= interval) {
    // Cập nhật thời điểm lần cuối đổi trạng thái
    previousMillis = millis();

    // Đảo trạng thái LED (bật nếu đang tắt, tắt nếu đang bật)
    ledState = !ledState;

    // Gửi tín hiệu điều khiển LED theo trạng thái mới
    digitalWrite(ledPin, ledState);

    // Sinh lại giá trị ngẫu nhiên mới cho khoảng thời gian tiếp theo
    interval = random(lowRandom, highRandom);
  }
}