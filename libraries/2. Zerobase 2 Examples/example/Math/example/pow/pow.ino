/*
* Ví dụ về hàm pow()
*
* Mô tả: Chương trình này minh họa cách sử dụng hàm pow() để tính lũy thừa của một số.
* Nó sẽ tính 'cơ số' mũ 'số mũ' và in kết quả ra Serial Monitor.
*/

#include <ZBPrint.h>  // Bao gồm thư viện ZBPrint.

// Cú pháp của hàm pow(): pow(cơ_số, số_mũ)
// Trả về: Kết quả của phép tính lũy thừa. Kiểu dữ liệu: double (số thực độ chính xác kép).

int base = 2;      // Cơ số (số được nâng lên lũy thừa)
int exponent = 3;  // Số mũ (lũy thừa)
int res = 0;       // Biến để lưu trữ kết quả. Lưu ý: pow() trả về double, nên có thể có mất mát chính xác khi gán vào int.

void setup() {
  Serial.begin(9600);  // Khởi tạo giao tiếp Serial ở tốc độ 9600 baud.
  while (!Serial) {
    ;  // Đợi cho cổng Serial kết nối. Điều này cần thiết cho các bo mạch có cổng USB gốc.
  }

  // Sử dụng hàm pow() để tính 'base' mũ 'exponent'.
  // Trong ví dụ này, pow(2, 3) sẽ tính 2^3 = 8.
  // Kết quả double (8.0) sẽ được tự động chuyển đổi thành int (8) khi gán vào 'res'.
  res = pow(base, exponent);

  Serial.print("Pow value: ");  // In chuỗi "Pow value: " ra Serial Monitor.
  Serial.println(res);          // In giá trị của res ra Serial Monitor (kết quả sẽ là 8).
}

void loop() {
  // Hàm loop để trống vì tất cả các thao tác minh họa đã được thực hiện trong hàm setup().
}