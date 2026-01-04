/*******************************************************************************
* Node hiển thị với LCD I2C
*
* Mô tả: Chương trình này hoạt động như một node chính trong mạng CAN bus.
* Nó gửi yêu cầu đọc nhiệt độ và thời gian đến các node cảm biến tương ứng,
* sau đó nhận dữ liệu phản hồi và hiển thị lên màn hình LCD.
*
* Link bài viết hướng dẫn chi tiết: [THÊM_LINK_BÀI_VIẾT_CỦA_BẠN_VỀ_HIỂN_THỊ_CAN_LCD]
*******************************************************************************/

#include <Arduino.h>            // Thư viện cơ bản của Arduino
#include <ZBCan.h>              // Thư viện giao tiếp CAN trên Zerobase
#include <Wire.h>               // Thư viện giao tiếp I2C
#include <LiquidCrystal_I2C.h>  // Thư viện điều khiển LCD qua I2C
#include <ZBPrint.h>            // Thư viện in ấn cho Zerobase

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Tạo đối tượng LCD (địa chỉ 0x27, 16 cột, 2 dòng)

int temperature = 0;                   // Biến lưu giá trị nhiệt độ nhận được
char dateTime[17] = "00/00/00 00:00";  // Biến lưu chuỗi ngày giờ, định dạng: DD/MM/YY HH:MM (cần đủ chỗ cho null terminator)

/**
 * @brief Hàm callback được gọi khi nhận được tin nhắn CAN.
 * Hàm này xử lý dữ liệu nhiệt độ và thời gian đến từ các node khác.
 */
void canMessageReceived() {
  uint32_t rxId = ZBCan1.getRxId();  // Lấy ID của node đã gửi tin nhắn

  if (rxId == 0x2) {                           // Nếu tin nhắn từ node nhiệt độ (ID 0x2)
    if (ZBCan1.isInt()) {                      // Kiểm tra nếu tin nhắn là kiểu số nguyên
      temperature = ZBCan1.getLastIntValue();  // Lấy giá trị nhiệt độ
      Serial.print("Temperature received: ");  // In thông báo nhận nhiệt độ
      Serial.print(temperature);
      Serial.println("°C");
    }
  } else if (rxId == 0x3) {                                                    // Nếu tin nhắn từ node thời gian (ID 0x3)
    if (ZBCan1.isString()) {                                                   // Kiểm tra nếu tin nhắn là kiểu chuỗi
      strncpy(dateTime, ZBCan1.getLastStringMessage(), sizeof(dateTime) - 1);  // Sao chép chuỗi thời gian nhận được (an toàn hơn với strncpy)
      dateTime[sizeof(dateTime) - 1] = '\0';                                   // Đảm bảo chuỗi kết thúc null
      Serial.print("DateTime received: ");                                     // In thông báo nhận thời gian
      Serial.println(dateTime);
    }
  }
}

/**
 * @brief Cập nhật màn hình LCD với ngày/giờ và nhiệt độ hiện tại.
 */
void updateDisplay() {
  lcd.clear();  // Xóa màn hình LCD

  lcd.setCursor(0, 0);  // Đặt vị trí con trỏ (dòng 1, cột 1)
  lcd.print(dateTime);  // Hiển thị ngày giờ ở dòng 1

  lcd.setCursor(3, 1);     // Đặt vị trí con trỏ (dòng 2, cột 4)
  lcd.print("Temp: ");     // Hiển thị chữ "Temp: "
  lcd.print(temperature);  // Hiển thị giá trị nhiệt độ
  lcd.print(" C");         // Hiển thị đơn vị độ C
}

void setup() {
  Serial.begin(115200);                     // Khởi tạo giao tiếp Serial với tốc độ 115200 baud
  Serial.println("Display Node Starting");  // In thông báo khởi động

  Wire.begin();                  // Khởi tạo giao tiếp I2C
  lcd.begin(16, 2);              // Khởi tạo LCD 16x2
  lcd.backlight();               // Bật đèn nền LCD
  lcd.clear();                   // Xóa màn hình LCD
  lcd.print("Initializing...");  // Hiển thị thông báo đang khởi tạo

  ZBCan1.begin();  // Khởi tạo giao tiếp CAN

  ZBCan1.setTxId(0x1);  // Đặt ID của node này là 0x1 khi gửi tin nhắn

  // Chỉ chấp nhận tin nhắn từ node nhiệt độ (ID 0x2) và node thời gian (ID 0x3)
  uint32_t acceptedIds[] = { 0x2, 0x3 };
  ZBCan1.configureMultipleFilters(acceptedIds, 2);  // Cấu hình bộ lọc CAN

  // Thiết lập hàm callback để xử lý khi có tin nhắn CAN đến.
  ZBCan1.onReceive(canMessageReceived);
}

void loop() {
  // Gửi yêu cầu cập nhật thời gian đến node RTC (TxId 0x8)
  ZBCan1.send("GET_TIME", 0x8);
  Serial.println("Requesting time update");

  // Gửi yêu cầu cập nhật nhiệt độ đến node nhiệt độ (TxId 0x7)
  ZBCan1.send("GET_TEMP", 0x7);
  Serial.println("Requesting temperature update");

  updateDisplay();  // Cập nhật màn hình LCD với dữ liệu nhận được

  delay(1000);  // Tạm dừng 1000ms (1 giây) trước khi lặp lại vòng lặp
}