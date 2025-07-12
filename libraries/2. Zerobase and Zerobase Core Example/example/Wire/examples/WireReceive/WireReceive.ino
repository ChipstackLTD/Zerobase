/*
* Ví dụ về nhận dữ liệu qua I2C (Inter-Integrated Circuit) - Chế độ Slave
*
* Mô tả: Chương trình này cấu hình board ZeroBase/Arduino hoạt động như một thiết bị I2C Slave
* với địa chỉ 8. Khi thiết bị Master gửi dữ liệu đến Slave, Slave sẽ nhận dữ liệu
* và in các ký tự nhận được ra Serial Monitor.
*
*/

// Không bao gồm thư viện ZBPrint theo yêu cầu.
#include <Wire.h>  // Bao gồm thư viện Wire để sử dụng giao tiếp I2C (còn gọi là TWI - Two Wire Interface).

void setup() {
  // Khởi tạo giao tiếp I2C dưới dạng thiết bị Slave với địa chỉ là 8.
  // Địa chỉ này phải khớp với địa chỉ mà Master gửi đến.
  Wire.begin(8);

  // Đăng ký hàm 'receiveEvent' để được gọi mỗi khi Slave nhận được dữ liệu từ Master.
  Wire.onReceive(receiveEvent);

  // Khởi tạo giao tiếp Serial1 ở tốc độ 9600 baud để in dữ liệu nhận được ra Serial Monitor.
  Serial1.begin(9600);
}

void loop() {
  // Trong chế độ Slave, hàm loop() thường không làm gì cả.
  // Mọi hoạt động nhận dữ liệu được xử lý bởi hàm ngắt 'receiveEvent'.
  // Chương trình chờ đợi dữ liệu từ Master.
}

// Hàm receiveEvent được gọi tự động khi Slave nhận được dữ liệu từ Master.
// Tham số 'howMany' cho biết số byte dữ liệu đã nhận được.
void receiveEvent(int howMany) {
  Serial1.print("Received: ");  // In ra thông báo "Received: "

  // Đọc tất cả các byte có sẵn trong bộ đệm nhận của I2C.
  while (Wire.available()) {
    char c = Wire.read();  // Đọc từng byte dữ liệu đến và ép kiểu thành ký tự.
    Serial1.print(c);      // In ký tự đó ra Serial Monitor.
  }
  Serial1.println();  // Xuống dòng sau khi in hết dữ liệu của một lần nhận.
}