/*******************************************************************************
* Node đồng hồ thời gian thực với DS3231
*
* Mô tả: Chương trình này đọc thời gian và ngày hiện tại từ module RTC DS3231
* và gửi nó qua giao tiếp CAN bus khi nhận được yêu cầu từ node khác
*
* Link bài viết hướng dẫn chi tiết: [THÊM_LINK_BÀI_VIẾT_CỦA_BẠN_VỀ_MODULE_RTC_CAN]
*******************************************************************************/

#include <Arduino.h>  // Thư viện cơ bản của Arduino
#include <ZBCan.h>    // Thư viện giao tiếp CAN trên Zerobase
#include <Wire.h>     // Thư viện giao tiếp I2C
#include <DS3231.h>   // Thư viện giao tiếp với module RTC DS3231
#include <ZBPrint.h>  // Thư viện in ấn cho Zerobase

DS3231 rtc;  // Khởi tạo đối tượng RTC DS3231

bool timeIsSet = false;  // Biến đánh dấu thời gian đã được cài đặt chưa (chỉ cài đặt một lần)

char dateTimeStr[17];  // Buffer lưu chuỗi ngày/giờ (ví dụ: DD/MM/YY HH:MM:SS\0)

bool timeRequested = false;  // Cờ đánh dấu có yêu cầu đọc thời gian từ node khác

/**
 * @brief Hàm callback được gọi khi nhận được tin nhắn CAN.
 * Hàm này kiểm tra ID và nội dung tin nhắn đến để xác định xem đó có phải
 * là yêu cầu đọc thời gian từ node chính hay không.
 */
void canMessageReceived() {
  uint32_t rxId = ZBCan1.getRxId();  // Lấy ID của node gửi tin nhắn

  // Kiểm tra nếu tin nhắn từ node chính (ID 0x8) và là dạng chuỗi
  if (rxId == 0x8 && ZBCan1.isString()) {
    String message = ZBCan1.getLastStringMessage();  // Lấy nội dung chuỗi tin nhắn
    Serial.print("Message received: ");              // In ra Serial để debug
    Serial.println(message);                         // In nội dung tin nhắn

    if (message == "GET_TIME") {  // Nếu tin nhắn yêu cầu thời gian
      timeRequested = true;       // Đặt cờ yêu cầu thời gian thành true
    }
  }
}

/**
 * @brief Đọc ngày giờ hiện tại từ module RTC và định dạng nó thành chuỗi.
 * Chuỗi có định dạng "DD/MM/YY HH:MM:SS".
 */
void readDateTime() {
  bool h12, PM, century;  // Các biến cần thiết cho các hàm đọc RTC

  // Đọc các thành phần thời gian
  byte month = rtc.getMonth(century);  // Đọc tháng
  byte day = rtc.getDate();            // Đọc ngày
  byte year = rtc.getYear();           // Đọc năm (chỉ 2 số cuối)
  byte hour = rtc.getHour(h12, PM);    // Đọc giờ
  byte minute = rtc.getMinute();       // Đọc phút
  byte second = rtc.getSecond();       // Đọc giây

  // Định dạng chuỗi ngày/giờ: DD/MM/YY HH:MM:SS
  sprintf(dateTimeStr, "%02d/%02d/%02d %02d:%02d:%02d",
          day, month, year, hour, minute, second);
}

void setup() {
  Serial.begin(115200);                 // Khởi tạo giao tiếp Serial với tốc độ 115200 baud
  Serial.println("RTC Node Starting");  // In thông báo khởi động

  Wire.begin();  // Khởi tạo giao tiếp I2C cho RTC

  // Chỉ cài đặt thời gian một lần khi khởi động lần đầu
  if (!timeIsSet) {
    // Cài đặt thời gian ban đầu (ví dụ: ngày 29 tháng 05 năm 2025, Thứ 5, 10:50:00)
    rtc.setYear(25);    // Đặt năm: 2025 (chỉ 2 số cuối)
    rtc.setMonth(5);    // Đặt tháng: tháng 5
    rtc.setDate(29);    // Đặt ngày: ngày 29
    rtc.setDoW(5);      // Đặt thứ: Thứ 5 (1=Chủ Nhật, 2=Thứ 2...)
    rtc.setHour(10);    // Đặt giờ: 10 giờ
    rtc.setMinute(50);  // Đặt phút: 50 phút
    rtc.setSecond(0);   // Đặt giây: 00 giây

    rtc.setClockMode(false);  // Đặt chế độ 24 giờ (false = 24h, true = 12h)

    timeIsSet = true;                         // Đánh dấu đã cài đặt thời gian
    Serial.println("Time set successfully");  // Thông báo cài đặt thành công
  }

  readDateTime();  // Đọc thời gian lần đầu tiên

  ZBCan1.begin();  // Khởi tạo giao tiếp CAN

  ZBCan1.setTxId(0x3);  // Đặt ID của node này là 0x3 khi gửi tin nhắn

  // Định nghĩa các ID CAN được chấp nhận cho tin nhắn đến:
  // 0x1: Tin nhắn phát sóng chung từ node chính
  // 0x8: Yêu cầu thời gian cụ thể từ node chính
  uint32_t acceptedIds[] = { 0x1, 0x8 };
  ZBCan1.configureMultipleFilters(acceptedIds, 2);  // Cấu hình bộ lọc CAN

  // Thiết lập hàm callback để xử lý khi có tin nhắn CAN đến.
  ZBCan1.onReceive(canMessageReceived);

  Serial.println("RTC Node Ready");  // In thông báo sẵn sàng
}

void loop() {
  readDateTime();  // Đọc thời gian hiện tại

  if (timeRequested) {            // Kiểm tra nếu có yêu cầu thời gian
    ZBCan1.send(dateTimeStr);     // Gửi chuỗi thời gian qua CAN
    Serial.print("Time sent: ");  // In thông báo đã gửi thời gian
    Serial.println(dateTimeStr);  // In chuỗi thời gian đã gửi
    timeRequested = false;        // Đặt lại cờ yêu cầu
  }

  delay(1000);  // Tạm dừng 1000ms (1 giây) để ổn định
}