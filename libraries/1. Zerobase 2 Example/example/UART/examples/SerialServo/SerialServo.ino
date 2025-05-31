/* Điều Khiển Động Cơ Servo qua Serial
 *
 * Mô tả: Sketch này cho phép điều khiển hai động cơ servo riêng lẻ hoặc cùng lúc
 * thông qua giao tiếp Serial. Người dùng có thể gửi các lệnh bằng văn bản để
 * thiết lập góc quay cho từng servo.
 *
 * Hướng dẫn sử dụng lệnh:
 * - Điều khiển Servo 1: "S1 <góc>" (ví dụ: "S1 90")
 * - Điều khiển Servo 2: "S2 <góc>" (ví dụ: "S2 45")
 * - Điều khiển cả hai Servo: "ALL <góc>" (ví dụ: "ALL 180")
 *
 * Lưu ý: Góc quay phải là số nguyên từ 0 đến 180.
 *
 * Link bài viết hướng dẫn chi tiết: [THÊM_LINK_BÀI_VIẾT_CỦA_BẠN_VỀ_ĐIỀU_KHIỂN_SERVO_ZEROBASÉ]
 */

#include <ZBPrint.h>  // Thư viện hỗ trợ USB để giao tiếp Serial
#include <ZBServo.h>  // Thư viện điều khiển servo cho Zerobase 2

ZBServo servo1;  // Khai báo đối tượng điều khiển servo thứ nhất
ZBServo servo2;  // Khai báo đối tượng điều khiển servo thứ hai

// Khai báo chân tín hiệu servo
const int servoPin1 = 2;  // Servo 1 được gắn vào chân D2 của bo mạch
const int servoPin2 = 3;  // Servo 2 được gắn vào chân D3 của bo mạch

String inputString = "";  // Biến dùng để lưu trữ dữ liệu lệnh nhận được từ Serial

void setup() {
  // Gắn đối tượng servo1 với chân điều khiển servoPin1 (D2)
  servo1.attach(servoPin1);
  // Gắn đối tượng servo2 với chân điều khiển servoPin2 (D3)
  servo2.attach(servoPin2);

  Serial.begin(9600);             // Khởi động giao tiếp Serial với tốc độ baudrate 9600
  Serial.println("Servo ready");  // Gửi thông báo "Servo ready" ra Serial Monitor khi hệ thống sẵn sàng
}

void loop() {
  // Kiểm tra nếu có dữ liệu khả dụng trong bộ đệm của Serial
  while (Serial.available()) {
    char c = Serial.read();  // Đọc từng ký tự một từ cổng Serial

    if (c == '\n') {               // Nếu ký tự đọc được là ký tự xuống dòng ('\n'), nghĩa là đã hết một lệnh
      handleCommand(inputString);  // Gọi hàm handleCommand() để xử lý chuỗi lệnh đã nhận được
      inputString = "";            // Xoá rỗng chuỗi inputString sau khi xử lý xong để chuẩn bị nhận lệnh mới
    } else if (c != '\r') {        // Nếu ký tự đọc được không phải là ký tự carriage return ('\r')
      inputString += c;            // Thêm ký tự vào cuối chuỗi inputString để xây dựng lệnh
    }
  }
}

// Hàm xử lý chuỗi lệnh nhận được từ Serial
void handleCommand(String cmd) {
  cmd.trim();         // Xoá bỏ tất cả khoảng trắng (space, tab, newline, carriage return) ở đầu và cuối chuỗi lệnh
  cmd.toUpperCase();  // Chuyển đổi toàn bộ chuỗi lệnh sang chữ hoa để đơn giản hóa việc so sánh và xử lý

  // Kiểm tra nếu lệnh bắt đầu bằng "S1 " (Điều khiển Servo 1)
  if (cmd.startsWith("S1 ")) {             // Ví dụ: "S1 90"
    int angle = cmd.substring(3).toInt();  // Trích xuất phần số sau "S1 " và chuyển đổi thành số nguyên (góc quay)
    angle = constrain(angle, 0, 180);      // Giới hạn giá trị góc trong khoảng từ 0 đến 180 độ
    servo1.write(angle);                   // Gửi lệnh quay cho servo1 đến góc đã chỉ định
    Serial.print("Servo 1: ");             // In ra Serial Monitor thông báo điều khiển Servo 1
    Serial.println(angle);                 // In ra góc đã quay

  }
  // Kiểm tra nếu lệnh bắt đầu bằng "S2 " (Điều khiển Servo 2)
  else if (cmd.startsWith("S2 ")) {        // Ví dụ: "S2 45"
    int angle = cmd.substring(3).toInt();  // Trích xuất phần số sau "S2 " và chuyển đổi thành số nguyên (góc quay)
    angle = constrain(angle, 0, 180);      // Giới hạn giá trị góc trong khoảng từ 0 đến 180 độ
    servo2.write(angle);                   // Gửi lệnh quay cho servo2 đến góc đã chỉ định
    Serial.print("Servo 2: ");             // In ra Serial Monitor thông báo điều khiển Servo 2
    Serial.println(angle);                 // In ra góc đã quay

  }
  // Kiểm tra nếu lệnh bắt đầu bằng "ALL " (Điều khiển cả hai Servo)
  else if (cmd.startsWith("ALL ")) {       // Ví dụ: "ALL 0"
    int angle = cmd.substring(4).toInt();  // Trích xuất phần số sau "ALL " và chuyển đổi thành số nguyên (góc quay)
    angle = constrain(angle, 0, 180);      // Giới hạn giá trị góc trong khoảng từ 0 đến 180 độ
    servo1.write(angle);                   // Gửi lệnh quay cho servo1
    servo2.write(angle);                   // Gửi lệnh quay cho servo2 (cùng góc)
    Serial.print("All servos: ");          // In ra Serial Monitor thông báo điều khiển cả hai Servo
    Serial.println(angle);                 // In ra góc đã quay

  }
  // Nếu lệnh không khớp với bất kỳ định dạng nào trên
  else {
    Serial.println("Invalid command");  // In ra thông báo lỗi "Invalid command"
  }
}