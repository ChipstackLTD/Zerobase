/*
* Ví dụ về Gửi và Nhận dữ liệu qua SPI
*
* Mô tả: Chương trình này minh họa giao tiếp SPI giữa Zerobase 2 (Master) và Arduino Mega (Slave).
* Nó gửi một chuỗi ("Hello World") và đồng thời nhận lại dữ liệu từ Slave.
* Do sự khác biệt về mức logic (Zerobase 2 dùng 3.3V, Arduino Mega dùng 5V), bạn cần một Bộ chuyển đổi mức logic (Logic Level Shifter)
* để đảm bảo tín hiệu tương thích.
*/

// Sơ đồ kết nối với Logic Level Shifter:
// Zerobase 2 (phía 3.3V của Logic Level Shifter)
// ss (Slave Select - thường là D10 hoặc tùy cấu hình) --> LV1 (Level Shifter Low Voltage 1)
// mo (Master Out, Slave In - MOSI) --> LV2
// 3.3V --> LV (cấp nguồn 3.3V cho phía LV của Level Shifter)
// gnd --> GND (nối đất)
// mi (Master In, Slave Out - MISO) --> LV3
// sck (Serial Clock - SCK) --> LV4

// Arduino Mega (phía 5V của Logic Level Shifter)
// ss (Slave Select - thường là D53 trên Mega) --> HV1 (Level Shifter High Voltage 1)
// mo (Master Out, Slave In - MOSI - thường là D51 trên Mega) --> HV2
// 5V --> HV (cấp nguồn 5V cho phía HV của Level Shifter)
// gnd --> GND (nối đất)
// mi (Master In, Slave Out - MISO - thường là D50 trên Mega) --> HV3
// sck (Serial Clock - SCK - thường là D52 trên Mega) --> HV4

#include <ZBPrint.h>  // Bao gồm thư viện ZBPrint, thường được sử dụng cho các chức năng in ấn.
#include <SPI.h>      // Bao gồm thư viện SPI để sử dụng các chức năng giao tiếp SPI.

#define SS 10  // Định nghĩa chân Slave Select (SS) là chân số 10 cho Zerobase 2.

void setup() {
  // Cấu hình chân SS là OUTPUT.
  pinMode(SS, OUTPUT);
  // Đặt chân SS ở trạng thái CAO (HIGH) để bỏ chọn Slave ban đầu.
  digitalWrite(SS, HIGH);

  // Khởi tạo giao tiếp SPI. Zerobase 2 sẽ hoạt động như Master.
  SPI.begin();
  // Khởi tạo giao tiếp Serial ở tốc độ 9600 baud để gỡ lỗi và hiển thị thông tin.
  Serial.begin(9600);
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
  Serial.print("String sent: ");
  Serial.println(sentString);

  Serial.print("String received: ");
  Serial.println(receivedString);
  Serial.println();  // In một dòng trống để phân tách các lần in.

  delay(1000);  // Dừng 1 giây trước khi lặp lại quá trình gửi/nhận.
}