/* Điều khiển Động cơ DC bằng UART TTL
 *
 * Mô tả: Chương trình này cho phép điều khiển một động cơ DC thông qua
 * các lệnh gửi từ Serial1 (UART). Bạn có thể điều khiển động cơ quay
 * thuận, quay ngược với tốc độ mong muốn hoặc dừng hoàn toàn. Các lệnh
 * được xử lý đơn giản, dễ dàng mở rộng cho các ứng dụng điều khiển từ xa.
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=97
 */

// Khai báo chân điều khiển động cơ
const int IN1 = 3;  // D3 nối IN1 (PWM)
const int IN2 = 2;  // D2 nối IN2 (PWM)

// Biến lưu chuỗi lệnh nhận từ Serial1
String inputString = "";

void setup() {
  // Thiết lập hai chân là OUTPUT
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  // Khởi tạo trạng thái ban đầu là dừng động cơ
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);

  Serial1.begin(9600);             // Khởi động Serial1 ở baudrate 9600
  Serial1.println("Motor ready");  // Thông báo sẵn sàng
}

void loop() {
  // Đọc dữ liệu từ Serial1 nếu có
  while (Serial1.available()) {
    char c = Serial1.read();  // Đọc từng ký tự

    if (c == '\n') {  // Khi gặp xuống dòng, xử lý lệnh
      handleCommand(inputString);
      inputString = "";      // Xoá chuỗi sau khi xử lý
    } else if (c != '\r') {  // Bỏ qua carriage return
      inputString += c;      // Thêm ký tự vào chuỗi
    }
  }
}

// Hàm xử lý lệnh nhận từ Serial1
void handleCommand(String cmd) {
  cmd.trim();         // Xoá khoảng trắng đầu/cuối
  cmd.toUpperCase();  // Chuyển lệnh sang chữ hoa

  if (cmd.startsWith("F ")) {              // Quay thuận
    int speed = cmd.substring(2).toInt();  // Lấy giá trị tốc độ
    speed = constrain(speed, 0, 4095);     // Giới hạn trong 12-bit (dải PWM của Zerobase)
    analogWrite(IN1, speed);
    analogWrite(IN2, 0);
    Serial1.print("Forward: ");
    Serial1.println(speed);

  } else if (cmd.startsWith("R ")) {  // Quay ngược
    int speed = cmd.substring(2).toInt();
    speed = constrain(speed, 0, 4095);
    analogWrite(IN1, 0);
    analogWrite(IN2, speed);
    Serial1.print("Reverse: ");
    Serial1.println(speed);

  } else if (cmd == "S") {  // Dừng
    analogWrite(IN1, 0);
    analogWrite(IN2, 0);
    Serial1.println("Stop");

  } else {
    Serial1.println("Invalid command");  // Lệnh không hợp lệ
  }
}