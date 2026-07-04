# IOT_SmartCity
# 🏙️ SCADA THỦ ĐỨC - Smart City IoT Monitoring & Control System

![Platform](https://img.shields.io/badge/Platform-ESP32--C3-orange)
![Cloud](https://img.shields.io/badge/Cloud-Firebase_RTDB-yellow)
![Web](https://img.shields.io/badge/Web-HTML5%20%7C%20JS-blue)
![Mobile](https://img.shields.io/badge/Mobile-Flutter-02569B)
![License](https://img.shields.io/badge/License-MIT-green)

## 📖 Tổng quan dự án (Overview)
**SCADA Thủ Đức** là một hệ thống Thu thập dữ liệu và Điều khiển giám sát (Supervisory Control and Data Acquisition) dựa trên nền tảng vạn vật kết nối (IoT). Hệ thống được thiết kế hướng tới mô hình "Thành phố thông minh", cho phép người quản trị giám sát các thông số môi trường (Nhiệt độ, Độ ẩm, Khí Gas) và điều khiển các cơ cấu chấp hành (Còi báo động, Bơm tưới, Đèn đường) theo thời gian thực.

Dự án sở hữu kiến trúc toàn diện từ **Phần cứng Biên (Edge)**, **Lưu trữ Đám mây (Cloud)** đến **Đa nền tảng Người dùng (Web Dashboard & Mobile App)** với tốc độ đồng bộ độ trễ cực thấp (< 2 giây).

---

## ✨ Tính năng nổi bật (Key Features)

* **⚡ Real-time Synchronization:** Đồng bộ hóa dữ liệu hai chiều giữa thiết bị vật lý và Đám mây thông qua `Firebase Realtime Database` với độ trễ xấp xỉ 0.
* **🛡️ State Change Detection Algorithm:** Thuật toán tối ưu hóa băng thông thông minh trên ESP32. Thiết bị chỉ đẩy dữ liệu lên Cloud khi phát hiện có sự thay đổi vật lý, giúp loại bỏ hoàn toàn hiện tượng *Race Condition* (Xung đột ghi đè dữ liệu 2 chiều).
* **📈 MultiMap Calibration:** Xử lý triệt để sai số lượng tử hóa của cảm biến khí Gas (MQ-2) bằng thuật toán nội suy đa điểm, "bẻ phẳng" đường cong logarit thành dữ liệu PPM chuẩn xác.
* **🖥️ Chuẩn Giao diện SCADA Công nghiệp:** Cả Web và Mobile App đều được thiết kế với Dark Mode, phối màu Neon High-Contrast để cảnh báo rủi ro, chống mỏi mắt khi quan sát 24/7.
* **📱 Đa nền tảng (Cross-Platform):** Quản lý hệ thống mọi lúc, mọi nơi thông qua Web Dashboard (JS) hoặc Mobile App (Flutter).

---

## 🏗️ Kiến trúc hệ thống (System Architecture)

Hệ thống được chia làm 3 lớp chính:
1. **Lớp Phần cứng (Hardware Layer):** * Vi điều khiển trung tâm: `ESP32-C3-MINI-1` (Xử lý đa nhiệm, tích hợp Wi-Fi).
   * Khối thu thập: `DHT22` (1-Wire), `MQ-2` (ADC 12-bit).
   * Khối chấp hành: Relay điều khiển Còi, Đèn, Bơm nước.
2. **Lớp Đám mây (Cloud Layer):**
   * `Google Firebase RTDB`: Phân chia cấu trúc JSON thành 2 nhánh độc lập `Sensors` (Uplink) và `Controls` (Downlink) để tránh tràn RAM và thắt nút cổ chai băng thông.
3. **Lớp Giao diện HMI (Presentation Layer):**
   * **Web Dashboard:** Tích hợp `Chart.js` để trực quan hóa đồ thị xu hướng thời gian thực. Cơ chế Event Listener bắt các sự kiện biến động môi trường.
   * **Mobile App:** Phát triển bằng `Flutter / Dart`, sử dụng `StreamBuilder` để thiết lập luồng Socket lắng nghe liên tục.

---

## 🛠️ Công nghệ sử dụng (Tech Stack)

| Thành phần | Công nghệ / Thư viện |
| --- | --- |
| **Firmware (ESP32)** | C++, PlatformIO, Firebase ESP32 Client, DHT Sensor Library |
| **Cloud Database** | Google Firebase Realtime Database |
| **Web Dashboard** | HTML5, CSS3, Vanilla JavaScript, Chart.js |
| **Mobile App** | Flutter, Dart, firebase_database, fl_chart |

---

## 📸 Ảnh chụp màn hình (Screenshots)


### 1. Web Dashboard
![Web Dashboard](https://cdn.phototourl.com/free/2026-07-04-f128dda4-e613-442d-abab-1f07828c9e8a.png)
*Giao diện giám sát trên màn hình máy tính với thống và Console Log.*

### 2. Mobile App (Flutter)
![Mobile App](https://cdn.phototourl.com/free/2026-07-04-02d9b077-c2ea-4ed4-ab25-4a1750c889fc.jpg)
*Ứng dụng cầm tay giám sát và điều khiển trên nền tảng Android/iOS.*

### 3. Sơ đồ phần cứng Wokwi
![Wokwi Simulation](https://cdn.phototourl.com/free/2026-07-04-e5edc84a-cb73-4f63-9e00-ac75ec740c77.png)


## 👥 Nhóm phát triển (Contributors)

* **Phạm Quốc Thắng** (MSSV: 23161193) - Kỹ thuật Viễn thông và Thiết kế Vi mạch, HCMUTE.
* **Đặng Thành Dương** - HCMUTE.

---
*Dự án được phát triển trong khuôn khổ học phần Đồ án/Final Project tại trường Đại học Sư phạm Kỹ thuật TP.HCM (HCMUTE).*
