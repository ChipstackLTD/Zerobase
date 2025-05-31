/* Buzzer điều khiển bằng delay()
 *
 * Mô tả: Chương trình điều khiển buzzer kêu và tắt xen kẽ sử dụng delay()
 * để tạm dừng chương trình. Trong thời gian delay(), Zerobase sẽ không
 * thực hiện được các tác vụ nào khác.
 *
 * Link bài viết hướng dẫn: [Thêm link bài viết hướng dẫn chi tiết]
 */

// Khai báo chân kết nối với buzzer
const int buzzer = 1;

void setup() {
  // Thiết lập chân buzzer làm đầu ra
  pinMode(buzzer, OUTPUT);
}

void loop() {
  // Bật buzzer
  digitalWrite(buzzer, HIGH);
  // Giữ trạng thái bật trong 1 giây
  delay(1000);

  // Tắt buzzer
  digitalWrite(buzzer, LOW);
  // Giữ trạng thái tắt trong 1 giây
  delay(1000);
}