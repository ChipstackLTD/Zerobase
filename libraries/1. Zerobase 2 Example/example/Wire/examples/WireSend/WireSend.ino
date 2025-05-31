/*
* Ví dụ về gửi dữ liệu qua I2C (Inter-Integrated Circuit) - Chế độ Master
*
* Mô tả: Chương trình này cấu hình board ZeroBase/Arduino hoạt động như một thiết bị I2C Master.
* Nó sẽ gửi một chuỗi văn bản ("Sending data...") đến một thiết bị I2C Slave
* (ví dụ: một board Arduino khác được cấu hình làm Slave với địa chỉ 8) mỗi giây.
*
* Sơ đồ kết nối:
* *** LƯU Ý: Nếu bạn sử dụng ZeroBase 2, bạn cần một bộ chuyển đổi mức logic (logic shifter)
* để chuyển đổi điện áp từ 3.3V (ZeroBase 2) sang 5V (Board Arduino) ***
* Board ZeroBase (phiên bản gốc) không cần bộ chuyển đổi mức logic.
*
* Cấu hình I2C trên ZeroBase 2:
* - Nếu bạn sử dụng I2C1 (chân D3 - SDA, D2 - SCL) --> Sử dụng Wire1.begin();
* - Nếu bạn sử dụng I2C2 (chân D18 - SDA, D19 - SCL) --> Sử dụng Wire.begin();
*
* Sơ đồ kết nối với Logic Shifter (khi dùng ZeroBase 2 với board 5V):
* ZeroBase 2       Logic Shifter       Board Arduino (hoặc thiết bị 5V khác)
* --------         -------------       ----------------------------------
* 3.3V     ------> LV (Low Voltage)
* GND      ------> GND
* SDA (D18/D3) --> LV1             ------> HV1 (High Voltage) --> SDA (A4)
* SCL (D19/D2) --> LV2             ------> HV2 (High Voltage) --> SCL (A5)
* 5V      ------> HV (High Voltage)
* GND     ------> GND
*/

#include <ZBPrint.h> // Bao gồm thư viện ZBPrint, thường được sử dụng cho các chức năng in ấn.
#include <Wire.h>    // Bao gồm thư viện Wire để sử dụng giao tiếp I2C (còn gọi là TWI - Two Wire Interface).

void setup() {
  Wire.begin();           // Khởi tạo giao tiếp I2C dưới dạng thiết bị Master.
  Wire.setClock(100000);  // Đặt tốc độ xung nhịp I2C ở chế độ tiêu chuẩn (Standard mode) là 100 kHz.
  Serial.begin(9600);     // Khởi tạo giao tiếp Serial ở tốc độ 9600 baud để in thông báo.
}

void loop() {
  Serial.println("Sending data to Arduino..."); // In thông báo lên Serial Monitor.

  Wire.beginTransmission(8);    // Bắt đầu truyền dữ liệu đến thiết bị Slave có địa chỉ là 8.
                                // (Ví dụ: một board Arduino Mega được cấu hình làm Slave).
  Wire.write("Sending data..."); // Ghi chuỗi dữ liệu "Sending data..." vào bộ đệm truyền.
  Wire.endTransmission();       // Kết thúc quá trình truyền. Dữ liệu trong bộ đệm sẽ được gửi đi.

  delay(1000); // Dừng chương trình 1 giây trước khi gửi lại dữ liệu.
}