// Wraith W60 RGB Keyboard Plugin for Signal RGB
// VID: 0x2E3C, PID: 0xC365, Interface: 2, Usage Page: 0xFF1B

export function Name()        { return "Wraith W60 Keyboard"; }
export function Publisher()   { return "WraithW60Library"; }
export function VendorId()    { return 0x2E3C; }
export function ProductId()   { return 0xC365; }
export function Type()        { return "hid"; }
export function Size()        { return [15, 6]; } // 60% keyboard layout

// LED positions in canvas coordinates (15 columns x 6 rows)
export function LedNames() {
    return [
        // Row 0: Number row (14 keys)
        "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "Minus", "Equal", "Backspace",
        // Row 1: QWERTY row (13 keys)
        "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "Ğ", "Ü",
        // Row 2: Home row (13 keys)
        "CapsLock", "A", "S", "D", "F", "G", "H", "J", "K", "L", "Ş", "İ", "Enter",
        // Row 3: Shift row (12 keys)
        "Left Shift", "Z", "X", "C", "V", "B", "N", "M", "Ö", "Ç", "Slash", "Right Shift",
        // Row 4: Bottom row (8 keys)
        "Left Ctrl", "Left Win", "Left Alt", "Space", "Right Alt", "Menu", "Right Ctrl", "Fn"
    ];
}

export function LedPositions() {
    return [
        // Row 0
        [0,0], [1,0], [2,0], [3,0], [4,0], [5,0], [6,0], [7,0], [8,0], [9,0], [10,0], [11,0], [12,0], [13,0],
        // Row 1
        [0,1], [1,1], [2,1], [3,1], [4,1], [5,1], [6,1], [7,1], [8,1], [9,1], [10,1], [11,1], [12,1],
        // Row 2
        [0,2], [1,2], [2,2], [3,2], [4,2], [5,2], [6,2], [7,2], [8,2], [9,2], [10,2], [11,2], [12,2],
        // Row 3
        [0,3], [1,3], [2,3], [3,3], [4,3], [5,3], [6,3], [7,3], [8,3], [9,3], [10,3], [11,3],
        // Row 4
        [0,4], [1,4], [2,4], [3,4], [4,4], [5,4], [6,4], [7,4]
    ];
}

// Physical LED index mapping (verified via hardware testing)
// Maps logical LED index to physical HID chunk/key index
const PHYSICAL_MAP = [
    // Row 0: Number row
    22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 36,
    // Row 1: QWERTY row
    44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56,
    // Row 2: Home row
    66, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 80,
    // Row 3: Shift row
    88, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
    // Row 4: Bottom row
    110, 111, 112, 116, 119, 120, 121, 122
];

// Validate correct USB endpoint
export function Validate(endpoint) {
    return endpoint.interface === 2 && endpoint.usage_page === 0xFF1B;
}

// Controllable parameters
export function ControllableParameters() {
    return [
        { "property": "LightingZone", "label": "Lighting Zone", "type": "combobox",
          "values": ["Per-Key", "Backlight", "Underglow"], "default": "Per-Key" },
        { "property": "BacklightMode", "label": "Backlight Mode", "type": "combobox",
          "values": ["Static", "Breathe", "Wave", "Neon", "Sparkle", "Reactive", "Ripple"],
          "default": "Static" }
    ];
}

// Initialize device
export function Initialize() {
    // Send all-black to clear any previous state
    sendAllBlack();
}

// Called every frame (~30ms)
export function Render() {
    if (LightingZone === "Per-Key") {
        sendPerKeyColors();
    } else if (LightingZone === "Backlight") {
        sendBacklight();
    } else if (LightingZone === "Underglow") {
        sendUnderglow();
    }
}

// Shutdown handler
export function Shutdown(SystemSuspending) {
    if (SystemSuspending) {
        sendAllBlack();
    }
}

// Send all-black to clear state
function sendAllBlack() {
    for (let chunk = 0; chunk < 8; chunk++) {
        let packet = new Array(65).fill(0);
        packet[0] = 0x01;  // Report ID
        packet[1] = 0x09;  // Per-key command
        packet[2] = 0x01;  // Sub-command
        packet[4] = chunk; // Chunk index
        packet[5] = (chunk === 7) ? 0x12 : 0x36; // Length
        device.send_report(packet, 65);
    }
}

// Send per-key colors from canvas
function sendPerKeyColors() {
    // Build 126-key color array
    let colors = new Array(126 * 3).fill(0);

    for (let i = 0; i < PHYSICAL_MAP.length; i++) {
        let physIdx = PHYSICAL_MAP[i];
        let pos = LedPositions()[i];
        let color = device.color(pos[0], pos[1]);

        // Parse hex color
        let r = (color >> 16) & 0xFF;
        let g = (color >> 8) & 0xFF;
        let b = color & 0xFF;

        colors[physIdx * 3] = r;
        colors[physIdx * 3 + 1] = g;
        colors[physIdx * 3 + 2] = b;
    }

    // Send all 8 chunks
    for (let chunk = 0; chunk < 8; chunk++) {
        let packet = new Array(65).fill(0);
        packet[0] = 0x01;  // Report ID
        packet[1] = 0x09;  // Per-key command
        packet[2] = 0x01;  // Sub-command
        packet[4] = chunk; // Chunk index
        packet[5] = (chunk === 7) ? 0x12 : 0x36; // Length

        let keysInChunk = (chunk === 7) ? 6 : 18;
        let startKey = chunk * 18;

        for (let i = 0; i < keysInChunk; i++) {
            let globalIdx = startKey + i;
            packet[6 + i * 3] = colors[globalIdx * 3];
            packet[6 + i * 3 + 1] = colors[globalIdx * 3 + 1];
            packet[6 + i * 3 + 2] = colors[globalIdx * 3 + 2];
        }

        device.send_report(packet, 65);
    }
}

// Send backlight color
function sendBacklight() {
    let packet = new Array(65).fill(0);
    packet[0] = 0x01;  // Report ID
    packet[1] = 0x07;  // Backlight command
    packet[5] = 0x0E;  // Length
    packet[6] = 0x00;  // Mode: Static
    packet[7] = 0x04;
    packet[8] = 0x04;  // Zone type: Backlight

    // Get color from canvas center
    let color = device.color(7, 3);
    packet[9] = (color >> 16) & 0xFF;  // R
    packet[10] = (color >> 8) & 0xFF;  // G
    packet[11] = color & 0xFF;          // B

    device.send_report(packet, 65);
}

// Send underglow color
function sendUnderglow() {
    let packet = new Array(65).fill(0);
    packet[0] = 0x01;  // Report ID
    packet[1] = 0x08;  // Underglow command
    packet[5] = 0x0E;  // Length
    packet[6] = 0x00;  // Mode: Static
    packet[7] = 0x04;
    packet[8] = 0x03;  // Zone type: Underglow

    // Get color from canvas center
    let color = device.color(7, 3);
    packet[9] = (color >> 16) & 0xFF;  // R
    packet[10] = (color >> 8) & 0xFF;  // G
    packet[11] = color & 0xFF;          // B

    device.send_report(packet, 65);
}

// Conflicting software
export function ConflictingProcesses() {
    return [];
}

// Device image URL (optional)
export function ImageUrl() {
    return "";
}
