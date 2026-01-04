/* Điều khiển động cơ bước 28BYJ-48 với Driver ULN2003
 *
 * Mô tả: Chương trình này điều khiển động cơ bước 28BYJ-48 quay một vòng
 * theo chiều kim đồng hồ, sau đó quay một vòng ngược chiều kim đồng hồ,
 * lặp lại liên tục. Tốc độ quay của động cơ được đặt là 15 vòng/phút.
 * Chương trình này rất hữu ích để kiểm tra hoạt động cơ bản của động cơ
 * bước và driver ULN2003.
 *
 * Link bài viết hướng dẫn: https://elearning.chipstack.vn/mod/page/view.php?id=98
 */

// Khai báo thư viện điều khiển motor bước với driver ULN2003
#include <Stepper.h>

// Số bước của một vòng quay (của 28BYJ-48 là khoảng 2048 bước/vòng)
const int stepsPerRevolution = 2048;

// Khởi tạo đối tượng Stepper
// Các chân A0, A1, A2, A3 trên Zerobase được kết nối với IN1, IN2, IN3, IN4 của ULN2003
// Chú ý thứ tự chân: IN1->IN3->IN2->IN4 theo ULN2003 datasheet (hoặc theo cách đấu nối thực tế)
// Đối với thư viện Stepper, thứ tự chân thường là (pin1, pin3, pin2, pin4) nếu xét theo thứ tự mạch điện phổ biến:
// IN1 (A0), IN2 (A1), IN3 (A2), IN4 (A3)
// Vì vậy, để động cơ quay đúng, bạn cần kiểm tra lại thứ tự chân đấu nối với driver và điều chỉnh các tham số trong hàm khởi tạo Stepper cho phù hợp.
// Ví dụ: Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);
Stepper myStepper(stepsPerRevolution, A0, A2, A1, A3);

void setup() {
  // Đặt tốc độ quay của động cơ (đơn vị: vòng/phút - rpm)
  myStepper.setSpeed(15);  // Đặt tốc độ là 15 vòng/phút
}

void loop() {
  // Quay động cơ một vòng theo chiều kim đồng hồ
  // Hàm .step() nhận vào số bước cần quay. Số dương quay thuận, số âm quay ngược.
  myStepper.step(stepsPerRevolution);  // Thực hiện quay đủ số bước cho một vòng
  delay(1000);                         // Dừng 1 giây sau khi quay xong một vòng

  // Quay động cơ một vòng ngược chiều kim đồng hồ
  myStepper.step(-stepsPerRevolution);  // Thực hiện quay đủ số bước cho một vòng theo chiều ngược lại
  delay(1000);                          // Dừng 1 giây sau khi quay xong một vòng
}