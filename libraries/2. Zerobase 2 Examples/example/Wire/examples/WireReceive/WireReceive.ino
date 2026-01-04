/*
* Ví dụ về nhận dữ liệu qua I2C (Inter-Integrated Circuit) - Chế độ Slave
*
* Mô tả: Chương trình này cấu hình board ZeroBase/Arduino hoạt động như một thiết bị I2C Slave
* với địa chỉ 8. Khi thiết bị Master gửi dữ liệu đến Slave, Slave sẽ nhận dữ liệu
* và in các ký tự nhận được ra Serial Monitor.
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

#include <ZBPrint.h>  // Bao gồm thư viện ZBPrint, thường được sử dụng cho các chức năng in ấn.
#include <Wire.h>     // Bao gồm thư viện Wire để sử dụng giao tiếp I2C (còn gọi là TWI - Two Wire Interface).

void setup() {
  // Khởi tạo giao tiếp I2C dưới dạng thiết bị Slave với địa chỉ là 8.
  // Địa chỉ này phải khớp với địa chỉ mà Master gửi đến.
  Wire.begin(8);

  // Đăng ký hàm 'receiveEvent' để được gọi mỗi khi Slave nhận được dữ liệu từ Master.
  Wire.onReceive(receiveEvent);

  // Khởi tạo giao tiếp Serial ở tốc độ 9600 baud để in dữ liệu nhận được ra Serial Monitor.
  Serial.begin(9600);
}

void loop() {
  // Trong chế độ Slave, hàm loop() thường không làm gì cả.
  // Mọi hoạt động nhận dữ liệu được xử lý bởi hàm ngắt 'receiveEvent'.
  // Chương trình chờ đợi dữ liệu từ Master.
}

// Hàm receiveEvent được gọi tự động khi Slave nhận được dữ liệu từ Master.
// Tham số 'howMany' cho biết số byte dữ liệu đã nhận được.
void receiveEvent(int howMany) {
  Serial.print("Received: ");  // In ra thông báo "Received: "

  // Đọc tất cả các byte có sẵn trong bộ đệm nhận của I2C.
  while (Wire.available()) {
    char c = Wire.read();  // Đọc từng byte dữ liệu đến và ép kiểu thành ký tự.
    Serial.print(c);       // In ký tự đó ra Serial Monitor.
  }
  Serial.println();  // Xuống dòng sau khi in hết dữ liệu của một lần nhận.
}