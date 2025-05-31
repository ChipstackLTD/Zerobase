/*
* Ví dụ về hàm abs()
*
* Mô tả: Chương trình này minh họa cách sử dụng hàm abs() để lấy giá trị tuyệt đối
* của một số nguyên. Nó sẽ in ra màn hình Serial Monitor giá trị tuyệt đối của cả
* số dương và số âm.
*/

#include <ZBPrint.h>  // Bao gồm thư viện ZBPrint, thường được sử dụng cho các chức năng in ấn.

void setup() {
  Serial.begin(9600);  // Khởi tạo giao tiếp Serial ở tốc độ 9600 baud.
  while (!Serial) {
    ;  // Đợi cho cổng Serial kết nối. Điều này cần thiết cho các cổng USB gốc.
  }

  // Khởi tạo một biến số nguyên x với giá trị dương.
  int x = 42;
  // In ra thông báo và giá trị tuyệt đối của x.
  Serial.print("The absolute value of ");
  Serial.print(x);
  Serial.print(" is ");
  Serial.println(abs(x));  // Sử dụng hàm abs() để lấy giá trị tuyệt đối.

  // Gán lại x với một giá trị âm.
  x = -42;
  // In ra thông báo và giá trị tuyệt đối của x.
  Serial.print("The absolute value of ");
  Serial.print(x);
  Serial.print(" is ");
  Serial.println(abs(x));  // Sử dụng hàm abs() để lấy giá trị tuyệt đối.
}

void loop() {
  // Hàm loop để trống vì tất cả các thao tác được thực hiện trong hàm setup().
}