/*
 * LED kép nhấp nháy đồng bộ
 * 
 * Mô tả: Chương trình điều khiển LED trên board ZeroBase và LED ngoài (chân 3) nhấp nháy 
 * đồng thời với chu kỳ 1 giây (bật 500ms, tắt 500ms). Sử dụng delay() để tạo thời gian chờ
 * giữa các trạng thái bật/tắt của LED.
 * 
 * Link bài viết hướng dẫn: [Thêm link bài viết hướng dẫn chi tiết]
 */

const int ledPin = 3;  // Khai báo biến hằng số cho chân 3

// Hàm setup() chạy một lần khi vi điều khiển khởi động
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);  // Thiết lập LED của board làm đầu ra
  pinMode(ledPin, OUTPUT);       // Thiết lập chân 3 làm đầu ra
}

// Hàm loop() chạy lặp lại liên tục
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);  // Bật LED trên board ZeroBase
  digitalWrite(ledPin, HIGH);       // Bật LED được kết nối với chân 3
  delay(500);                       // Giữ trạng thái HIGH trong 500ms

  digitalWrite(LED_BUILTIN, LOW);  // Tắt LED trên board ZeroBase
  digitalWrite(ledPin, LOW);       // Tắt LED được kết nối với chân 3
  delay(500);                      // Giữ trạng thái LOW trong 500ms
}