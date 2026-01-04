/* Lưu giá trị đếm từ nút nhấn vào EEPROM nội
 *
 * Mô tả: Chương trình này triển khai một bộ đếm đơn giản với các nút nhấn để
 * tăng hoặc đặt lại giá trị. Giá trị bộ đếm được lưu trữ trong bộ nhớ EEPROM
 * nội bộ của Zerobase 2, đảm bảo rằng giá trị không bị mất khi mất điện.
 * Giá trị hiện tại được hiển thị trên màn hình LCD I2C.
 *
 * Link bài viết hướng dẫn: [Thêm link bài viết hướng dẫn chi tiết]
 */

// Thêm các thư viện cần thiết
#include <LiquidCrystal_I2C.h>  // Thư viện điều khiển màn hình LCD qua I2C
#include <EEPROM.h>             // Thư viện để truy cập bộ nhớ EEPROM
#include <ZBPrint.h>            // Thư viện in ra Serial Monitor

// Định nghĩa các chân kết nối
const int COUNT_BUTTON_PIN = 1;  // Chân kết nối nút đếm lên
const int RESET_BUTTON_PIN = 0;  // Chân kết nối nút reset bộ đếm
const int EEPROM_ADDRESS = 0;    // Địa chỉ EEPROM để lưu giá trị bộ đếm

// Khởi tạo màn hình LCD với địa chỉ I2C (thường là 0x27 hoặc 0x3F)
// Cài đặt 16 cột và 2 dòng
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Khai báo các biến
uint8_t counter = 0;               // Giá trị bộ đếm (0-255)
bool countButtonState = HIGH;      // Trạng thái hiện tại của nút đếm
bool lastCountButtonState = HIGH;  // Trạng thái trước đó của nút đếm
bool resetButtonState = HIGH;      // Trạng thái hiện tại của nút reset
bool lastResetButtonState = HIGH;  // Trạng thái trước đó của nút reset

void setup() {
  // Khởi tạo giao tiếp Serial để gỡ lỗi
  Serial.begin(9600);  // Thiết lập tốc độ giao tiếp là 9600 baud
  while (!Serial) {
    delay(10);  // Đợi kết nối cổng Serial
  }

  Serial.println("Counter with EEPROM Storage");  // In thông báo khởi động lên Serial Monitor

  // Khởi tạo màn hình LCD
  lcd.init();       // Khởi tạo màn hình LCD
  lcd.backlight();  // Bật đèn nền cho màn hình LCD

  // Hiển thị thông báo khởi động trên LCD
  lcd.setCursor(2, 0);         // Đặt con trỏ ở cột 2, dòng 0 (dòng đầu tiên)
  lcd.print("Zerobase 2!!!");  // Hiển thị tên thiết bị
  lcd.setCursor(3, 1);         // Đặt con trỏ ở cột 3, dòng 1 (dòng thứ hai)
  lcd.print("Starting...");    // Hiển thị thông báo đang khởi động
  delay(2000);                 // Chờ 2 giây để người dùng đọc thông báo

  // Thiết lập các nút nhấn với điện trở kéo lên
  pinMode(COUNT_BUTTON_PIN, INPUT_PULLUP);  // Cấu hình chân nút đếm với trở kéo lên
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);  // Cấu hình chân nút reset với trở kéo lên

  // Khởi tạo EEPROM
  EEPROM.begin();  // Bắt đầu sử dụng EEPROM

  // Đọc giá trị bộ đếm từ EEPROM
  counter = EEPROM.read(EEPROM_ADDRESS);                               // Đọc giá trị từ địa chỉ EEPROM đã định nghĩa
  Serial.println("Reading stored counter value: " + String(counter));  // In giá trị đã đọc lên Serial

  // Hiển thị giá trị bộ đếm ban đầu
  updateDisplay();  // Gọi hàm cập nhật màn hình LCD
}

void loop() {
  // Đọc trạng thái nút đếm (do dùng INPUT_PULLUP nên LOW nghĩa là được nhấn)
  countButtonState = digitalRead(COUNT_BUTTON_PIN);  // Đọc trạng thái hiện tại của nút đếm

  // Nút đếm được nhấn (phát hiện cạnh xuống)
  if (countButtonState == LOW && lastCountButtonState == HIGH) {  // Kiểm tra xem nút vừa được nhấn xuống
    delay(50);                                                    // Chờ 50ms để chống dội phím
    // Kiểm tra xem nút vẫn còn được nhấn sau khi chống dội
    if (digitalRead(COUNT_BUTTON_PIN) == LOW) {  // Kiểm tra lại sau khoảng thời gian chống dội
      // Tăng bộ đếm, giữ trong phạm vi 0-255
      if (counter < 255) {  // Kiểm tra xem giá trị hiện tại có nhỏ hơn 255 không
        counter++;          // Tăng bộ đếm lên 1
      }
      // Lưu vào EEPROM
      EEPROM.write(EEPROM_ADDRESS, counter);  // Ghi giá trị counter vào địa chỉ EEPROM_ADDRESS
      // Thực hiện commit để đảm bảo dữ liệu được lưu vào bộ nhớ flash
      bool commitSuccess = EEPROM.commit();  // Gọi hàm commit() và lưu kết quả

      // Cập nhật hiển thị
      updateDisplay();  // Cập nhật giá trị mới lên màn hình LCD

      // In thông báo trạng thái lên Serial Monitor
      if (commitSuccess) {
        Serial.println("Counter incremented to: " + String(counter) + " and saved to EEPROM");  // Thông báo thành công
      } else {
        Serial.println("Counter incremented to: " + String(counter) + " but EEPROM save FAILED!");  // Thông báo lỗi
      }
    }
  }

  // Đọc trạng thái nút reset
  resetButtonState = digitalRead(RESET_BUTTON_PIN);  // Đọc trạng thái hiện tại của nút reset

  // Nút reset được nhấn (phát hiện cạnh xuống)
  if (resetButtonState == LOW && lastResetButtonState == HIGH) {  // Kiểm tra xem nút vừa được nhấn xuống
    delay(50);                                                    // Chờ 50ms để chống dội phím
    // Kiểm tra xem nút vẫn còn được nhấn sau khi chống dội
    if (digitalRead(RESET_BUTTON_PIN) == LOW) {  // Kiểm tra lại sau khoảng thời gian chống dội
      // Đặt lại bộ đếm về 0
      counter = 0;  // Reset giá trị counter về 0
      // Lưu vào EEPROM
      EEPROM.write(EEPROM_ADDRESS, counter);  // Ghi giá trị 0 vào địa chỉ EEPROM_ADDRESS
      // Thực hiện commit để đảm bảo dữ liệu được lưu vào bộ nhớ flash
      bool commitSuccess = EEPROM.commit();  // Gọi hàm commit() và lưu kết quả

      // Cập nhật hiển thị
      updateDisplay();  // Cập nhật giá trị mới lên màn hình LCD

      // In thông báo trạng thái lên Serial Monitor
      if (commitSuccess) {
        Serial.println("Counter reset to 0 and saved to EEPROM");  // Thông báo thành công
      } else {
        Serial.println("Counter reset to 0 but EEPROM save FAILED!");  // Thông báo lỗi
      }
    }
  }

  // Cập nhật trạng thái nút nhấn cho lần lặp tiếp theo
  lastCountButtonState = countButtonState;  // Lưu trạng thái hiện tại của nút đếm cho lần kiểm tra tiếp theo
  lastResetButtonState = resetButtonState;  // Lưu trạng thái hiện tại của nút reset cho lần kiểm tra tiếp theo

  // Độ trễ ngắn để tránh dội phím
  delay(10);  // Chờ 10ms giữa các lần kiểm tra trạng thái nút
}

// Hàm cập nhật hiển thị LCD
void updateDisplay() {
  lcd.setCursor(1, 0);          // Đặt con trỏ ở cột 1, dòng 0 (dòng đầu tiên)
  lcd.print("Press to Count");  // Hiển thị hướng dẫn sử dụng

  lcd.setCursor(3, 1);                 // Đặt con trỏ ở cột 3, dòng 1 (dòng thứ hai)
  lcd.print("Count: ");                // Hiển thị chữ "Count: "
  lcd.print("                     ");  // Xóa dữ liệu cũ trên màn hình bằng cách ghi đè khoảng trắng

  lcd.setCursor(11, 1);  // Đặt con trỏ ở cột 11, dòng 1
  lcd.print(counter);    // Hiển thị giá trị đếm hiện tại
}