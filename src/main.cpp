
/***********************************************************************
 * Project      :     ESP32-S3-POE-ETH-8DI-8DO Interactive Command Tester
 * Description  :     Interactive command interface for testing all board features
 * Hardware     :     ESP32-S3-POE-ETH-8DI-8DO
 * Author       :     Uten Boonliam (Tenergy Innovation Co., Ltd.)
 * Date         :     28/06/2026
 * Revision     :     1.0.0
 * ***********************************************************************/

#include <Arduino.h>
#include <ESP32S3_8DI8DO.h>
#include <SPIFFS.h>

/***********************************************************************
 * GLOBAL VARIABLES
 ***********************************************************************/
ESP32S3_8DI8DO board;

// Serial input buffer
String inputBuffer = "";
const int MAX_COMMAND_LENGTH = 100;

// Timing variables
unsigned long lastSerialPromptTime = 0;
const unsigned long PROMPT_INTERVAL = 100;  // Show prompt every 100ms if idle

/***********************************************************************
 * FUNCTION PROTOTYPES
 ***********************************************************************/
void printWelcome(void);
void printHelpMenu(void);
void printPrompt(void);
void processCommand(String cmd);
void handleBuzzerCommand(String params);
void handleLEDCommand(String params);
void handleOutputCommand(String params);
void handleInputCommand(String params);
void handleRTCCommand(String params);
void handleStatusCommand(String params);
void handleResetCommand(void);
void printSeparator(const char* title = "");

/***********************************************************************
 * SETUP FUNCTION
 ***********************************************************************/
void setup() {
    Serial.begin(115200);
    delay(1000);
    
    // Initialize board
    ESP32S3_8DI8DO::Status status = board.begin();
    
    if (status != ESP32S3_8DI8DO::STATUS_OK) {
        Serial.println("\n❌ BOARD INITIALIZATION FAILED!");
        Serial.print("Status: ");
        Serial.println((int)status);
        while (1) delay(1000);
    }
    
    // Initialize RS485
    if (!board.setupRS485(9600)) {
        Serial.println("\n⚠️  RS485 initialization skipped");
    }
        // Initialize SPIFFS for flash space info
    if (!SPIFFS.begin(true)) {
        Serial.println("⚠️  SPIFFS initialization failed");
    }
        printWelcome();
}

/***********************************************************************
 * MAIN LOOP FUNCTION
 ***********************************************************************/
void loop() {
    // Update board (required for animations)
    board.loop();
    
    // Process incoming Serial data
    while (Serial.available() > 0) {
        char ch = Serial.read();
        
        // Echo character to user
        Serial.write(ch);
        
        if (ch == '\n' || ch == '\r') {
            if (inputBuffer.length() > 0) {
                Serial.println();  // New line after input
                processCommand(inputBuffer);
                inputBuffer = "";
            }
            printPrompt();
        } else if (ch == '\b' || ch == 127) {
            // Handle backspace
            if (inputBuffer.length() > 0) {
                inputBuffer.remove(inputBuffer.length() - 1);
                Serial.print(" \b");  // Erase character visually
            }
        } else if (ch >= 32 && ch < 127) {
            // Printable characters
            if (inputBuffer.length() < MAX_COMMAND_LENGTH) {
                inputBuffer += ch;
            }
        }
    }
}

/***********************************************************************
 * PRINT WELCOME MESSAGE
 ***********************************************************************/
void printWelcome(void) {
    Serial.println("\n\n");
    Serial.println("╔══════════════════════════════════════════════════════════════════╗");
    Serial.println("║                                                                  ║");
    Serial.println("║         ESP32-S3-POE-ETH-8DI-8DO Interactive Tester             ║");
    Serial.println("║                                                                  ║");
    Serial.println("║  🎛️  Control: LED, Buzzer, Digital Input/Output, RS485         ║");
    Serial.println("║  📋 Type 'HELP' for command list                                ║");
    Serial.println("║                                                                  ║");
    Serial.println("╚══════════════════════════════════════════════════════════════════╝");
    Serial.println();
    Serial.println("✅ Board initialized successfully!");
    Serial.println("✅ RS485 initialized at 9600 baud");
    Serial.println();
    
    printPrompt();
}

/***********************************************************************
 * PRINT HELP MENU
 ***********************************************************************/
void printHelpMenu(void) {
    printSeparator("AVAILABLE COMMANDS");
    
    Serial.println("\n1️⃣  BUZZER <times>");
    Serial.println("    Description: Sound buzzer N times");
    Serial.println("    Example:     BUZZER 3");
    Serial.println("    Valid range: 1-20 times\n");
    
    Serial.println("2️⃣  LED <color> <seconds>");
    Serial.println("    Description: Control RGB LED color");
    Serial.println("    Colors:      red, green, blue, yellow, purple, orange, white");
    Serial.println("    Duration:    Float value (0.5-10.0) in seconds, or -1 to turn off");
    Serial.println("    Examples:    LED red 2.5");
    Serial.println("                 LED blue -1    (Turn off LED)\n");
    
    Serial.println("3️⃣  OUTPUT <channel> <state>");
    Serial.println("    Description: Control Digital Output (Relay)");
    Serial.println("    Channel:     1-8 (or DO1-DO8)");
    Serial.println("    State:       ON/OFF, 1/0, or ? (read status)");
    Serial.println("    Examples:    OUTPUT 1 ON");
    Serial.println("                 OUTPUT DO5 OFF");
    Serial.println("                 OUTPUT 3 1       (same as ON)");
    Serial.println("                 OUTPUT 2 0       (same as OFF)");
    Serial.println("                 OUTPUT 4 ?       (read status)\n");
    
    Serial.println("4️⃣  INPUT <channel>");
    Serial.println("    Description: Read Digital Input status");
    Serial.println("    Channel:     1-8 (or DI1-DI8)");
    Serial.println("    Examples:    INPUT 1");
    Serial.println("                 INPUT DI3\n");
    
    Serial.println("5️⃣  RTC GET");
    Serial.println("    Description: Read current RTC time");
    Serial.println("    Example:     RTC GET");
    Serial.println("    Output:      Current time in format: YYYY-MM-DD HH:MM:SS\n");
    
    Serial.println("6️⃣  RTC SET <year>/<month>/<day> <hour>:<minute>:<second>");
    Serial.println("    Description: Set RTC to specific date and time");
    Serial.println("    Example:     RTC SET 2026/06/28 15:30:45\n");
    
    Serial.println("7️⃣  STATUS");
    Serial.println("    Description: Display ESP32-S3 system information");
    Serial.println("    Information: Temperature, MAC, RAM, Flash, etc.\n");
    
    Serial.println("8️⃣  RESET");
    Serial.println("    Description: Reset ESP32-S3 board\n");
    
    Serial.println("9️⃣  HELP");
    Serial.println("    Description: Show this help menu\n");
    
    printSeparator();
    Serial.println();
}

/***********************************************************************
 * PRINT COMMAND PROMPT
 ***********************************************************************/
void printPrompt(void) {
    Serial.print("📝 > ");
}

/***********************************************************************
 * PRINT SEPARATOR
 ***********************************************************************/
void printSeparator(const char* title) {
    if (title != NULL && strlen(title) > 0) {
        Serial.print("┌─── ");
        Serial.print(title);
        Serial.println(" ───────────────────────────────────┐");
    } else {
        Serial.println("└─────────────────────────────────────────────────────────┘");
    }
}

/***********************************************************************
 * PROCESS INCOMING COMMAND
 ***********************************************************************/
void processCommand(String cmd) {
    // Convert to uppercase
    cmd.toUpperCase();
    
    // Trim whitespace
    cmd.trim();
    
    if (cmd.length() == 0) {
        return;
    }
    
    Serial.println();
    
    // Parse command
    int spaceIndex = cmd.indexOf(' ');
    String commandType = (spaceIndex > 0) ? cmd.substring(0, spaceIndex) : cmd;
    String params = (spaceIndex > 0) ? cmd.substring(spaceIndex + 1) : "";
    
    // Execute command
    if (commandType == "BUZZER") {
        handleBuzzerCommand(params);
    } else if (commandType == "LED") {
        handleLEDCommand(params);
    } else if (commandType == "OUTPUT") {
        handleOutputCommand(params);
    } else if (commandType == "INPUT") {
        handleInputCommand(params);
    } else if (commandType == "RTC") {
        handleRTCCommand(params);
    } else if (commandType == "STATUS") {
        handleStatusCommand(params);
    } else if (commandType == "RESET") {
        handleResetCommand();
    } else if (commandType == "HELP") {
        printHelpMenu();
    } else {
        Serial.print("❌ Unknown command: '");
        Serial.print(commandType);
        Serial.println("' - Type 'HELP' for available commands");
    }
    
    Serial.println();
}

/***********************************************************************
 * HANDLE BUZZER COMMAND
 * Format: BUZZER <times>
 ***********************************************************************/
void handleBuzzerCommand(String params) {
    params.trim();
    
    if (params.length() == 0) {
        Serial.println("❌ Missing parameter: BUZZER <times>");
        Serial.println("   Example: BUZZER 3");
        return;
    }
    
    int times = params.toInt();
    
    if (times < 1 || times > 20) {
        Serial.print("❌ Invalid times: ");
        Serial.print(times);
        Serial.println(" (valid range: 1-20)");
        return;
    }
    
    Serial.print("🔊 Buzzing ");
    Serial.print(times);
    Serial.println(" time(s)...");
    
    board.buzzer_beep(times);
    
    Serial.println("✅ Buzzer complete");
}

/***********************************************************************
 * HANDLE LED COMMAND
 * Format: LED <color> <seconds>
 ***********************************************************************/
void handleLEDCommand(String params) {
    params.trim();
    
    // Parse color and duration
    int spaceIndex = params.indexOf(' ');
    if (spaceIndex < 0) {
        Serial.println("❌ Missing parameters: LED <color> <seconds>");
        Serial.println("   Example: LED red 2.5");
        return;
    }
    
    String color = params.substring(0, spaceIndex);
    String durationStr = params.substring(spaceIndex + 1);
    
    color.toUpperCase();
    float duration = durationStr.toFloat();
    
    // Validate duration
    if (duration < -1 || (duration > 0 && duration < 0.1)) {
        Serial.print("❌ Invalid duration: ");
        Serial.println(duration);
        Serial.println("   Use: -1 (OFF), or 0.1-10.0 (seconds)");
        return;
    }
    
    // Turn off LED
    if (duration == -1) {
        Serial.print("💡 Turning OFF LED...");
        board.LEDOff();
        Serial.println(" ✅");
        return;
    }
    
    // Set LED color
    Serial.print("💡 LED: ");
    Serial.print(color);
    Serial.print(" for ");
    Serial.print(duration);
    Serial.println(" seconds...");
    
    if (color == "RED") {
        board.TickRedLED(duration);
    } else if (color == "GREEN") {
        board.TickGreenLED(duration);
    } else if (color == "BLUE") {
        board.TickBlueLED(duration);
    } else if (color == "YELLOW") {
        board.TickYellowLED(duration);
    } else if (color == "PURPLE") {
        board.TickPurpleLED(duration);
    } else if (color == "ORANGE") {
        board.TickOrangeLED(duration);
    } else if (color == "WHITE") {
        board.TickWhiteLED(duration);
    } else {
        Serial.print("❌ Unknown color: ");
        Serial.println(color);
        Serial.println("   Valid colors: red, green, blue, yellow, purple, orange, white");
        return;
    }
    
    Serial.println("✅ LED animation started");
}

/***********************************************************************
 * HANDLE OUTPUT COMMAND
 * Format: OUTPUT <channel> <state>
 * state: ON, OFF, 1, 0, or ? (to read status)
 ***********************************************************************/
void handleOutputCommand(String params) {
    params.trim();
    
    // Parse channel and state
    int spaceIndex = params.indexOf(' ');
    if (spaceIndex < 0) {
        Serial.println("❌ Missing parameters: OUTPUT <channel> <state>");
        Serial.println("   Example: OUTPUT 1 ON");
        Serial.println("   Example: OUTPUT 5 ?");
        return;
    }
    
    String channelStr = params.substring(0, spaceIndex);
    String state = params.substring(spaceIndex + 1);
    
    state.trim();
    state.toUpperCase();
    
    // Parse channel number (handle "DO1" or "1" format)
    int channel = 0;
    if (channelStr.startsWith("DO")) {
        channel = channelStr.substring(2).toInt();
    } else {
        channel = channelStr.toInt();
    }
    
    // Validate channel
    if (channel < 1 || channel > 8) {
        Serial.print("❌ Invalid channel: ");
        Serial.print(channel);
        Serial.println(" (valid range: 1-8)");
        return;
    }
    
    // Handle READ status request
    if (state == "?") {
        bool status = board.getDO().getState(channel - 1);
        Serial.print("🔌 DO");
        Serial.print(channel);
        Serial.print(" Status: ");
        Serial.println(status ? "🟢 ON" : "🔴 OFF");
        return;
    }
    
    // Convert alternate formats to ON/OFF
    if (state == "1") {
        state = "ON";
    } else if (state == "0") {
        state = "OFF";
    }
    
    // Validate state
    if (state != "ON" && state != "OFF") {
        Serial.print("❌ Invalid state: ");
        Serial.println(state);
        Serial.println("   Valid states: ON, OFF, 1, 0, or ? (read status)");
        return;
    }
    
    // Control output
    Serial.print("🔌 DO");
    Serial.print(channel);
    Serial.print(" -> ");
    Serial.print(state);
    Serial.println("...");
    
    if (state == "ON") {
        board.getDO().on(channel - 1);
    } else {
        board.getDO().off(channel - 1);
    }
    
    Serial.println("✅ Output changed");
}

/***********************************************************************
 * HANDLE INPUT COMMAND
 * Format: INPUT <channel>
 ***********************************************************************/
void handleInputCommand(String params) {
    params.trim();
    
    if (params.length() == 0) {
        Serial.println("❌ Missing parameter: INPUT <channel>");
        Serial.println("   Example: INPUT 1");
        return;
    }
    
    // Parse channel (handle "DI1" or "1" format)
    int channel = 0;
    if (params.startsWith("DI")) {
        channel = params.substring(2).toInt();
    } else {
        channel = params.toInt();
    }
    
    // Validate channel
    if (channel < 1 || channel > 8) {
        Serial.print("❌ Invalid channel: ");
        Serial.print(channel);
        Serial.println(" (valid range: 1-8)");
        return;
    }
    
    // Read input
    bool inputState = board.getDigitalInput(channel);
    
    Serial.print("🔌 DI");
    Serial.print(channel);
    Serial.print(" = ");
    Serial.print(inputState ? "HIGH" : "LOW");
    Serial.println();
    
    Serial.print("   Status: ");
    Serial.println(inputState ? "🟢 ON (HIGH)" : "🔴 OFF (LOW)");
}

/***********************************************************************
 * HANDLE STATUS COMMAND
 * Displays ESP32S3 system information
 ***********************************************************************/
void handleStatusCommand(String params) {
    params.trim();
    params.toUpperCase();
    
    Serial.println();
    Serial.println("═══════════════════════════════════════════════════════════════");
    Serial.println("              📊 ESP32-S3 SYSTEM STATUS");
    Serial.println("═══════════════════════════════════════════════════════════════");
    Serial.println();
    
    // 1. Temperature
    Serial.println("🌡️  TEMPERATURE");
    Serial.println("───────────────────────────────────────────────────────────────");
    float tempC = temperatureRead();  // Already returns Celsius
    Serial.printf("   Current: %.1f°C\n", tempC);
    Serial.println();
    
    // 2. MAC Address
    Serial.println("📱 MAC ADDRESS");
    Serial.println("───────────────────────────────────────────────────────────────");
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    Serial.printf("   WiFi (STA): %02X:%02X:%02X:%02X:%02X:%02X\n", 
                  mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.println();
    
    // 3. Memory Information
    Serial.println("💾 MEMORY (RAM)");
    Serial.println("───────────────────────────────────────────────────────────────");
    uint32_t heapSize = ESP.getHeapSize();
    uint32_t heapFree = ESP.getFreeHeap();
    uint32_t heapUsed = heapSize - heapFree;
    Serial.printf("   Total Size: %u bytes (%.2f KB)\n", heapSize, heapSize / 1024.0);
    Serial.printf("   Used: %u bytes (%.2f KB)\n", heapUsed, heapUsed / 1024.0);
    Serial.printf("   Free: %u bytes (%.2f KB)\n", heapFree, heapFree / 1024.0);
    Serial.printf("   Usage: %.1f%%\n", (heapUsed * 100.0) / heapSize);
    Serial.println();
    
    // 4. Flash Information
    Serial.println("💿 FLASH");
    Serial.println("───────────────────────────────────────────────────────────────");
    uint32_t flashSize = ESP.getFlashChipSize();
    uint32_t sketchSize = ESP.getSketchSize();
    // Calculate remaining flash available for sketch
    uint32_t flashFree = flashSize - sketchSize;
    Serial.printf("   Total Size: %u bytes (%.2f MB)\n", flashSize, flashSize / 1024.0 / 1024.0);
    Serial.printf("   Sketch Size: %u bytes (%.2f KB)\n", sketchSize, sketchSize / 1024.0);
    Serial.printf("   Free: %u bytes (%.2f MB)\n", flashFree, flashFree / 1024.0 / 1024.0);
    
    // Check SPIFFS if available
    if (SPIFFS.begin(false)) {
        uint32_t spiffsTotalBytes = SPIFFS.totalBytes();
        uint32_t spiffsUsedBytes = SPIFFS.usedBytes();
        uint32_t spiffsFreeSpace = spiffsTotalBytes - spiffsUsedBytes;
        Serial.printf("   SPIFFS: %u bytes total, %u bytes free (%.2f KB)\n", 
                      spiffsTotalBytes, spiffsFreeSpace, spiffsFreeSpace / 1024.0);
    } else {
        Serial.println("   SPIFFS: Not initialized");
    }
    Serial.println();
    
    // 5. Chip Information
    Serial.println("🖥️  CHIP INFORMATION");
    Serial.println("───────────────────────────────────────────────────────────────");
    Serial.printf("   Model: %s\n", ESP.getChipModel());
    Serial.printf("   Revision: %d\n", ESP.getChipRevision());
    Serial.printf("   Cores: %d\n", ESP.getChipCores());
    Serial.printf("   CPU Frequency: %u MHz\n", ESP.getCpuFreqMHz());
    Serial.printf("   SDK Version: %s\n", ESP.getSdkVersion());
    Serial.println();
    
    // 6. Uptime
    Serial.println("⏱️  UPTIME");
    Serial.println("───────────────────────────────────────────────────────────────");
    uint32_t uptime = millis();
    uint32_t days = uptime / (24 * 60 * 60 * 1000);
    uint32_t hours = (uptime % (24 * 60 * 60 * 1000)) / (60 * 60 * 1000);
    uint32_t minutes = (uptime % (60 * 60 * 1000)) / (60 * 1000);
    uint32_t seconds = (uptime % (60 * 1000)) / 1000;
    Serial.printf("   %d days, %02d:%02d:%02d (Total: %u ms)\n", days, hours, minutes, seconds, uptime);
    Serial.println();
    
    Serial.println("═══════════════════════════════════════════════════════════════");
    Serial.println();
}

/***********************************************************************
 * HANDLE RTC COMMAND
 * Format: RTC GET
 *         RTC SET <year>/<month>/<day> <hour>:<minute>:<second>
 ***********************************************************************/
void handleRTCCommand(String params) {
    params.trim();
    params.toUpperCase();
    
    if (params.length() == 0) {
        Serial.println("❌ Missing RTC sub-command");
        Serial.println("   Usage: RTC GET");
        Serial.println("          RTC SET <year>/<month>/<day> <hour>:<minute>:<second>");
        Serial.println("   Example: RTC SET 2026/06/28 15:30:45");
        return;
    }
    
    // Parse RTC subcommand
    int spaceIndex = params.indexOf(' ');
    String subcommand = (spaceIndex > 0) ? params.substring(0, spaceIndex) : params;
    String rtcParams = (spaceIndex > 0) ? params.substring(spaceIndex + 1) : "";
    
    if (subcommand == "GET") {
        // Read and display current RTC time
        String currentTime = board.getRTC();
        Serial.println("📅 Current RTC Time:");
        Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        Serial.printf("  🕐 %s\n", currentTime.c_str());
    }
    else if (subcommand == "SET") {
        // Parse SET command: RTC SET 2026/06/28 15:30:45
        // Split date and time parts
        int timeSpaceIndex = rtcParams.indexOf(' ');
        if (timeSpaceIndex < 0) {
            Serial.println("❌ Invalid RTC SET format!");
            Serial.println("   Usage: RTC SET <year>/<month>/<day> <hour>:<minute>:<second>");
            Serial.println("   Example: RTC SET 2026/06/28 15:30:45");
            return;
        }
        
        String datePart = rtcParams.substring(0, timeSpaceIndex);
        String timePart = rtcParams.substring(timeSpaceIndex + 1);
        
        // Parse date: YYYY/MM/DD
        int dateSlash1 = datePart.indexOf('/');
        int dateSlash2 = datePart.lastIndexOf('/');
        
        if (dateSlash1 < 0 || dateSlash2 < 0 || dateSlash1 == dateSlash2) {
            Serial.println("❌ Invalid date format! Use: YYYY/MM/DD");
            return;
        }
        
        int year = datePart.substring(0, dateSlash1).toInt();
        int month = datePart.substring(dateSlash1 + 1, dateSlash2).toInt();
        int day = datePart.substring(dateSlash2 + 1).toInt();
        
        // Parse time: HH:MM:SS
        int timeColon1 = timePart.indexOf(':');
        int timeColon2 = timePart.lastIndexOf(':');
        
        if (timeColon1 < 0 || timeColon2 < 0 || timeColon1 == timeColon2) {
            Serial.println("❌ Invalid time format! Use: HH:MM:SS");
            return;
        }
        
        int hour = timePart.substring(0, timeColon1).toInt();
        int minute = timePart.substring(timeColon1 + 1, timeColon2).toInt();
        int second = timePart.substring(timeColon2 + 1).toInt();
        
        // Validate date/time values
        if (year < 2000 || year > 2099 || month < 1 || month > 12 || 
            day < 1 || day > 31 || hour < 0 || hour > 23 || 
            minute < 0 || minute > 59 || second < 0 || second > 59) {
            Serial.println("❌ Invalid date or time values!");
            Serial.println("   Year:   2000-2099");
            Serial.println("   Month:  1-12");
            Serial.println("   Day:    1-31");
            Serial.println("   Hour:   0-23");
            Serial.println("   Minute: 0-59");
            Serial.println("   Second: 0-59");
            return;
        }
        
        // Set RTC
        Serial.println("📅 Setting RTC Time...");
        Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
        board.setRTC(year, month, day, hour, minute, second);
        
        // Verify the set time
        delay(100);
        String newTime = board.getRTC();
        Serial.printf("✅ RTC updated to: %s\n", newTime.c_str());
    }
    else {
        Serial.print("❌ Unknown RTC sub-command: '");
        Serial.print(subcommand);
        Serial.println("'");
        Serial.println("   Valid sub-commands: GET, SET");
    }
}

/***********************************************************************
 * HANDLE RESET COMMAND
 ***********************************************************************/
void handleResetCommand(void) {
    Serial.println("🔄 Resetting ESP32-S3 board...");
    Serial.flush();
    
    delay(500);
    
    // Perform software reset
    ESP.restart();
}
