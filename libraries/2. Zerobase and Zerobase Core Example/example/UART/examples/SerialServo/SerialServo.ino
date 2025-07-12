/* Điều khiển động cơ Servo bằng lệnh Serial (UART)
 *
 * Mô tả: Chương trình này cho phép bạn điều khiển hai động cơ servo riêng biệt
 * hoặc đồng thời bằng cách gửi các lệnh qua giao tiếp Serial1 (UART). Bạn có
 * thể chỉ định góc quay mong muốn cho từng servo hoặc cho cả hai. Đây là một
 * giải pháp linh hoạt để thử nghiệm và điều khiển các ứng dụng robot, cánh tay
 * máy hoặc các cơ cấu chuyển động khác.
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=100
 */

#include <ZBServo.h>  // Thư viện điều khiển servo cho Zerobase

ZBServo servo1;  // Đối tượng điều khiển servo thứ nhất
ZBServo servo2;  // Đối tượng điều khiển servo thứ hai

// Khai báo chân tín hiệu servo
const int servoPin1 = 2;  // Servo 1 gắn vào chân D2
const int servoPin2 = 3;  // Servo 2 gắn vào chân D3

String inputString = "";  // Biến lưu dữ liệu lệnh nhận từ Serial1

void setup() {
  servo1.attach(servoPin1);  // Gắn servo1 với chân D2
  servo2.attach(servoPin2);  // Gắn servo2 với chân D3

  Serial1.begin(9600);             // Khởi động Serial1 ở baudrate 9600
  Serial1.println("Servo ready");  // Thông báo sẵn sàng
}

void loop() {
  // Kiểm tra nếu có dữ liệu từ Serial1
  while (Serial1.available()) {
    char c = Serial1.read();  // Đọc từng ký tự

    if (c == '\n') {               // Nếu gặp ký tự xuống dòng, bắt đầu xử lý lệnh
      handleCommand(inputString);  // Gọi hàm xử lý lệnh
      inputString = "";            // Xoá chuỗi sau khi xử lý xong
    } else if (c != '\r') {        // Bỏ qua ký tự carriage return
      inputString += c;            // Thêm ký tự vào chuỗi lệnh
    }
  }
}

// Hàm xử lý chuỗi lệnh từ Serial1
void handleCommand(String cmd) {
  cmd.trim();         // Xoá khoảng trắng đầu/cuối lệnh
  cmd.toUpperCase();  // Chuyển lệnh thành chữ hoa để dễ so sánh

  if (cmd.startsWith("S1 ")) {             // Lệnh điều khiển servo 1
    int angle = cmd.substring(3).toInt();  // Lấy góc quay từ lệnh
    angle = constrain(angle, 0, 180);      // Giới hạn góc từ 0 đến 180 độ
    servo1.write(angle);                   // Gửi lệnh quay cho servo 1
    Serial1.print("Servo 1: ");
    Serial1.println(angle);  // Phản hồi lại góc đã quay

  } else if (cmd.startsWith("S2 ")) {  // Lệnh điều khiển servo 2
    int angle = cmd.substring(3).toInt();
    angle = constrain(angle, 0, 180);
    servo2.write(angle);  // Gửi lệnh quay cho servo 2
    Serial1.print("Servo 2: ");
    Serial1.println(angle);

  } else if (cmd.startsWith("ALL ")) {  // Lệnh điều khiển cả hai servo
    int angle = cmd.substring(4).toInt();
    angle = constrain(angle, 0, 180);
    servo1.write(angle);  // Quay servo 1
    servo2.write(angle);  // Quay servo 2
    Serial1.print("All servos: ");
    Serial1.println(angle);

  } else {
    Serial1.println("Invalid command");  // Lệnh không hợp lệ
  }
}