#include <WiFi.h>
#include <FirebaseESP32.h>
#include "DHT.h"
#include <math.h> // Thư viện để dùng hàm fabs() so sánh số thực

// Các thư viện phụ trợ bắt buộc của Firebase
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// 1. Cấu hình WiFi ảo của Wokwi
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// 2. Cấu hình Firebase của bạn
#define API_KEY "ADD YOUR API KEY FIREBASE HERE"
#define DATABASE_URL "ADD YOUR DATABASE URL HERE"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// --- KHAI BÁO CHO LUỒNG STREAM 2 CHIỀU ---
FirebaseData streamSensors;
unsigned long lastUpdateTimer = 0;        
const unsigned long UPDATE_INTERVAL = 2000; // Tốc độ phản hồi cực nhanh: 2 giây/lần

// --- BIẾN LƯU TRẠNG THÁI CŨ (STATE CHANGE DETECTION) ---
float last_t = -999.0;
float last_h = -999.0;
int last_gas = -999;

// Khai báo chân cảm biến
#define DHTPIN 8
#define DHTTYPE DHT22
#define MQ_PIN 1
DHT dht(DHTPIN, DHTTYPE);

// Danh sách các điểm dữ liệu thực tế trích xuất từ Wokwi
int raw_points[] = {0,    3672  , 3762   , 3839   ,  3899 , 3903 , 3935  , 3946  , 3977 , 4095};

// Các mốc PPM tương ứng
int ppm_points[] = {0,    500,         1000,       2000,       4000,       6000,       8000,       10000,       15000,       100000};

// Sửa lại số lượng điểm mốc
const int num_points = 10;

// Hàm nội suy đa điểm
int multiMap(int val, int* _in, int* _out, uint8_t size) {
  if (val <= _in[0]) return _out[0];
  if (val >= _in[size-1]) return _out[size-1];
  uint8_t pos = 1;
  while(val > _in[pos]) pos++;
  if (val == _in[pos]) return _out[pos];
  return (val - _in[pos-1]) * (_out[pos] - _out[pos-1]) / (_in[pos] - _in[pos-1]) + _out[pos-1];
}

// ---------- HÀM BẮT SỰ KIỆN KHI BẠN NHẬP TAY TRÊN FIREBASE ----------
void streamSensorCallback(StreamData data) {
  Serial.printf("\n>>> [FIREBASE -> WOKWI] Nhận lệnh sửa tay từ Firebase tại nhánh %s = %s\n", data.dataPath().c_str(), data.stringData().c_str());
  // Nhận xong hệ thống vẫn chạy bình thường, KHÔNG KHÓA GÌ CẢ!
}

void streamSensorTimeoutCallback(bool timeout) {
  if (timeout) Serial.println("Stream Sensors Timeout! Đang kết nối lại...");
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // Thiết lập độ phân giải ADC cho ESP32-C3
  analogReadResolution(12);
  
  // ---------- KẾT NỐI WIFI ----------
  Serial.print("Đang kết nối WiFi Wokwi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nĐã kết nối WiFi!");
  
  // ---------- CẤU HÌNH FIREBASE ----------
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Xác thực Firebase thành công!");
  } else {
    Serial.printf("Lỗi xác thực: %s\n", config.signer.signupError.message.c_str());
  }
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // ---------- MỞ LUỒNG LẮNG NGHE 2 CHIỀU ----------
  if (Firebase.beginStream(streamSensors, "/SmartCity/ThuDuc/Sensors")) { 
    Serial.println("Đã mở luồng lắng nghe 2 chiều không khóa cho Sensors!");
    Firebase.setStreamCallback(streamSensors, streamSensorCallback, streamSensorTimeoutCallback); 
  } else {
    Serial.println("Lỗi mở Stream: " + streamSensors.errorReason());
  }
}

void loop() {
  // Lấy mẫu siêu tốc mỗi 2 giây
  if (millis() - lastUpdateTimer >= UPDATE_INTERVAL) {
    lastUpdateTimer = millis(); 
    
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    int mqValue = analogRead(MQ_PIN);

    if (isnan(h) || isnan(t)) return; 

    int gasPPM = multiMap(mqValue, raw_points, ppm_points, num_points);

    // ---------- THUẬT TOÁN QUYẾT ĐỊNH: CÓ GỬI HAY KHÔNG? ----------
    // Chỉ gửi Firebase NẾU giá trị cảm biến khác với lần đọc trước đó
    if (fabs(t - last_t) > 0.01 || fabs(h - last_h) > 0.01 || gasPPM != last_gas) {
      
      // 1. Cập nhật lại bộ nhớ
      last_t = t;
      last_h = h;
      last_gas = gasPPM;

      // 2. Bắn dữ liệu lên Firebase
      if (Firebase.ready()) {
        Firebase.setFloat(fbdo, "/SmartCity/ThuDuc/Sensors/temp", t);
        Firebase.setFloat(fbdo, "/SmartCity/ThuDuc/Sensors/humi", h);
        Firebase.setInt(fbdo, "/SmartCity/ThuDuc/Sensors/gas", gasPPM); 

        Serial.printf("[WOKWI -> FIREBASE] Có thay đổi vật lý! T=%.1f | H=%.1f | Gas=%d\n", t, h, gasPPM);
      }
    } else {
      // Cảm biến vật lý đang đứng im -> Bỏ qua không gửi
      // Đây là lúc Firebase được toàn quyền nhận dữ liệu nhập tay của bạn!
    }
  }

  delay(10); 
}