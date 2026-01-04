/*
 * Đọc/Ghi dữ liệu EEPROM I2C (24Cxx)
 *
 * Mô tả: Chương trình này trình bày cách đọc và ghi các loại dữ liệu khác nhau
 * (byte, int, long, String) vào bộ nhớ EEPROM ngoài thông qua giao tiếp I2C.
 * Nó cũng bao gồm các chức năng kiểm tra kết nối, xóa vùng nhớ, và các lưu ý
 * quan trọng khi sử dụng EEPROM với Zerobase.
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=162
 */

/*
 * ╔════════════════════════════════════════════════════════╗
 * ║ BẢNG THÔNG TIN IC EEPROM                               ║
 * ╠═══════════════╦═══════════════════╦═══════════════════╣
 * ║ Tên IC        ║ Dung lượng        ║ Vùng địa chỉ      ║
 * ╠═══════════════╬═══════════════════╬═══════════════════╣
 * ║ 24C01         ║ 128 bytes         ║ 0 -> 127          ║
 * ║ 24C02         ║ 256 bytes         ║ 0 -> 255          ║
 * ║ 24C04         ║ 512 bytes         ║ 0 -> 511          ║
 * ║ 24C08         ║ 1024 bytes        ║ 0 -> 1023         ║
 * ║ 24C16         ║ 2 KB              ║ 0 -> 2047         ║
 * ║ 24C32         ║ 4 KB              ║ 0 -> 4095         ║
 * ║ 24C64         ║ 8 KB              ║ 0 -> 8191         ║
 * ║ 24C128        ║ 16 KB             ║ 0 -> 16383        ║
 * ║ 24C256        ║ 32 KB             ║ 0 -> 32767        ║
 * ║ 24C512        ║ 64,5 KB           ║ 0 -> 65535        ║
 * ╚═══════════════╩═══════════════════╩═══════════════════╝
 */

/*
 * ╔════════════════════════════════════════════════════════════════════════════╗
 * ║ LƯU Ý KHI SỬ DỤNG EEPROM                                                     ║
 * ╠════════════════════════════════════════════════════════════════════════════╣
 * ║ 1. Chỉ ghi vào EEPROM các số nguyên dương                                    ║
 * ║                                                                              ║
 * ║ 2. Cẩn thận tránh ghi đè lên các ô nhớ đã sử dụng:                          ║
 * ║    VD: biến b kiểu unsigned int (2 byte) ghi vào địa chỉ 56                 ║
 * ║        => b chiếm ô nhớ 56 và 57                                             ║
 * ║        Nếu ghi biến M (2 byte) vào địa chỉ 57                                ║
 * ║        => M sẽ chiếm ô 57 và 58, gây đè lên 1 phần của biến b                ║
 * ╚════════════════════════════════════════════════════════════════════════════╝
 */

/*
 * ╔════════════════════════════════════════════════════════╗
 * ║ BẢNG THÔNG TIN IC EEPROM                              ║
 * ╠═══════════════╦═══════════════════╦═══════════════════╣
 * ║ Tên IC        ║ Dung lượng        ║ Vùng địa chỉ      ║
 * ╠═══════════════╬═══════════════════╬═══════════════════╣
 * ║ 24C01         ║ 128 bytes         ║ 0 -> 127          ║
 * ║ 24C02         ║ 256 bytes         ║ 0 -> 255          ║
 * ║ 24C04         ║ 512 bytes         ║ 0 -> 511          ║
 * ║ 24C08         ║ 1024 bytes        ║ 0 -> 1023         ║
 * ║ 24C16         ║ 2 KB              ║ 0 -> 2047         ║
 * ║ 24C32         ║ 4 KB              ║ 0 -> 4095         ║
 * ║ 24C64         ║ 8 KB              ║ 0 -> 8191         ║
 * ║ 24C128        ║ 16 KB             ║ 0 -> 16383        ║
 * ║ 24C256        ║ 32 KB             ║ 0 -> 32767        ║
 * ║ 24C512        ║ 64,5 KB           ║ 0 -> 65535        ║
 * ╚═══════════════╩═══════════════════╩═══════════════════╝
 */

/*
 * ╔════════════════════════════════════════════════════════════════════════════╗
 * ║ LƯU Ý KHI SỬ DỤNG EEPROM                                                  ║
 * ╠════════════════════════════════════════════════════════════════════════════╣
 * ║ 1. Chỉ ghi vào EEPROM các số nguyên dương                                 ║
 * ║                                                                            ║
 * ║ 2. Cẩn thận tránh ghi đè lên các ô nhớ đã sử dụng:                        ║
 * ║    VD: biến b kiểu unsigned int (2 byte) ghi vào địa chỉ 56               ║
 * ║        => b chiếm ô nhớ 56 và 57                                          ║
 * ║        Nếu ghi biến M (2 byte) vào địa chỉ 57                             ║
 * ║        => M sẽ chiếm ô 57 và 58, gây đè lên 1 phần của biến b             ║
 * ╚════════════════════════════════════════════════════════════════════════════╝
 */

// Nhập các thư viện cần thiết
#include <ZBPrint.h>        // Thư viện in ra Serial Monitor
#include <ZBEeprom24Cxx.h>  // Thư viện tự tạo để giao tiếp với EEPROM 24Cxx

// Khởi tạo đối tượng EEPROM với tham số mặc định
ZBEeprom24Cxx eeprom;  // Sử dụng mặc định: AT24C256 (32KB) tại địa chỉ 0x50

// Định nghĩa các vị trí địa chỉ khác nhau để lưu trữ dữ liệu
#define ADDR_BYTE 0     // Vị trí lưu dữ liệu 1 byte (8 bit)
#define ADDR_INT 10     // Vị trí lưu dữ liệu 2 byte (16 bit)
#define ADDR_LONG 20    // Vị trí lưu dữ liệu 4 byte (32 bit)
#define ADDR_STRING 30  // Vị trí lưu chuỗi văn bản
#define ADDR_CLEAR 100  // Vị trí bắt đầu vùng nhớ sẽ xóa

void setup() {
  // Khởi tạo giao tiếp Serial với tốc độ 9600 baud
  Serial.begin(9600);  // Cấu hình cổng Serial với tốc độ 9600 bps

  while (!Serial) delay(10);  // Chờ kết nối Serial được thiết lập (quan trọng cho các board dùng USB)

  Serial.println("EEPROM Complete Test");  // In tiêu đề chương trình

  // Khởi tạo EEPROM và giao tiếp I2C
  eeprom.begin();  // Tự động khởi tạo Wire và cấu hình giao tiếp

  // Đối với Wire1
  // eeprom.begin(Wire1);  // Khởi tạo Wire1 và cấu hình giao tiếp

  // Kiểm tra kết nối với EEPROM
  if (!eeprom.isConnected()) {            // Kiểm tra xem EEPROM có phản hồi trên bus I2C không
    Serial.println("EEPROM not found!");  // Thông báo lỗi nếu không tìm thấy EEPROM
    return;                               // Thoát hàm setup nếu không có kết nối
  }

  // In thông tin kích thước EEPROM
  Serial.print("EEPROM size: ");
  Serial.print(eeprom.length());  // Lấy kích thước của EEPROM đang sử dụng
  Serial.println(" bytes");       // In đơn vị bytes

  // --------------------------
  // Ví dụ thao tác với dữ liệu 1 byte
  // --------------------------
  byte byte_val = 123;                       // Tạo biến 1 byte với giá trị 123
  eeprom.write_1_byte(ADDR_BYTE, byte_val);  // Ghi giá trị 1 byte vào EEPROM tại địa chỉ ADDR_BYTE

  byte read_byte = eeprom.read_1_byte(ADDR_BYTE);  // Đọc giá trị 1 byte từ EEPROM

  // In kết quả đọc/ghi 1 byte
  Serial.print("1-byte: wrote ");
  Serial.print(byte_val);  // In giá trị đã ghi
  Serial.print(", read ");
  Serial.println(read_byte);  // In giá trị đã đọc

  // --------------------------
  // Ví dụ thao tác với dữ liệu 2 byte
  // --------------------------
  int word_val = 0xABCD;                    // Tạo biến 2 byte với giá trị hex 0xABCD (43981 theo hệ thập phân)
  eeprom.write_2_byte(ADDR_INT, word_val);  // Ghi giá trị 2 byte vào EEPROM tại địa chỉ ADDR_INT

  int read_word = eeprom.read_2_byte(ADDR_INT);  // Đọc giá trị 2 byte từ EEPROM

  // In kết quả đọc/ghi 2 byte theo định dạng hex
  Serial.print("2-byte: wrote 0x");
  Serial.print(word_val, HEX);  // In giá trị đã ghi dưới dạng hex
  Serial.print(", read 0x");
  Serial.println(read_word, HEX);  // In giá trị đã đọc dưới dạng hex

  // --------------------------
  // Ví dụ thao tác với dữ liệu 4 byte
  // --------------------------
  long long_val = 0x12345678;                // Tạo biến 4 byte với giá trị hex phức tạp
  eeprom.write_4_byte(ADDR_LONG, long_val);  // Ghi giá trị 4 byte vào EEPROM

  long read_long = eeprom.read_4_byte(ADDR_LONG);  // Đọc giá trị 4 byte từ EEPROM

  // In kết quả đọc/ghi 4 byte theo định dạng hex
  Serial.print("4-byte: wrote 0x");
  Serial.print(long_val, HEX);  // In giá trị đã ghi dưới dạng hex
  Serial.print(", read 0x");
  Serial.println(read_long, HEX);  // In giá trị đã đọc dưới dạng hex

  // --------------------------
  // Ví dụ thao tác với chuỗi (sử dụng lớp String của Arduino)
  // --------------------------
  String test_string = "Hello EEPROM!";  // Tạo một chuỗi String để lưu trữ
  Serial.print("Writing string: ");
  Serial.println(test_string);  // In chuỗi sẽ ghi

  eeprom.writeString(ADDR_STRING, test_string);  // Ghi chuỗi vào EEPROM

  // Đọc chuỗi từ EEPROM sử dụng lớp String
  String read_string = eeprom.readString(ADDR_STRING);  // Hàm trả về trực tiếp đối tượng String

  // In chuỗi đã đọc
  Serial.print("Read string: ");
  Serial.println(read_string);  // In chuỗi đã đọc từ EEPROM

  // Kiểm tra chuỗi đã đọc có giống chuỗi đã ghi không
  if (test_string.equals(read_string)) {
    Serial.println("String verification successful!");  // Thành công nếu trùng khớp
  } else {
    Serial.println("String verification failed!");  // Thất bại nếu không khớp
  }

  // --------------------------
  // Ví dụ thao tác xóa vùng nhớ
  // --------------------------

  // Ví dụ 1: Xóa một vùng nhớ cụ thể
  Serial.println("\nClearing 20 bytes at address 100");  // Thông báo sẽ xóa 20 byte

  // Xóa 20 byte bắt đầu từ địa chỉ ADDR_CLEAR với giá trị 0xFF (mặc định)
  eeprom.clearRange(ADDR_CLEAR, 20);  // Ghi giá trị 0xFF vào 20 ô nhớ liên tiếp

  // Ví dụ 2: Xóa toàn bộ EEPROM (đã bị comment do mất nhiều thời gian)
  //   Serial.println("\nClearing entire EEPROM (0 to 32KB)");
  //   Serial.println("This will take several seconds to complete!");

  // Lưu ý: Thao tác này sẽ mất nhiều thời gian (vài chục giây)
  // vì phải ghi vào từng ô nhớ trong toàn bộ 32KB

  //   unsigned long startTime = millis();  // Ghi lại thời điểm bắt đầu

  //   eeprom.clear();  // Xóa toàn bộ EEPROM (ghi 0xFF vào tất cả ô nhớ)

  //   unsigned long endTime = millis();  // Ghi lại thời điểm kết thúc
  //   Serial.print("Full EEPROM clear complete! Time taken: ");
  //   Serial.print((endTime - startTime) / 1000.0);  // Tính thời gian đã dùng (giây)
  //   Serial.println(" seconds");

  // Đọc và hiển thị vùng nhớ đã xóa
  Serial.println("Reading cleared area:");  // Thông báo sẽ đọc vùng nhớ đã xóa

  for (int i = 0; i < 20; i++) {                      // Lặp qua 20 byte đã xóa
    byte value = eeprom.read_1_byte(ADDR_CLEAR + i);  // Đọc giá trị tại mỗi địa chỉ

    // In địa chỉ và giá trị đọc được theo định dạng hex
    Serial.print("Addr ");
    Serial.print(ADDR_CLEAR + i);  // In địa chỉ đang đọc
    Serial.print(": 0x");
    Serial.println(value, HEX);  // In giá trị đọc được dưới dạng hex

    // Chỉ in 5 giá trị đầu tiên để tránh quá nhiều dữ liệu hiển thị
    if (i == 4) {
      Serial.println("... (remaining values also 0xFF)");  // Thông báo các giá trị còn lại
      break;                                               // Thoát vòng lặp sau khi in 5 giá trị
    }
  }

  // Thông báo hoàn thành tất cả các bài kiểm tra
  Serial.println("\nAll tests complete!");  // In thông báo kết thúc
}