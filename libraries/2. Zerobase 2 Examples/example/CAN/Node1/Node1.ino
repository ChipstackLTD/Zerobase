/*******************************************************************************
* Node cảm biến nhiệt độ
*
* Mô tả: Chương trình này đọc dữ liệu nhiệt độ từ cảm biến DS18B20 và gửi
* nó qua giao tiếp CAN bus khi nhận được yêu cầu từ node khác.
*
* Link bài viết hướng dẫn chi tiết: [THÊM_LINK_BÀI_VIẾT_CỦA_BẠN_VỀ_CẢM_BIẾN_NHIỆT_ĐỘ_CAN]
*******************************************************************************/

#include <Arduino.h>            // Thư viện cơ bản của Arduino
#include <ZBCan.h>              // Thư viện giao tiếp CAN trên Zerobase
#include <OneWire.h>            // Thư viện giao tiếp với thiết bị OneWire
#include <DallasTemperature.h>  // Thư viện đọc nhiệt độ từ cảm biến DS18B20
#include <ZBPrint.h>            // Thư viện in ấn cho Zerobase (thường dùng cho debug/Serial)

#define ONE_WIRE_BUS A0  // Định nghĩa chân A0 để kết nối với cảm biến DS18B20

OneWire oneWire(ONE_WIRE_BUS);        // Khởi tạo đối tượng OneWire trên chân A0
DallasTemperature sensors(&oneWire);  // Khởi tạo đối tượng cảm biến DS18B20 qua giao tiếp OneWire

int lastTemperature = 0;  // Biến lưu trữ giá trị nhiệt độ gần nhất đọc được

volatile bool tempRequested = false;  // Cờ đánh dấu có yêu cầu đọc nhiệt độ từ node khác

/**
 * @brief Hàm callback được gọi khi nhận được tin nhắn CAN.
 * Hàm này kiểm tra ID và nội dung tin nhắn đến để xác định xem đó có phải
 * là yêu cầu đọc nhiệt độ từ node chính hay không.
 */
void canMessageReceived() {
  uint32_t rxId = ZBCan1.getRxId();  // Lấy ID của node gửi tin nhắn

  // Kiểm tra nếu tin nhắn từ node chính (ID 0x7) và là dạng chuỗi
  if (rxId == 0x7 && ZBCan1.isString()) {
    String message = ZBCan1.getLastStringMessage();  // Lấy nội dung chuỗi tin nhắn
    Serial.print("Message received: ");              // In ra Serial để debug
    Serial.println(message);                         // In nội dung tin nhắn

    if (message == "GET_TEMP") {  // Nếu tin nhắn yêu cầu nhiệt độ
      tempRequested = true;       // Đặt cờ yêu cầu nhiệt độ thành true
    }
  }
}

/**
 * @brief Đọc nhiệt độ từ cảm biến DS18B20.
 * Xử lý lỗi đọc cảm biến bằng cách gán một giá trị mặc định cao.
 */
void readTemperature() {
  sensors.requestTemperatures();                 // Yêu cầu cảm biến đo nhiệt độ
  lastTemperature = sensors.getTempCByIndex(0);  // Đọc nhiệt độ từ cảm biến đầu tiên (index 0), đơn vị độ C

  // Kiểm tra nếu đọc lỗi hoặc giá trị bất thường
  if (lastTemperature == DEVICE_DISCONNECTED_C || lastTemperature < -50) {
    lastTemperature = 1000;                                      // Gán giá trị mặc định là 1000°C để báo lỗi
    Serial.println("Sensor error - using default temperature");  // Thông báo lỗi cảm biến
  }
}

void setup() {
  Serial.begin(115200);                         // Khởi tạo giao tiếp Serial với tốc độ 115200 baud
  Serial.println("Temperature Node Starting");  // In thông báo khởi động

  sensors.begin();  // Khởi tạo cảm biến nhiệt độ

  readTemperature();  // Đọc nhiệt độ lần đầu tiên

  ZBCan1.begin();  // Khởi tạo giao tiếp CAN

  ZBCan1.setTxId(0x2);  // Đặt ID của node này là 0x2 khi gửi tin nhắn

  // Định nghĩa các ID CAN được chấp nhận cho tin nhắn đến:
  // 0x1: Tin nhắn phát sóng chung từ node chính
  // 0x7: Yêu cầu nhiệt độ cụ thể từ node chính
  uint32_t acceptedIds[] = { 0x1, 0x7 };
  ZBCan1.configureMultipleFilters(acceptedIds, 2);  // Cấu hình bộ lọc CAN để chỉ chấp nhận tin nhắn từ các ID đã chọn

  // Thiết lập hàm callback để xử lý khi có tin nhắn CAN đến.
  ZBCan1.onReceive(canMessageReceived);

  Serial.println("Temperature Node Ready");  // In thông báo sẵn sàng
}

void loop() {
  readTemperature();                // Đọc nhiệt độ định kỳ
  Serial.print("Temperature: ");    // In thông tin nhiệt độ ra Serial
  Serial.println(lastTemperature);  // In giá trị nhiệt độ

  if (tempRequested) {                   // Kiểm tra nếu có yêu cầu gửi nhiệt độ
    ZBCan1.send(lastTemperature);        // Gửi giá trị nhiệt độ qua CAN
    Serial.println("Temperature sent");  // In thông báo đã gửi nhiệt độ
    tempRequested = false;               // Đặt lại cờ yêu cầu
  }

  delay(500);  // Tạm dừng 500ms để ổn định
}