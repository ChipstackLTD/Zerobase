/*
* Ví dụ về Gửi và Nhận dữ liệu qua SPI
*
* Mô tả: Chương trình này minh họa giao tiếp SPI giữa Zerobase (Master) và Arduino Mega (Slave).
* Nó gửi một chuỗi ("Hello World") và đồng thời nhận lại dữ liệu từ Slave.
* Do sự khác biệt về mức logic (Zerobase dùng 3.3V, Arduino Mega dùng 5V), bạn cần một Bộ chuyển đổi mức logic (Logic Level Shifter)
* để đảm bảo tín hiệu tương thích.
*/

// Không bao gồm thư viện ZBPrint theo yêu cầu.
#include <SPI.h>  // Bao gồm thư viện SPI để sử dụng các chức năng giao tiếp SPI.

#define SS 10  // Định nghĩa chân Slave Select (SS) là chân số 10 cho Zerobase.

void setup() {
  // Cấu hình chân SS là OUTPUT.
  pinMode(SS, OUTPUT);
  // Đặt chân SS ở trạng thái CAO (HIGH) để bỏ chọn Slave ban đầu.
  digitalWrite(SS, HIGH);

  // Khởi tạo giao tiếp SPI. Zerobase sẽ hoạt động như Master.
  SPI.begin();
  // Khởi tạo giao tiếp Serial1 ở tốc độ 9600 baud để gỡ lỗi và hiển thị thông tin.
  Serial1.begin(9600);
}

void loop() {
  const char* sentString = "Hello World";  // Chuỗi Master sẽ gửi (11 ký tự).
  char receivedString[12] = { 0 };         // Mảng ký tự để lưu chuỗi nhận được (+1 cho ký tự null kết thúc chuỗi).

  // Bắt đầu một giao dịch SPI với các thiết lập:
  // - Tốc độ xung nhịp: 1,000,000 Hz (1 MHz)
  // - Thứ tự bit: MSBFIRST (bit có trọng số lớn nhất được gửi trước)
  // - Chế độ SPI: SPI_MODE0 (CPOL=0, CPHA=0)
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(SS, LOW);   // Chọn Slave bằng cách kéo chân SS xuống THẤP (LOW).
  delayMicroseconds(100);  // Dừng một chút để Slave có thời gian chuẩn bị sau khi được chọn.

  // Lặp qua từng ký tự của chuỗi để gửi và nhận đồng thời.
  for (int i = 0; i < 11; i++) {
    // Hàm SPI.transfer() gửi một byte và đồng thời nhận một byte.
    // Dữ liệu nhận được sẽ được lưu vào mảng receivedString.
    receivedString[i] = SPI.transfer(sentString[i]);
    delayMicroseconds(200);  // Dừng một chút để Slave có thời gian xử lý byte đã nhận/gửi.
  }

  digitalWrite(SS, HIGH);  // Bỏ chọn Slave bằng cách kéo chân SS lên CAO (HIGH).
  SPI.endTransaction();    // Kết thúc giao dịch SPI.

  receivedString[11] = '\0';  // Thêm ký tự null vào cuối chuỗi nhận được để nó là một chuỗi C hợp lệ.

  // In thông tin gỡ lỗi ra Serial Monitor.
  Serial1.print("String sent: ");
  Serial1.println(sentString);

  Serial1.print("String received: ");
  Serial1.println(receivedString);
  Serial1.println();  // In một dòng trống để phân tách các lần in.

  delay(1000);  // Dừng 1 giây trước khi lặp lại quá trình gửi/nhận.
}