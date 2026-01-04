/*
* Ví dụ về hàm map()
*
* Mô tả: Chương trình này minh họa cách sử dụng hàm map() để ánh xạ (chuyển đổi)
* một giá trị từ một phạm vi đầu vào sang một phạm vi đầu ra khác. Điều này rất
* hữu ích khi bạn muốn thay đổi thang đo của một cảm biến hoặc điều khiển.
*/

#include <ZBPrint.h>  // Bao gồm thư viện ZBPrint, thường được sử dụng cho các chức năng in ấn.

// Cú pháp của hàm map(): map(giá_trị, từ_thấp, từ_cao, đến_thấp, đến_cao)
// Trả về: Giá trị đã được ánh xạ. Kiểu dữ liệu: long (số nguyên dài).

int val = 1023;       // Giá trị đầu vào cần ánh xạ (ví dụ: đọc từ cảm biến analog 10-bit)
int fromLow = 0;      // Giới hạn dưới của phạm vi đầu vào
int fromHigh = 1023;  // Giới hạn trên của phạm vi đầu vào
int toLow = 0;        // Giới hạn dưới của phạm vi đầu ra
int toHigh = 255;     // Giới hạn trên của phạm vi đầu ra (ví dụ: điều khiển đèn LED PWM 8-bit)

void setup() {
  Serial.begin(9600);  // Khởi tạo giao tiếp Serial với tốc độ 9600 baud để in dữ liệu.
  while (!Serial) {
    ;  // Đợi cổng Serial kết nối. Cần thiết cho các bo mạch có cổng USB gốc.
  }

  // Ánh xạ giá trị 'val' từ phạm vi [fromLow, fromHigh] sang phạm vi [toLow, toHigh].
  // Trong ví dụ này, 1023 (từ 0-1023) sẽ được ánh xạ thành 255 (từ 0-255).
  val = map(val, fromLow, fromHigh, toLow, toHigh);

  Serial.print("Map value: ");  // In ra chuỗi "Map value: "
  Serial.println(val);          // In giá trị 'val' đã được ánh xạ ra Serial Monitor (kết quả sẽ là 255).
}

void loop() {
  // Hàm loop để trống vì ví dụ này chỉ cần chạy một lần trong hàm setup().
}