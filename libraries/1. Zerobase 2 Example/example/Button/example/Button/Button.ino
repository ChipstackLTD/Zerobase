/*
* Điều khiển LED bằng nút nhấn
* 
* Mô tả: Chương trình điều khiển LED đơn giản bằng nút nhấn trên ZeroBase.
* Khi nhấn nút, LED sẽ sáng; khi nhả nút, LED sẽ tắt. Sử dụng chế độ INPUT_PULLUP
* cho nút nhấn để tránh trạng thái lơ lửng và đảm bảo hoạt động ổn định.
* 
* Link bài viết hướng dẫn: [Thêm link bài viết hướng dẫn chi tiết]
*/

// Khai báo chân nút nhấn (button) được kết nối tại chân số 14
const int btn = 14;

// Khai báo chân đèn LED được kết nối tại chân số 3
const int led = 3;

// Hàm setup() chạy một lần khi khởi động hoặc reset board
void setup() {
  // Cấu hình chân nút nhấn là INPUT với chế độ PULLUP (mặc định là mức HIGH khi không nhấn)
  pinMode(btn, INPUT_PULLUP);

  // Cấu hình chân LED là OUTPUT để điều khiển đèn LED
  pinMode(led, OUTPUT);
}

// Hàm loop() chạy liên tục sau khi setup() hoàn thành
void loop() {
  // Kiểm tra trạng thái của nút nhấn
  if (digitalRead(btn)) {     // Nếu nút chưa được nhấn (mức HIGH do PULLUP)
    digitalWrite(led, LOW);   // Tắt đèn LED
  } else {                    // Nếu nút được nhấn (mức LOW)
    digitalWrite(led, HIGH);  // Bật đèn LED
  }
}