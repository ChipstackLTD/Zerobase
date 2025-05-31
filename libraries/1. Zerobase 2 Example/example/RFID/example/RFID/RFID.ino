/* Điều khiển LED bằng Thẻ RFID trên Zerobase 2
 *
 * Mô tả: Chương trình này cho phép bạn điều khiển trạng thái bật/tắt của một đèn LED
 * bằng cách sử dụng module đọc thẻ RFID RC522. Khi một thẻ RFID được quét, chương trình
 * sẽ đọc UID (Unique Identifier) của thẻ. Nếu UID của thẻ khớp với một ID đã được định nghĩa
 * để bật đèn, LED sẽ bật sáng. Nếu khớp với một ID khác để tắt đèn, LED sẽ tắt.
 * Đây là một ví dụ cơ bản về ứng dụng điều khiển truy cập hoặc tự động hóa bằng RFID.
 *
 * Lưu ý: Bạn cần thay đổi giá trị của mảng ID1 và ID2 bằng UID thực của các thẻ RFID
 * mà bạn muốn sử dụng để điều khiển LED.
 *
 * Link bài viết hướng dẫn chi tiết: [THÊM_LINK_BÀI_VIẾT_CỦA_BẠN_VỀ_ĐIỀU_KHIỂN_LED_BẰNG_RFID_ZEROBASÉ]
 */

// Thêm thư viện SPI để giao tiếp với module RFID qua giao thức SPI
#include <SPI.h>
// Thêm thư viện MFRC522 để điều khiển module đọc thẻ RFID RC522
#include <MFRC522.h>
// Thêm thư viện ZBPrint để in ra Serial Monitor
#include <ZBPrint.h>

// Định nghĩa chân Reset cho module RFID RC522
#define RST_PIN A0
// Định nghĩa chân SS (Slave Select) cho module RFID RC522
#define SS_PIN 10
// Định nghĩa chân LED
#define LED_PIN D2

// Khai báo mảng lưu giá trị UID của thẻ đọc được, có 4 byte
int UID[4], i;

// Khai báo mảng ID1 với giá trị UID cụ thể - thẻ được sử dụng để bật đèn
// Lưu ý: Thay giá trị này bằng UID thực của thẻ bạn muốn sử dụng để bật đèn
int ID1[4] = { 243, 248, 222, 41 };  //Thẻ mở đèn
// Khai báo mảng ID2 với giá trị UID cụ thể - thẻ được sử dụng để tắt đèn
// Lưu ý: Thay giá trị này bằng UID thực của thẻ bạn muốn sử dụng để tắt đèn
int ID2[4] = { 225, 78, 243, 123 };  //Thẻ tắt đèn

// Khởi tạo đối tượng mfrc522 từ lớp MFRC522 với các tham số SS_PIN và RST_PIN
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Hàm kiểm tra xem UID đọc được có khớp với ID được lưu hay không
bool compareUID(int uid[], int id[]) {
  for (byte i = 0; i < 4; i++) {
    if (uid[i] != id[i]) {
      return false;  // Nếu có bất kỳ byte nào không khớp, trả về false
    }
  }
  return true;  // Nếu tất cả các byte đều khớp, trả về true
}

// Hàm setup() - chạy một lần khi Arduino khởi động
void setup() {
  // Khởi tạo giao tiếp Serial với tốc độ baud 9600 để hiển thị thông tin
  Serial.begin(9600);
  while (!Serial) delay(10);  // Chờ kết nối Serial được thiết lập

  Serial.println("Hệ thống điều khiển LED bằng thẻ RFID");
  Serial.println("---------------------------------------");

  // Cấu hình chân LED là OUTPUT để điều khiển đèn LED
  pinMode(LED_PIN, OUTPUT);
  // Tắt đèn LED khi khởi động (mức LOW)
  digitalWrite(LED_PIN, LOW);

  // Khởi tạo giao tiếp SPI
  SPI.begin();
  // Khởi tạo module RFID RC522
  mfrc522.PCD_Init();

  Serial.println("Đã khởi tạo module RFID RC522");
  Serial.println("Vui lòng quét thẻ để điều khiển LED");
}

// Hàm loop() - chạy lặp lại liên tục sau khi setup() hoàn thành
void loop() {
  // Kiểm tra xem có thẻ mới xuất hiện hay không
  // Nếu không có thẻ mới, thoát khỏi vòng lặp hiện tại
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Đọc dữ liệu từ thẻ
  // Nếu không đọc được, thoát khỏi vòng lặp hiện tại
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // In dòng chữ "UID của thẻ: " ra Serial Monitor
  Serial.print("UID của thẻ: ");

  // Vòng lặp để đọc và hiển thị từng byte của UID thẻ
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    // Thêm khoảng trắng và số 0 đứng trước nếu giá trị byte nhỏ hơn 0x10 (16)
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    // Lưu giá trị byte UID vào mảng UID
    UID[i] = mfrc522.uid.uidByte[i];
    // In giá trị byte ra Serial Monitor
    Serial.print(UID[i]);
  }

  // Xuống dòng sau khi in xong UID
  Serial.println("   ");

  // So sánh UID đọc được với ID1 (thẻ mở đèn)
  if (compareUID(UID, ID1)) {
    // Nếu trùng với ID1, bật đèn LED
    digitalWrite(LED_PIN, HIGH);
    // In thông báo ra Serial Monitor
    Serial.println("Thẻ mở đèn - ĐÈN ON");
  }
  // So sánh UID đọc được với ID2 (thẻ tắt đèn)
  else if (compareUID(UID, ID2)) {
    // Nếu trùng với ID2, tắt đèn LED
    digitalWrite(LED_PIN, LOW);
    // In thông báo ra Serial Monitor
    Serial.println("Thẻ tắt đèn - ĐÈN OFF");
  }
  // Trường hợp UID không trùng với ID1 và ID2
  else {
    // In thông báo ra Serial Monitor
    Serial.println("Sai thẻ - Không thực hiện hành động");
  }

  // Dừng giao tiếp với thẻ hiện tại
  mfrc522.PICC_HaltA();
  // Dừng mã hóa để chuẩn bị cho lần đọc thẻ tiếp theo
  mfrc522.PCD_StopCrypto1();

  // Chờ một khoảng thời gian ngắn trước khi đọc thẻ tiếp theo
  delay(1000);
}