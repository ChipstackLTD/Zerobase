/* Đọc giá trị biến trở với ADC và hiển thị lên LCD I2C
 *
 * Mô tả: Chương trình này đọc giá trị analog từ một biến trở được kết nối
 * với chân A2 của Zerobase, sau đó hiển thị giá trị số (0-1023) lên màn hình
 * LCD 16x2 thông qua giao tiếp I2C.
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=106
 */

#include <LiquidCrystal_I2C.h>  // Thư viện điều khiển màn hình LCD I2C

// Khai báo địa chỉ LCD 0x27 hoặc 0x3F (tùy vào module)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Khởi tạo đối tượng LCD với địa chỉ 0x27, kích thước 16x2

const int potPin = A2;  // Chân analog đọc giá trị từ biến trở

void setup() {
  lcd.init();                 // Khởi động màn hình LCD
  lcd.backlight();            // Bật đèn nền LCD
  lcd.setCursor(2, 0);        // Đặt con trỏ tại vị trí cột 2, dòng 0
  lcd.print("Zerobase ADC");  // In chuỗi "Zerobase ADC" lên LCD
}

void loop() {
  int value = analogRead(potPin);  // Đọc giá trị từ biến trở (0 - 1023)

  lcd.setCursor(2, 1);   // Đặt con trỏ tại vị trí cột 2, dòng 1
  lcd.print("Value: ");  // In chuỗi "Value: " lên LCD
  lcd.print(value);      // Hiển thị giá trị số đọc được từ biến trở
  lcd.print("   ");      // Xóa ký tự dư trên màn hình LCD (nếu giá trị nhỏ hơn giá trị trước đó)

  delay(500);  // Chờ 500ms trước khi cập nhật lại giá trị mới
}