/* Điều khiển Động cơ DC qua Serial trên Zerobase 2
 *
 * Mô tả: Sketch này cho phép bạn điều khiển động cơ DC (sử dụng cầu H L298N hoặc tương tự)
 * thông qua giao tiếp Serial. Người dùng có thể nhập các lệnh để:
 * - Điều khiển động cơ quay thuận (Forward) với tốc độ mong muốn.
 * - Điều khiển động cơ quay ngược (Reverse) với tốc độ mong muốn.
 * - Dừng động cơ.
 * Tốc độ được điều chỉnh bằng PWM (Pulse Width Modulation) với dải giá trị từ 0 đến 4095
 * (tương ứng với độ phân giải 12-bit của Zerobase 2).
 *
 * Hướng dẫn sử dụng lệnh:
 * - Để quay thuận: Gửi "F <tốc độ>" (ví dụ: "F 1000" để quay thuận với tốc độ 1000)
 * - Để quay ngược: Gửi "R <tốc độ>" (ví dụ: "R 2000" để quay ngược với tốc độ 2000)
 * - Để dừng: Gửi "S"
 *
 * Lưu ý: Tốc độ phải là số nguyên từ 0 đến 4095.
 *
 * Link bài viết hướng dẫn chi tiết: [THÊM_LINK_BÀI_VIẾT_CỦA_BẠN_VỀ_ĐIỀU_KHIỂN_ĐỘNG_CƠ_DC_ZEROBASÉ]
 */

#include <ZBPrint.h>  // Thư viện hỗ trợ Serial

// Khai báo chân điều khiển động cơ
const int IN1 = 3;  // D3 nối IN1 của driver động cơ (hỗ trợ PWM)
const int IN2 = 2;  // D2 nối IN2 của driver động cơ (hỗ trợ PWM)

// Biến lưu chuỗi lệnh nhận từ Serial
String inputString = "";

void setup() {
  // Thiết lập hai chân điều khiển động cơ là OUTPUT (ngõ ra)
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  // Khởi tạo trạng thái ban đầu là dừng động cơ bằng cách đặt cả hai chân về 0 PWM
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);

  Serial.begin(9600);             // Khởi động giao tiếp Serial ở baudrate 9600 bps
  Serial.println("Motor ready");  // Gửi thông báo "Motor ready" ra Serial Monitor khi hệ thống sẵn sàng
}

void loop() {
  // Đọc dữ liệu từ Serial nếu có ký tự khả dụng trong bộ đệm Serial
  while (Serial.available()) {
    char c = Serial.read();  // Đọc từng ký tự một từ bộ đệm Serial

    if (c == '\n') {               // Nếu ký tự đọc được là ký tự xuống dòng ('\n'), nghĩa là đã hết một lệnh
      handleCommand(inputString);  // Gọi hàm handleCommand() để xử lý chuỗi lệnh đã nhận được
      inputString = "";            // Sau khi xử lý, xoá rỗng chuỗi inputString để chuẩn bị nhận lệnh mới
    } else if (c != '\r') {        // Nếu ký tự đọc được không phải là ký tự carriage return ('\r')
      inputString += c;            // Thêm ký tự vào cuối chuỗi inputString
    }
  }
}

// Hàm xử lý lệnh nhận từ Serial
void handleCommand(String cmd) {
  cmd.trim();         // Xoá bỏ tất cả khoảng trắng (space, tab, newline, carriage return) ở đầu và cuối chuỗi lệnh
  cmd.toUpperCase();  // Chuyển đổi toàn bộ chuỗi lệnh sang chữ hoa để tiện cho việc so sánh và xử lý

  // Kiểm tra nếu lệnh bắt đầu bằng "F " (Forward - Quay thuận)
  if (cmd.startsWith("F ")) {              // Ví dụ: "F 1000"
    int speed = cmd.substring(2).toInt();  // Trích xuất phần số sau "F " và chuyển đổi thành số nguyên (tốc độ)
    speed = constrain(speed, 0, 4095);     // Giới hạn giá trị tốc độ trong khoảng từ 0 đến 4095 (do Zerobase 2 có PWM 12-bit)

    analogWrite(IN1, speed);    // Thiết lập PWM cho chân IN1 bằng giá trị tốc độ
    analogWrite(IN2, 0);        // Đặt chân IN2 về 0 để đảm bảo động cơ quay thuận
    Serial.print("Forward: ");  // In ra Serial Monitor thông báo động cơ đang quay thuận
    Serial.println(speed);      // In ra tốc độ hiện tại

  }
  // Kiểm tra nếu lệnh bắt đầu bằng "R " (Reverse - Quay ngược)
  else if (cmd.startsWith("R ")) {         // Ví dụ: "R 500"
    int speed = cmd.substring(2).toInt();  // Trích xuất phần số sau "R " và chuyển đổi thành số nguyên (tốc độ)
    speed = constrain(speed, 0, 4095);     // Giới hạn giá trị tốc độ trong khoảng từ 0 đến 4095

    analogWrite(IN1, 0);        // Đặt chân IN1 về 0
    analogWrite(IN2, speed);    // Thiết lập PWM cho chân IN2 bằng giá trị tốc độ để động cơ quay ngược
    Serial.print("Reverse: ");  // In ra Serial Monitor thông báo động cơ đang quay ngược
    Serial.println(speed);      // In ra tốc độ hiện tại

  }
  // Kiểm tra nếu lệnh là "S" (Stop - Dừng)
  else if (cmd == "S") {     // Ví dụ: "S"
    analogWrite(IN1, 0);     // Đặt cả hai chân điều khiển về 0 PWM
    analogWrite(IN2, 0);     // Đảm bảo động cơ dừng hoàn toàn
    Serial.println("Stop");  // In ra Serial Monitor thông báo động cơ đã dừng

  }
  // Nếu lệnh không khớp với bất kỳ định dạng nào trên
  else {
    Serial.println("Invalid command");  // In ra thông báo lỗi "Invalid command"
  }
}