/* Kiểm Tra Giao Tiếp Serial1 (Echo)
 *
 * Mô tả: Sketch này thiết lập giao tiếp trên cổng Serial1 để nhận dữ liệu
 * và sau đó phản hồi lại chính dữ liệu đó.
 *
 * Link bài viết hướng dẫn chi tiết: [THÊM_LINK_BÀI_VIẾT_CỦA_BẠN_VỀ_KIỂM_TRA_SERIAL1_ZEROBASÉ]
 */

void setup() {
  // Khởi tạo cổng Serial1 với tốc độ truyền là 9600 baud.
  // Serial1 thường được sử dụng cho giao tiếp với các module ngoài,
  // khác với Serial (thường là Serial USB) dùng để giao tiếp với máy tính.
  Serial1.begin(9600);
}

void loop() {
  // Kiểm tra xem có bất kỳ dữ liệu nào đang chờ trong bộ đệm của Serial1 hay không.
  // Nếu Serial1.available() trả về một số lớn hơn 0, có nghĩa là có dữ liệu để đọc.
  if (Serial1.available() > 0) {
    // Đọc toàn bộ chuỗi dữ liệu có sẵn trong bộ đệm Serial1 cho đến khi không còn ký tự nào.
    // Hàm readString() sẽ đọc cho đến khi hết thời gian chờ hoặc nhận được ký tự kết thúc.
    String data = Serial1.readString();

    // Xóa các ký tự khoảng trắng, ký tự xuống dòng (\r, \n) ở đầu và cuối chuỗi.
    // Điều này đặc biệt hữu ích khi dữ liệu được gửi từ bàn phím (nhấn Enter sẽ gửi kèm \r\n).
    data.trim();

    // Phản hồi lại dữ liệu đã nhận được bằng cách in nó ra chính cổng Serial1.
    // Kèm theo chuỗi "Received: " để làm rõ thông tin.
    // Hàm println() sẽ tự động thêm ký tự xuống dòng sau chuỗi.
    Serial1.println("Received: " + data);
  } else {
    // Nếu không có dữ liệu nào trong bộ đệm của Serial1, in ra thông báo này.
    // Điều này giúp bạn biết được trạng thái của cổng Serial1 khi không có hoạt động.
    Serial1.println("No data received!");
  }

  // Dừng chương trình trong 1 giây (1000 mili giây) trước khi thực hiện vòng lặp tiếp theo.
  // Điều này giúp giảm tốc độ gửi dữ liệu khi không có input và dễ dàng quan sát output hơn.
  delay(1000);
}