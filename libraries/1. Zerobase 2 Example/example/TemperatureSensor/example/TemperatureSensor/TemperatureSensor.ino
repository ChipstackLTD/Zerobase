/* Đọc nhiệt độ DS18B20 và hiển thị lên LCD I2C
 *
 * Mô tả: Chương trình này đọc nhiệt độ từ cảm biến DS18B20 và hiển thị
 * giá trị lên màn hình LCD 16x2 thông qua giao tiếp I2C.
 *
 * Link bài viết hướng dẫn: [Thêm link bài viết hướng dẫn chi tiết]
 */

// Thư viện giao tiếp OneWire (cho cảm biến DS18B20)
#include <OneWire.h>

// Thư viện để đọc nhiệt độ từ cảm biến Dallas như DS18B20
#include <DallasTemperature.h>

// Thư viện điều khiển màn hình LCD thông qua giao tiếp I2C
#include <LiquidCrystal_I2C.h>

// Chân dữ liệu kết nối với cảm biến nhiệt độ DS18B20
#define ONE_WIRE_BUS 2

// Tạo đối tượng oneWire để giao tiếp với các thiết bị OneWire
OneWire oneWire(ONE_WIRE_BUS);

// Tạo đối tượng cảm biến DallasTemperature, sử dụng oneWire để giao tiếp
DallasTemperature sensors(&oneWire);

// Tạo đối tượng LCD I2C, với địa chỉ I2C là 0x27, màn hình 16 cột và 2 hàng
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();       // Khởi tạo LCD
  lcd.backlight();  // Bật đèn nền LCD
  sensors.begin();  // Khởi động thư viện cảm biến nhiệt độ

  lcd.clear();               // Xóa màn hình LCD
  lcd.setCursor(3, 0);       // Di chuyển con trỏ đến cột 3, dòng 0
  lcd.print("Zerobase");     // In dòng chữ "Zerobase"
  lcd.setCursor(3, 1);       // Di chuyển con trỏ đến cột 3, dòng 1
  lcd.print("Starting...");  // In dòng chữ "Starting..."
  delay(2000);               // Đợi 2 giây
  lcd.clear();               // Xóa LCD trước khi vào vòng lặp chính
}

void loop() {
  sensors.requestTemperatures();             // Gửi lệnh yêu cầu đo nhiệt độ
  float tempC = sensors.getTempCByIndex(0);  // Đọc nhiệt độ từ cảm biến số 0 (chỉ dùng 1 cảm biến)

  lcd.setCursor(0, 0);           // Di chuyển con trỏ đến đầu dòng 0
  lcd.print("ZB Temperatures");  // In tiêu đề

  lcd.setCursor(1, 1);  // Di chuyển con trỏ đến cột 1, dòng 1
  lcd.print("Temp: ");  // In nhãn "Temp: "
  lcd.print(tempC);     // In giá trị nhiệt độ (float)
  lcd.print(" C");      // In đơn vị nhiệt độ

  lcd.print("                    ");  // In khoảng trắng để xóa dữ liệu dư nếu có
}