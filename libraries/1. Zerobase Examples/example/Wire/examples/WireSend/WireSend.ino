/*
* Ví dụ về gửi dữ liệu qua I2C (Inter-Integrated Circuit) - Chế độ Master
*
* Mô tả: Chương trình này cấu hình board ZeroBase/Arduino hoạt động như một thiết bị I2C Master.
* Nó sẽ gửi một chuỗi văn bản ("Sending data...") đến một thiết bị I2C Slave
* (ví dụ: một board Arduino khác được cấu hình làm Slave với địa chỉ 8) mỗi giây.
*
*/

// Không bao gồm thư viện ZBPrint theo yêu cầu.
#include <Wire.h>  // Bao gồm thư viện Wire để sử dụng giao tiếp I2C (còn gọi là TWI - Two Wire Interface).

void setup() {
  Wire.begin();           // Khởi tạo giao tiếp I2C dưới dạng thiết bị Master.
  Wire.setClock(100000);  // Đặt tốc độ xung nhịp I2C ở chế độ tiêu chuẩn (Standard mode) là 100 kHz.
  Serial1.begin(9600);    // Khởi tạo giao tiếp Serial1 ở tốc độ 9600 baud để in thông báo.
}

void loop() {
  Serial1.println("Sending data to Arduino...");  // In thông báo lên Serial Monitor.

  Wire.beginTransmission(8);      // Bắt đầu truyền dữ liệu đến thiết bị Slave có địa chỉ là 8.
                                  // (Ví dụ: một board Arduino Mega được cấu hình làm Slave).
  Wire.write("Sending data...");  // Ghi chuỗi dữ liệu "Sending data..." vào bộ đệm truyền.
  Wire.endTransmission();         // Kết thúc quá trình truyền. Dữ liệu trong bộ đệm sẽ được gửi đi.

  delay(1000);  // Dừng chương trình 1 giây trước khi gửi lại dữ liệu.
}