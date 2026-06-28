# Changelog

ทุกการเปลี่ยนแปลงที่สำคัญของโปรเจคนี้จะถูกบันทึกไว้ในไฟล์นี้

รูปแบบของไฟล์นี้ขึ้นอยู่กับ [Keep a Changelog](https://keepachangelog.com/en/1.0.0/)
และโปรเจคนี้ปฏิบัติตามหลักการ [Semantic Versioning](https://semver.org/spec/v2.0.0.html)

## [1.0.0] - 2026-06-28

### Added
- ✨ ลำแรกของระบบ Interactive Command Interface
- ✨ รองรับคำสั่ง OUTPUT: ON/OFF, 1/0, status query (?)
- ✨ ฟังก์ชั่นควบคุม LED RGB (Red, Green, Blue, Yellow, Purple, Orange, White)
- ✨ ฟังก์ชั่นควบคุม Buzzer (beep sounds)
- ✨ อ่านสถานะ Digital Input (DI1-DI8)
- ✨ ควบคุม Digital Output (DO1-DO8)
- ✨ RS485 Serial communication
- ✨ HELP menu แสดงคำสั่งทั้งหมด

### Fixed
- 🐛 แก้ไขปัญหา LED RED ไม่ดับ (LEDOff animation bug)
- 🐛 แก้ไขปัญหา OUTPUT ทุกตัวติด (state synchronization bug)
- 🐛 แก้ไขปัญหา OUTPUT command ยังไม่รองรับ lowercase หรือ 1/0

### Changed
- 🔄 อัปเดต help menu ให้แสดงตัวเลือกใหม่
