/*
* Ví dụ về gửi dữ liệu qua SPI (Serial Peripheral Interface)
*
* Mô tả: Chương trình này minh họa cách gửi một chuỗi văn bản ("Hello World")
* từ một thiết bị Master (chẳng hạn như Arduino hoặc ESP32) đến một thiết bị Slave
* thông qua giao tiếp SPI. Nó gửi độ dài của thông điệp trước, sau đó là từng ký tự.
*/

#include <ZBPrint.h>  // Bao gồm thư viện ZBPrint, thường được sử dụng cho các chức năng in ấn.

#include <SPI.h>  // Bao gồm thư viện SPI để sử dụng các chức năng giao tiếp SPI.

#define SS 10  // Định nghĩa chân Slave Select (SS) là chân số 10. Chân này được sử dụng để chọn thiết bị Slave.

void setup() {
  // Cấu hình chân SS là OUTPUT và đặt nó ở trạng thái CAO (HIGH).
  // Khi SS ở HIGH, thiết bị Slave không được chọn.
  pinMode(SS, OUTPUT);
  digitalWrite(SS, HIGH);

  // Khởi tạo giao tiếp SPI.
  // Mặc định, thiết bị được cấu hình là Master.
  SPI.begin();
  // Khởi tạo giao tiếp Serial ở tốc độ 9600 baud để gửi thông báo debug.
  Serial.begin(9600);
}

void loop() {
  const char* message = "Hello World";  // Chuỗi thông điệp cần gửi.
  uint8_t length = strlen(message);     // Lấy độ dài của chuỗi thông điệp.

  // Bắt đầu một giao dịch SPI.
  // SPISettings:
  //   1000000: Tốc độ xung nhịp SPI (1 MHz).
  //   MSBFIRST: Dữ liệu được gửi với bit có trọng số lớn nhất trước (Most Significant Bit First).
  //   SPI_MODE0: Chế độ SPI (CPOL=0, CPHA=0).
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  digitalWrite(SS, LOW);  // Chọn thiết bị Slave bằng cách kéo chân SS xuống THẤP (LOW).
                          // Điều này báo hiệu cho Slave rằng Master sắp gửi dữ liệu.

  // Gửi độ dài của thông điệp đến Slave.
  SPI.transfer(length);

  // Gửi từng ký tự của thông điệp.
  for (uint8_t i = 0; i < length; i++) {
    SPI.transfer(message[i]);  // Gửi một ký tự qua SPI.
  }

  digitalWrite(SS, HIGH);  // Bỏ chọn thiết bị Slave bằng cách kéo chân SS lên CAO (HIGH).
                           // Điều này báo hiệu cho Slave rằng giao dịch đã kết thúc.

  SPI.endTransaction();  // Kết thúc giao dịch SPI.

  Serial.println("Message sent: Hello World");  // In thông báo xác nhận lên Serial Monitor.

  delay(1000);  // Dừng chương trình 1 giây trước khi gửi lại thông điệp.
}