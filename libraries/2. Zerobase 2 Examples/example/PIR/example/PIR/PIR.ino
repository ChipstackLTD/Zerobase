/* Điều khiển đèn LED bằng cảm biến chuyển động PIR
 *
 * Mô tả: Chương trình này sử dụng cảm biến chuyển động PIR để phát hiện sự
 * hiện diện của người hoặc vật thể. Khi phát hiện chuyển động, một đèn LED
 * sẽ được bật sáng. Sau đó, LED sẽ tắt ngay lập tức trong vòng lặp tiếp
 * theo. Điều này có thể khiến LED nháy nhanh nếu bạn muốn nó sáng liên tục
 * khi có chuyển động, bạn cần thêm độ trễ hoặc logic trạng thái.
 *
 * Link bài viết hướng dẫn: [Thêm link bài viết hướng dẫn chi tiết]
 */

// Khai báo chân LED được kết nối tại chân số 3
const int led = 3;

// Khai báo chân cảm biến PIR (Passive Infrared Sensor) được kết nối tại chân số 14
const int pir = 14;

void setup() {
  // Cấu hình chân cảm biến PIR là INPUT để nhận tín hiệu phát hiện chuyển động
  pinMode(pir, INPUT);

  // Cấu hình chân LED là OUTPUT để điều khiển đèn
  pinMode(led, OUTPUT);
}

void loop() {
  // Kiểm tra xem cảm biến PIR có phát hiện chuyển động không
  if (digitalRead(pir) == HIGH) {
    digitalWrite(led, HIGH);  // Bật đèn LED nếu có chuyển động
  }

  // Ngay lập tức tắt LED, khiến LED nháy nhanh thay vì duy trì sáng chỉ khi có chuyển động
  digitalWrite(led, LOW);
}