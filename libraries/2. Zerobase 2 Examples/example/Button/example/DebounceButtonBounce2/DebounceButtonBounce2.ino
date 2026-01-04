/* Chống dội nút nhấn sử dụng thư viện Bounce2
 *
 * Mô tả: Chương trình này điều khiển một LED bật/tắt mỗi khi nút nhấn được
 * bấm. Nó sử dụng thư viện Bounce2 để xử lý chống dội (debounce) nút nhấn
 * một cách hiệu quả, giúp mã gọn gàng và đáng tin cậy hơn.
 *
 * Link bài viết hướng dẫn: [Thêm link bài viết hướng dẫn chi tiết]
 */

// Thêm thư viện Bounce2 để hỗ trợ xử lý chống dội nút nhấn
#include <Bounce2.h>

// Khai báo chân nút nhấn là chân số 3
const int buttonPin = 3;

// Khai báo chân điều khiển LED là chân số 2
const int ledPin = 2;

// Thời gian chống dội phím (debounce) tính bằng mili giây
const int debouceDelay = 50;

// Tạo đối tượng debouncer từ thư viện Bounce
Bounce debouncer = Bounce();

void setup() {
  // Cấu hình chân LED là OUTPUT
  pinMode(ledPin, OUTPUT);

  // Gán chân nút nhấn vào đối tượng debouncer và sử dụng điện trở kéo lên nội (INPUT_PULLUP)
  debouncer.attach(buttonPin, INPUT_PULLUP);

  // Thiết lập khoảng thời gian debounce cho nút nhấn
  debouncer.interval(debouceDelay);
}

void loop() {
  // Cập nhật trạng thái nút nhấn (kiểm tra thay đổi)
  debouncer.update();

  // Nếu nút vừa được nhấn (từ HIGH → LOW)
  if (debouncer.fell()) {
    // Đảo trạng thái của LED (nếu đang bật thì tắt, đang tắt thì bật)
    digitalWrite(ledPin, !digitalRead(ledPin));
  }
}