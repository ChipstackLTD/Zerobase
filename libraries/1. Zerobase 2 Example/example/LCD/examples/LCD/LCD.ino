/* Kiểm tra đầy đủ các chức năng của LCD I2C
 *
 * Mô tả: Chương trình này trình diễn hầu hết các chức năng cơ bản và nâng cao
 * của màn hình LCD 16x2 thông qua giao tiếp I2C, bao gồm hiển thị chữ, số,
 * ký tự tùy chỉnh, điều khiển con trỏ và đèn nền, cũng như hiệu ứng cuộn và nhấp nháy.
 * Đây là một ví dụ toàn diện để kiểm tra hoạt động của module LCD I2C với Zerobase.
 *
 * Link bài viết hướng dẫn: [Thêm link bài viết hướng dẫn chi tiết]
 */

#include <LiquidCrystal_I2C.h>  // Thư viện điều khiển LCD qua I2C

// Khai báo đối tượng LCD với địa chỉ 0x27, màn hình 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Định nghĩa các ký tự tùy chỉnh để hiển thị trên LCD
uint8_t HeartChar[] = { 0x00, 0x00, 0x0a, 0x15, 0x11, 0x0a, 0x04, 0x00 };       // Biểu tượng trái tim
uint8_t SpeakerChar[] = { 0x01, 0x03, 0x07, 0x1f, 0x1f, 0x07, 0x03, 0x01 };     // Biểu tượng loa
uint8_t SmileyFaceChar[] = { 0x00, 0x00, 0x0a, 0x00, 0x1f, 0x11, 0x0e, 0x00 };  // Mặt cười
uint8_t BellChar[] = { 0x04, 0x0e, 0x0a, 0x0a, 0x0a, 0x1f, 0x00, 0x04 };        // Chuông
uint8_t Battery1Char[] = { 0x0e, 0x1b, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1f };    // Pin mức 1
uint8_t Battery2Char[] = { 0x0e, 0x1b, 0x11, 0x11, 0x11, 0x11, 0x1f, 0x1f };    // Pin mức 2
uint8_t Battery3Char[] = { 0x0e, 0x1b, 0x11, 0x11, 0x11, 0x1f, 0x1f, 0x1f };    // Pin mức 3
uint8_t Battery4Char[] = { 0x0e, 0x1b, 0x11, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f };    // Pin mức 4

// Hàm hiển thị một chuỗi tại vị trí (col, row) trong một khoảng thời gian nhất định
void showMessage(const char *msg, int delayTime, int col, int row) {
  lcd.clear();
  lcd.setCursor(col, row);  // Đặt con trỏ tại vị trí mong muốn
  lcd.print(msg);           // In chuỗi ra màn hình LCD
  delay(delayTime);         // Dừng trong khoảng thời gian chỉ định
}

// Hàm nhấp nháy một đoạn văn bản tại hàng row, số lần times
void blinkText(String text, int row, int times) {
  for (int i = 0; i < times; i++) {
    lcd.setCursor(4, row);
    lcd.print(text);
    delay(500);
    lcd.setCursor(4, row);
    lcd.print("                ");  // Xóa chữ
    delay(500);
  }
}

void setup() {
  lcd.init();       // Khởi tạo LCD
  lcd.backlight();  // Bật đèn nền

  // Lưu các ký tự tùy chỉnh vào bộ nhớ LCD
  lcd.createChar(0, HeartChar);
  lcd.createChar(1, SpeakerChar);
  lcd.createChar(2, SmileyFaceChar);
  lcd.createChar(3, BellChar);
  lcd.createChar(4, Battery1Char);
  lcd.createChar(5, Battery2Char);
  lcd.createChar(6, Battery3Char);
  lcd.createChar(7, Battery4Char);
}

void loop() {
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("Zerobase");
  lcd.setCursor(3, 1);
  lcd.print("Starting...");
  delay(2000);
  lcd.clear();


  // Hiển thị chữ thường, chữ hoa và số
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lowercase: abcdef");
  lcd.setCursor(0, 1);
  lcd.print("Uppercase: ABCDEF");
  delay(2000);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Numbers: 1234567890");
  delay(2000);

  // Hiển thị các ký tự tùy chỉnh
  lcd.clear();
  lcd.print("Custom Characters:");
  lcd.setCursor(0, 1);
  lcd.write(byte(0));
  lcd.setCursor(2, 1);
  lcd.write(byte(1));
  lcd.setCursor(4, 1);
  lcd.write(byte(2));
  lcd.setCursor(6, 1);
  lcd.write(byte(3));
  lcd.setCursor(8, 1);
  lcd.write(byte(4));
  lcd.setCursor(10, 1);
  lcd.write(byte(5));
  lcd.setCursor(12, 1);
  lcd.write(byte(6));
  lcd.setCursor(14, 1);
  lcd.write(byte(7));
  delay(2000);

  // Hiển thị và nhấp nháy con trỏ
  String cursorOn = "Cursor ON";
  showMessage(cursorOn.c_str(), 1000, 0, 0);
  // Nhấp nháy con trỏ
  lcd.cursor();
  for (int i = 0; i < cursorOn.length(); i++) {
    lcd.setCursor(i, 1);
    lcd.print(cursorOn[i]);
    delay(120);
  }
  delay(2000);
  lcd.noCursor();
  showMessage("Cursor OFF", 2000, 0, 0);

  String blinkOn = "Blink ON";
  showMessage("Blink ON", 1000, 0, 0);
  lcd.blink();
  for (int i = 0; i < blinkOn.length(); i++) {
    lcd.setCursor(i, 1);
    lcd.print(blinkOn[i]);
    delay(120);
  }
  delay(2000);
  lcd.noBlink();
  showMessage("Blink OFF", 2000, 0, 0);

  // Cuộn chữ qua lại
  showMessage("Scrolling Left", 0, 0, 0);
  for (int i = 0; i < 16; i++) {
    lcd.scrollDisplayLeft();
    delay(300);
  }
  showMessage("Scrolling Right", 0, 0, 0);
  for (int i = 0; i < 16; i++) {
    lcd.scrollDisplayRight();
    delay(300);
  }

  // Tắt/bật đèn nền
  showMessage("Backlight OFF", 0, 2, 0);
  lcd.noBacklight();
  delay(2000);
  lcd.backlight();
  showMessage("Backlight ON", 2000, 2, 0);

  lcd.clear();
  blinkText("Blinking!", 0, 5);
  delay(1000);
  lcd.clear();

  // Hoàn tất chương trình
  showMessage("Done!", 3000, 5, 0);
}