/* Điều khiển nhiều đèn LED bằng UART TTL
 *
 * Mô tả: Chương trình này cho phép bạn điều khiển nhiều đèn LED khác nhau
 * được kết nối với Zerobase thông qua các lệnh gửi từ giao tiếp Serial1 (UART).
 * Bạn có thể bật/tắt từng LED cụ thể hoặc bật/tắt tất cả các LED cùng một lúc.
 * Đây là một ví dụ cơ bản về giao tiếp Zerobase với máy tính hoặc các thiết bị
 * khác qua cổng serial để điều khiển ngoại vi.
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=87
 */

// Khai báo các chân LED
const int ledPins[] = { 2, 3, 10, 11, 12 };  // Mảng chứa các chân LED: D2, D3, D10, D11, D12

const int ledCount = sizeof(ledPins) / sizeof(ledPins[0]);  // Tính số lượng LED dựa trên kích thước mảng

String inputString = "";  // Biến lưu chuỗi lệnh nhận được từ Serial1

void setup() {
  // Thiết lập các chân LED là OUTPUT và tắt LED ban đầu
  for (int i = 0; i < ledCount; i++) {
    pinMode(ledPins[i], OUTPUT);    // Cấu hình chân LED là OUTPUT
    digitalWrite(ledPins[i], LOW);  // Tắt LED ở trạng thái ban đầu
  }

  Serial1.begin(9600);  // Khởi động giao tiếp Serial1 với baudrate 9600
}

void loop() {
  // Nếu có dữ liệu từ Serial1 thì đọc và xử lý
  while (Serial1.available()) {
    char c = Serial1.read();  // Đọc từng ký tự từ Serial1

    if (c == '\n') {               // Nếu gặp ký tự xuống dòng '\n', bắt đầu xử lý lệnh
      handleCommand(inputString);  // Gọi hàm xử lý lệnh
      inputString = "";            // Xoá chuỗi sau khi xử lý xong
    } else if (c != '\r') {        // Bỏ qua ký tự carriage return '\r'
      inputString += c;            // Thêm ký tự vào chuỗi lệnh
    }
  }
}

// Hàm xử lý lệnh điều khiển từ Serial1
void handleCommand(String cmd) {
  cmd.trim();         // Xoá khoảng trắng đầu/cuối chuỗi lệnh
  cmd.toUpperCase();  // Chuyển lệnh sang chữ hoa để xử lý dễ hơn

  if (cmd == "ON ALL") {
    // Bật tất cả LED
    for (int i = 0; i < ledCount; i++) {
      digitalWrite(ledPins[i], HIGH);  // Bật từng LED
    }
    Serial1.println("All LEDs ON");  // Phản hồi qua Serial1
  } else if (cmd == "OFF ALL") {
    // Tắt tất cả LED
    for (int i = 0; i < ledCount; i++) {
      digitalWrite(ledPins[i], LOW);  // Tắt từng LED
    }
    Serial1.println("All LEDs OFF");  // Phản hồi qua Serial1
  } else if (cmd.startsWith("ON ")) {
    // Bật LED theo số chân
    int pin = cmd.substring(3).toInt();  // Lấy phần số sau "ON "
    setLed(pin, true);                   // Gọi hàm bật LED tương ứng
  } else if (cmd.startsWith("OFF ")) {
    // Tắt LED theo số chân
    int pin = cmd.substring(4).toInt();  // Lấy phần số sau "OFF "
    setLed(pin, false);                  // Gọi hàm tắt LED tương ứng
  } else {
    // Nếu lệnh không hợp lệ
    Serial1.println("Invalid command");  // Thông báo lỗi
  }
}

// Hàm bật/tắt LED theo số chân cụ thể
void setLed(int pin, bool state) {
  for (int i = 0; i < ledCount; i++) {
    if (ledPins[i] == pin) {
      digitalWrite(pin, state ? HIGH : LOW);  // Bật hoặc tắt LED tuỳ theo trạng thái

      Serial1.print("LED on D");                // Phản hồi chân LED
      Serial1.print(pin);                       //
      Serial1.println(state ? " ON" : " OFF");  // Phản hồi trạng thái

      return;  // Thoát khỏi hàm sau khi xử lý xong
    }
  }

  // Nếu không tìm thấy chân LED hợp lệ
  Serial1.println("Invalid pin");  // Thông báo lỗi
}