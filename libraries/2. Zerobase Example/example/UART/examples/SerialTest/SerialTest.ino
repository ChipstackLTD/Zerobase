/* Giao tiếp Serial Monitor bằng dây USB-UART TTL
 *
 * Mô tả: Chương trình này trình diễn giao tiếp hai chiều giữa Zerobase
 * và máy tính (hoặc thiết bị khác) thông qua cổng Serial1 (UART), sử dụng
 * một cáp USB-UART TTL. Zerobase sẽ lắng nghe dữ liệu đến từ cổng Serial.
 * Khi nhận được dữ liệu, nó sẽ in lại dữ liệu đó kèm theo thông báo "Received: ".
 * Nếu không có dữ liệu, nó sẽ thông báo "No data received!".
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=86
 */

void setup() {
  // Khởi tạo cổng Serial1 với tốc độ truyền là 9600 baud.
  // Đây là tốc độ mà cả Zerobase và thiết bị giao tiếp (ví dụ: Serial Monitor trên máy tính)
  // phải được cấu hình để đảm bảo dữ liệu được truyền nhận chính xác.
  Serial1.begin(9600);
}

void loop() {
  // Kiểm tra xem có bất kỳ dữ liệu nào sẵn sàng để đọc từ cổng Serial1 không.
  // `Serial1.available()` trả về số byte (ký tự) hiện có trong bộ đệm nhận.
  if (Serial1.available() > 0) {
    // Nếu có dữ liệu đến từ Serial1, đọc toàn bộ chuỗi dữ liệu nhận được.
    // `Serial1.readString()` đọc các ký tự từ bộ đệm cho đến khi hết thời gian chờ
    // hoặc cho đến khi một ký tự kết thúc dòng được nhận.
    String data = Serial1.readString();

    // Xóa các ký tự khoảng trắng (space, tab, newline, carriage return) ở đầu và cuối chuỗi.
    // Điều này đặc biệt hữu ích khi nhận dữ liệu từ Serial Monitor, nơi nhấn Enter
    // thường gửi kèm các ký tự xuống dòng ('\r', '\n').
    data.trim();

    // Phản hồi lại dữ liệu đã nhận được bằng cách in nó ra cổng Serial1.
    // `Serial1.println()` tự động thêm ký tự xuống dòng sau chuỗi,
    // giúp mỗi lần gửi hiển thị trên một dòng mới trong Serial Monitor.
    Serial1.println("Received: " + data);
  } else {
    // Nếu không có dữ liệu nào trong bộ đệm nhận của Serial1, in ra thông báo này.
    // Điều này cho biết Zerobase đang chờ dữ liệu.
    Serial1.println("No data received!");
  }

  // Chờ 1 giây (1000 mili giây) trước khi thực hiện vòng lặp `loop()` tiếp theo.
  // Khoảng trễ này giúp hạn chế việc gửi dữ liệu quá nhanh, làm cho output dễ đọc hơn
  // và giảm tải cho vi điều khiển.
  delay(1000);
}