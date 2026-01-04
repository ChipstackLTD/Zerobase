/* Điều Khiển LED qua Serial1 trên Zerobase 2
 *
 * Mô tả: Sketch này cho phép bạn điều khiển các đèn LED riêng lẻ hoặc nhóm LED
 * thông qua giao tiếp Serial1. Nó nhận các lệnh văn bản từ cổng Serial và
 * chuyển đổi chúng thành các hành động bật/tắt đèn LED tương ứng.
 * Rất hữu ích cho các ứng dụng điều khiển từ xa đơn giản hoặc giao tiếp giữa
 * các module.
 *
 * Hướng dẫn sử dụng lệnh:
 * - Bật tất cả LED: "ON ALL"
 * - Tắt tất cả LED: "OFF ALL"
 * - Bật một LED cụ thể: "ON <số_chân_LED>" (ví dụ: "ON 2", "ON 11")
 * - Tắt một LED cụ thể: "OFF <số_chân_LED>" (ví dụ: "OFF 3", "OFF 12")
 *
 * Link bài viết hướng dẫn chi tiết: [THÊM_LINK_BÀI_VIẾT_CỦA_BẠN_VỀ_ĐIỀU_KHIỂN_LED_SERIAL1_ZEROBASÉ]
 */

// Khai báo các chân LED
const int ledPins[] = { 2, 3, 10, 11, 12 };  // Mảng chứa các chân LED được kết nối: D2, D3, D10, D11, D12

const int ledCount = sizeof(ledPins) / sizeof(ledPins[0]);  // Tính số lượng LED dựa trên kích thước của mảng ledPins

String inputString = "";  // Biến dùng để lưu chuỗi lệnh nhận được từ Serial1

void setup() {
  // Thiết lập từng chân LED trong mảng là OUTPUT và tắt LED ban đầu
  for (int i = 0; i < ledCount; i++) {
    pinMode(ledPins[i], OUTPUT);    // Cấu hình chân LED hiện tại là OUTPUT (ngõ ra)
    digitalWrite(ledPins[i], LOW);  // Đặt trạng thái ban đầu của LED là LOW (tắt)
  }

  Serial1.begin(9600);  // Khởi động giao tiếp Serial1 với tốc độ baudrate 9600
}

void loop() {
  // Kiểm tra nếu có dữ liệu khả dụng trong bộ đệm của Serial1 thì đọc và xử lý
  while (Serial1.available()) {
    char c = Serial1.read();  // Đọc từng ký tự một từ cổng Serial1

    if (c == '\n') {               // Nếu ký tự đọc được là ký tự xuống dòng ('\n'), nghĩa là đã hết một lệnh
      handleCommand(inputString);  // Gọi hàm handleCommand() để xử lý chuỗi lệnh đã nhận được
      inputString = "";            // Xoá rỗng chuỗi inputString sau khi xử lý xong để chuẩn bị nhận lệnh mới
    } else if (c != '\r') {        // Nếu ký tự đọc được không phải là ký tự carriage return ('\r')
      inputString += c;            // Thêm ký tự vào cuối chuỗi inputString để xây dựng lệnh
    }
  }
}

// Hàm xử lý lệnh điều khiển nhận được từ Serial1
void handleCommand(String cmd) {
  cmd.trim();         // Xoá bỏ tất cả khoảng trắng (space, tab, newline, carriage return) ở đầu và cuối chuỗi lệnh
  cmd.toUpperCase();  // Chuyển đổi toàn bộ chuỗi lệnh sang chữ hoa để đơn giản hóa việc so sánh và xử lý

  // Kiểm tra các loại lệnh khác nhau
  if (cmd == "ON ALL") {
    // Lệnh "ON ALL": Bật tất cả các đèn LED
    for (int i = 0; i < ledCount; i++) {
      digitalWrite(ledPins[i], HIGH);  // Bật từng LED bằng cách đặt chân về HIGH
    }
    Serial1.println("All LEDs ON");  // Gửi phản hồi "All LEDs ON" qua Serial1
  } else if (cmd == "OFF ALL") {
    // Lệnh "OFF ALL": Tắt tất cả các đèn LED
    for (int i = 0; i < ledCount; i++) {
      digitalWrite(ledPins[i], LOW);  // Tắt từng LED bằng cách đặt chân về LOW
    }
    Serial1.println("All LEDs OFF");  // Gửi phản hồi "All LEDs OFF" qua Serial1
  } else if (cmd.startsWith("ON ")) {
    // Lệnh "ON <số_chân_LED>": Bật một LED cụ thể
    int pin = cmd.substring(3).toInt();  // Trích xuất phần số sau "ON " và chuyển đổi thành số nguyên (số chân LED)
    setLed(pin, true);                   // Gọi hàm setLed() để bật LED ở chân tương ứng
  } else if (cmd.startsWith("OFF ")) {
    // Lệnh "OFF <số_chân_LED>": Tắt một LED cụ thể
    int pin = cmd.substring(4).toInt();  // Trích xuất phần số sau "OFF " và chuyển đổi thành số nguyên (số chân LED)
    setLed(pin, false);                  // Gọi hàm setLed() để tắt LED ở chân tương ứng
  } else {
    // Lệnh không hợp lệ
    Serial1.println("Invalid command");  // Gửi thông báo "Invalid command" nếu lệnh không được nhận dạng
  }
}

// Hàm bật/tắt LED theo số chân cụ thể và trạng thái (true = ON, false = OFF)
void setLed(int pin, bool state) {
  // Duyệt qua mảng các chân LED đã khai báo
  for (int i = 0; i < ledCount; i++) {
    if (ledPins[i] == pin) {                  // Nếu tìm thấy chân LED khớp với số chân trong lệnh
      digitalWrite(pin, state ? HIGH : LOW);  // Đặt trạng thái của LED: HIGH nếu state là true (bật), LOW nếu state là false (tắt)

      Serial1.print("LED on D");                // Gửi phản hồi "LED on D"
      Serial1.print(pin);                       // In ra số chân của LED
      Serial1.println(state ? " ON" : " OFF");  // In ra trạng thái "ON" hoặc "OFF"

      return;  // Thoát khỏi hàm sau khi đã xử lý xong (tìm thấy và điều khiển LED)
    }
  }

  // Nếu vòng lặp kết thúc mà không tìm thấy chân LED hợp lệ trong mảng
  Serial1.println("Invalid pin");  // Gửi thông báo "Invalid pin" qua Serial1
}