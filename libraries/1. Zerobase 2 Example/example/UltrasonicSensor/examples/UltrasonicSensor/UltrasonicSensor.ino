/* Hiển thị khoảng cách bằng đèn LED (Ultrasonic Range Indicator)
 *
 * Mô tả: Chương trình này sử dụng cảm biến siêu âm HC-SR04 để đo khoảng cách
 * đến vật thể. Dựa trên khoảng cách đo được, một dải đèn LED sẽ được bật
 * sáng. Càng có vật thể ở gần, càng nhiều đèn LED sẽ sáng lên, tạo thành
 * một chỉ báo trực quan về khoảng cách. Khi vật thể vượt ra ngoài ngưỡng
 * cho phép, tất cả các đèn LED sẽ tắt.
 *
 * Link bài viết hướng dẫn: [Thêm link bài viết hướng dẫn chi tiết]
 */

// Khai báo mảng chứa các chân kết nối LED
const int ledPins[] = { 3, 18, 17, 16, 15, 14 };

// Tính số lượng LED dựa trên kích thước mảng `ledPins`
const int numLeds = sizeof(ledPins) / sizeof(ledPins[0]);

// Khai báo chân phát và nhận tín hiệu của cảm biến siêu âm
const int trigPin = 11;  // Chân gửi tín hiệu (Trigger)
const int echoPin = 10;  // Chân nhận tín hiệu phản hồi (Echo)

// Định nghĩa ngưỡng khoảng cách (tính bằng cm) để bắt đầu bật LED
// Nếu khoảng cách nhỏ hơn hoặc bằng ngưỡng này, LED sẽ bắt đầu sáng
const int distanceThreshold = 20;

void setup() {
  // Cấu hình chân `trigPin` là OUTPUT để gửi xung siêu âm
  pinMode(trigPin, OUTPUT);
  // Cấu hình chân `echoPin` là INPUT để nhận xung phản hồi
  pinMode(echoPin, INPUT);

  // Vòng lặp để thiết lập tất cả các chân LED là OUTPUT
  // và đảm bảo rằng tất cả LED đều tắt khi khởi động
  for (int i = 0; i < numLeds; i++) {
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);  // Tắt LED ban đầu
  }
}

/**
 * @brief Đo khoảng cách đến vật thể bằng cảm biến siêu âm HC-SR04.
 *
 * Hàm này gửi một xung siêu âm ngắn và sau đó đo thời gian chờ để nhận
 * xung phản hồi. Dựa vào thời gian này và tốc độ âm thanh, khoảng cách
 * đến vật thể sẽ được tính toán.
 *
 * @return Khoảng cách đo được bằng centimet (cm).
 */
long getDistance() {
  // Đảm bảo chân `trigPin` ở mức thấp để bắt đầu một chu kỳ đo mới
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);  // Chờ 2 micro giây để chân ổn định

  // Gửi một xung HIGH kéo dài 10 micro giây từ `trigPin` để phát sóng siêu âm
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);  // Kết thúc xung

  // Đo thời gian mà chân `echoPin` ở mức HIGH. Thời gian này tương ứng với
  // thời gian sóng siêu âm đi và về.
  long duration = pulseIn(echoPin, HIGH);

  // Tính toán khoảng cách. Công thức: distance = (duration * speed_of_sound) / 2
  // Tốc độ âm thanh trong không khí là khoảng 343 mét/giây, tương đương 0.0343 cm/micro giây.
  // Chia cho 2 vì sóng đi và về.
  long distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  // Đọc khoảng cách hiện tại từ cảm biến siêu âm
  long distance = getDistance();

  // Kiểm tra nếu khoảng cách đo được nhỏ hơn hoặc bằng ngưỡng `distanceThreshold`
  if (distance <= distanceThreshold) {
    // Ánh xạ khoảng cách từ dải [0, distanceThreshold] sang số lượng LED cần bật [numLeds, 0].
    // Nghĩa là, khi khoảng cách là 0, tất cả LED sẽ sáng (numLeds).
    // Khi khoảng cách là `distanceThreshold`, 0 LED sẽ sáng.
    int ledsOn = map(distance, 0, distanceThreshold, numLeds, 0);

    // Đảm bảo rằng số lượng LED cần bật nằm trong phạm vi từ 0 đến `numLeds`
    ledsOn = constrain(ledsOn, 0, numLeds);

    // Vòng lặp để bật/tắt các đèn LED tương ứng
    for (int i = 0; i < numLeds; i++) {
      // Nếu chỉ số `i` nhỏ hơn số LED cần bật (`ledsOn`), bật LED.
      // Ngược lại, tắt LED.
      digitalWrite(ledPins[i], i < ledsOn ? HIGH : LOW);
    }
    // Giữ trạng thái LED trong một khoảng thời gian ngắn để dễ quan sát
    delay(100);
  } else {
    // Nếu khoảng cách lớn hơn ngưỡng `distanceThreshold`, tắt tất cả các đèn LED
    for (int i = 0; i < numLeds; i++) {
      digitalWrite(ledPins[i], LOW);
    }
  }
  // Thêm một độ trễ nhỏ giữa các lần đọc để tránh đọc quá nhanh và nhiễu
  // hoặc để điều chỉnh tần suất cập nhật hiển thị LED
  delay(50);
}