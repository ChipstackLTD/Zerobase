/* Điều khiển Relay bằng Remote IR (Hồng ngoại)
 *
 * Mô tả: Chương trình này cho phép bạn điều khiển các module relay bằng
 * remote hồng ngoại sử dụng cảm biến VS1838B. Nó tự triển khai việc đọc
 * tín hiệu IR theo giao thức NEC mà không cần thư viện ngoài, giúp Zerobase
 * phản ứng với các nút nhấn cụ thể trên remote để bật/tắt relay.
 *
 * Link bài viết hướng dẫn: [Thêm link bài viết hướng dẫn chi tiết]
 */

// Khai báo chân nhận tín hiệu IR
const int IR_PIN = 2;  // Chân D2 kết nối với chân OUT của VS1838B

// Thời gian chờ timeout (10ms) cho từng bước khi chờ trạng thái HIGH hoặc LOW
const int TIMEOUT_US = 10000;

// Khai báo chân điều khiển relay
const int RELAY1_PIN = 10;  // Relay 1 kết nối chân D10
const int RELAY2_PIN = 11;  // Relay 2 kết nối chân D11

void setup() {
  pinMode(IR_PIN, INPUT);  // Đặt chân IR là đầu vào để nhận tín hiệu từ remote

  pinMode(RELAY1_PIN, OUTPUT);  // Đặt chân relay 1 là đầu ra
  pinMode(RELAY2_PIN, OUTPUT);  // Đặt chân relay 2 là đầu ra

  // Relay mức thấp: LOW là bật, HIGH là tắt
  digitalWrite(RELAY1_PIN, HIGH);  // Tắt relay 1 lúc khởi động
  digitalWrite(RELAY2_PIN, HIGH);  // Tắt relay 2 lúc khởi động
}

// Hàm chờ cho đến khi IR_PIN có trạng thái 'state', hoặc hết thời gian timeout_us
bool waitForState(int state, unsigned long timeout_us) {
  unsigned long start = micros();  // Lưu thời điểm bắt đầu
  while (digitalRead(IR_PIN) != state) {
    if ((micros() - start) > timeout_us) {
      return false;  // Nếu quá thời gian chờ, thoát và trả về false
    }
  }
  return true;  // Nếu đến trạng thái đúng, trả về true
}

// Hàm đọc tín hiệu IR theo giao thức NEC
unsigned long readIR() {
  // Bước 1: Chờ tín hiệu bắt đầu (Start signal) = 9ms LOW + 4.5ms HIGH
  if (!waitForState(LOW, TIMEOUT_US)) return 0;  // Chờ bắt đầu xuống LOW
  unsigned long t1 = micros();
  if (!waitForState(HIGH, TIMEOUT_US)) return 0;  // Kết thúc phần LOW
  unsigned long t2 = micros();
  if ((t2 - t1) < 8000 || (t2 - t1) > 10000) return 0;  // Kiểm tra có phải 9ms không

  // Bước 2: Chờ phần HIGH 4.5ms
  t1 = micros();
  if (!waitForState(LOW, TIMEOUT_US)) return 0;
  t2 = micros();
  if ((t2 - t1) < 4000 || (t2 - t1) > 5000) return 0;  // Kiểm tra có phải 4.5ms không

  // Bước 3: Đọc 32 bit dữ liệu từ remote
  unsigned long data = 0;
  for (int i = 0; i < 32; i++) {
    if (!waitForState(HIGH, TIMEOUT_US)) return 0;  // Bắt đầu HIGH của bit
    t1 = micros();
    if (!waitForState(LOW, TIMEOUT_US)) return 0;  // Kết thúc HIGH
    t2 = micros();

    unsigned long pulse = t2 - t1;  // Đo thời gian HIGH

    if (pulse > 1000) {
      data = (data << 1) | 1;  // Nếu HIGH dài (~1.69ms) => bit 1
    } else {
      data = (data << 1);  // Nếu HIGH ngắn (~0.56ms) => bit 0
    }
  }

  return data;  // Trả về mã IR đọc được
}

void loop() {
  unsigned long irCode = readIR();  // Gọi hàm đọc tín hiệu IR

  if (irCode != 0) {  // Nếu có mã IR nhận được

    // Xử lý mã lệnh từ remote
    switch (irCode) {
      case 0xFFA25D:  // Nút: bật relay 1
        digitalWrite(RELAY1_PIN, LOW);
        break;

      case 0xFF629D:  // Nút: bật relay 2
        digitalWrite(RELAY2_PIN, LOW);
        break;

      case 0xFFE21D:  // Nút: tắt relay 1
        digitalWrite(RELAY1_PIN, HIGH);
        break;

      case 0xFF22DD:  // Nút: tắt relay 2
        digitalWrite(RELAY2_PIN, HIGH);
        break;

      case 0xFF02FD:  // Nút: bật cả 2 relay
        digitalWrite(RELAY1_PIN, LOW);
        digitalWrite(RELAY2_PIN, LOW);
        break;

      case 0xFFC23D:  // Nút: tắt cả 2 relay
        digitalWrite(RELAY1_PIN, HIGH);
        digitalWrite(RELAY2_PIN, HIGH);
        break;
    }
  }
}