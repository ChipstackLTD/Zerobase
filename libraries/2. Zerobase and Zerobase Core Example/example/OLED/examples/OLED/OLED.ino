/* Cuộn chữ trên màn hình OLED SSD1306
 *
 * Mô tả: Chương trình này trình diễn cách hiển thị và cuộn một đoạn văn bản
 * dài trên màn hình OLED SSD1306 128x64 sử dụng giao tiếp I2C. Văn bản sẽ
 * liên tục cuộn ngang qua màn hình, hữu ích cho việc hiển thị thông tin
 * dài hoặc tin nhắn trạng thái.
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=113
 */

// Thêm thư viện Wire.h để hỗ trợ giao tiếp I2C
#include <Wire.h>
// Thêm thư viện cơ bản SSD1306Ascii để điều khiển màn hình OLED
#include "SSD1306Ascii.h"
// Thêm thư viện SSD1306AsciiWire để điều khiển màn hình OLED qua giao tiếp I2C
#include "SSD1306AsciiWire.h"

// Định nghĩa địa chỉ I2C của màn hình OLED (0X3C+SA0 - có thể là 0x3C hoặc 0x3D)
#define I2C_ADDRESS 0x3C

// Định nghĩa chân RST, -1 nghĩa là không sử dụng chân reset riêng
#define RST_PIN -1

// Tạo một đối tượng oled từ lớp SSD1306AsciiWire để điều khiển màn hình
SSD1306AsciiWire oled;

// Khai báo chuỗi văn bản cần hiển thị
const char message[] = "Hello World, this is Zerobase using OLED SSD1306. ";
// Vị trí bắt đầu hiển thị văn bản (cho chức năng cuộn)
int position = 0;
// Biến lưu thời gian của lần cuộn trước đó
unsigned long previousMillis = 0;
// Khoảng thời gian giữa các lần cuộn (150ms)
const long scrollInterval = 150;  // Adjust for scroll speed (milliseconds)
// Số ký tự có thể hiển thị trên màn hình
const int displayWidth = 21;  // Number of characters that fit on the display

// Hàm setup() chạy một lần khi khởi động
void setup() {
  // Khởi tạo giao tiếp I2C
  Wire.begin();
  // Thiết lập tốc độ I2C (400kHz)
  Wire.setClock(400000L);

// Kiểm tra nếu có định nghĩa chân RST
#if RST_PIN >= 0
  // Khởi tạo màn hình OLED với chân RST được chỉ định
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else   // RST_PIN >= 0
  // Khởi tạo màn hình OLED không có chân RST
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif  // RST_PIN >= 0

  // Thiết lập font chữ System5x7
  oled.setFont(System5x7);
  // Xóa màn hình
  oled.clear();
  // Thiết lập kích thước chữ gấp đôi
  oled.set2X();
}

// Hàm loop() chạy lặp đi lặp lại
void loop() {
  // Lấy độ dài của chuỗi văn bản
  int messageLength = strlen(message);

  // Nếu độ dài văn bản lớn hơn độ rộng màn hình, thực hiện cuộn văn bản
  if (messageLength > displayWidth) {
    // Lấy thời gian hiện tại tính bằng mili giây
    unsigned long currentMillis = millis();

    // Kiểm tra nếu đã đến thời điểm cần cuộn văn bản
    if (currentMillis - previousMillis >= scrollInterval) {
      // Cập nhật thời gian cuộn cuối cùng
      previousMillis = currentMillis;

      // Đặt vị trí con trỏ tại cột 0, hàng 3
      // Lưu ý:
      // Số cột x từ 0 - 127
      // Số hàng y từ 0 - 7.
      oled.setCursor(0, 3);

      // Tạo chuỗi để hiển thị trên màn hình
      String displayText = "";

      // Tính toán phần văn bản cần hiển thị
      for (int i = 0; i < displayWidth; i++) {
        // Tính vị trí ký tự dựa trên vị trí hiện tại và độ dài văn bản
        int charPosition = (position + i) % messageLength;
        // Thêm ký tự vào chuỗi hiển thị
        displayText += message[charPosition];
      }

      // Hiển thị văn bản mà không xóa màn hình trước
      oled.print(displayText);

      // Tăng vị trí cho bước cuộn tiếp theo, quay về đầu nếu đến cuối chuỗi
      position = (position + 1) % messageLength;
    }
  }
}