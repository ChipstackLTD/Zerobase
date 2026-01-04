/*
* Ví dụ về hàm sq()
*
* Mô tả: Chương trình này minh họa cách sử dụng hàm sq() để tính bình phương của một số.
* Nó sẽ đọc một số từ Serial Monitor, tính bình phương của số đó và in cả số gốc
* và kết quả bình phương ra màn hình.
*/

#include <ZBPrint.h>  // Bao gồm thư viện ZBPrint.

// Cú pháp của hàm sq(): sq(x)
// Trả về: Bình phương của số. Kiểu dữ liệu: double (số thực độ chính xác kép).

int x = 0;    // Biến để lưu trữ giá trị đọc từ Serial Monitor.
int res = 0;  // Biến để lưu trữ kết quả bình phương. Lưu ý: sq() trả về double, nên có thể có mất mát chính xác khi gán vào int.

void setup() {
  Serial.begin(9600);  // Khởi tạo giao tiếp Serial ở tốc độ 9600 baud.
  while (!Serial) {
    ;  // Đợi cho cổng Serial kết nối. Điều này cần thiết cho các bo mạch có cổng USB gốc.
  }
  // Lưu ý quan trọng: Do cách triển khai của hàm sq(),
  // bạn nên tránh sử dụng các hàm khác bên trong dấu ngoặc của nó, điều này có thể dẫn đến kết quả không chính xác.
  // Ví dụ: // int x = sq(Serial.parseInt()); // nên tránh cách này
}

void loop() {
  // Đọc một số nguyên từ Serial Monitor. Hàm parseInt() sẽ đợi cho đến khi nhận được một số.
  x = Serial.parseInt();  // Giữ các phép toán khác bên ngoài hàm sq().

  // Chỉ thực hiện tính toán nếu giá trị đọc được khác 0.
  if (x != 0) {
    // Tính bình phương của x. Kết quả là double và sẽ được chuyển đổi thành int khi gán vào 'res'.
    int res = sq(x);

    Serial.print("x value: ");  // In ra chuỗi "x value: "
    Serial.println(x);          // In giá trị gốc của x.

    Serial.print("Square value: ");  // In ra chuỗi "Square value: "
    Serial.println(res);             // In giá trị bình phương (phần nguyên).
  }
}