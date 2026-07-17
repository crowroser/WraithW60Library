// Wraith W60 RGB Keyboard Plugin for Signal RGB
// VID: 0x2E3C, PID: 0xC365, Interface: 2, Usage Page: 0xFF1B
// Uses device.write() (Interrupt OUT) instead of device.send_report() (Feature Report)

export function Name()        { return "Wraith W60 Keyboard"; }
export function Publisher()   { return "WraithW60Library"; }
export function VendorId()    { return 0x2E3C; }
export function ProductId()   { return 0xC365; }
export function Type()        { return "hid"; }

// Canvas size: 15 units wide, 6 units tall (each unit = 40px)
export function Size()        { return [600, 240]; }

// Key names matching physical layout
export function LedNames() {
    return [
        // Row 0 (14 keys): Esc, 1-0, -, =, Backspace(2u)
        "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "Minus", "Equal", "Backspace",
        // Row 1 (13 keys): Tab(1.5u), Q-P, [{, ]}, \
        "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "Left Brace", "Right Brace",
        // Row 2 (13 keys): Caps(1.75u), A-L, ;:, '", Enter(2.25u)
        "Caps Lock", "A", "S", "D", "F", "G", "H", "J", "K", "L", "Semicolon", "Quote", "Enter",
        // Row 3 (12 keys): LShift(2.25u), Z-M, ,<, .>, /?, RShift(1.75u)
        "Left Shift", "Z", "X", "C", "V", "B", "N", "M", "Comma", "Period", "Slash", "Right Shift",
        // Row 4 (8 keys): LCtrl(1.25u), LWin(1.25u), LAlt(1.25u), Space(6.25u), RAlt(1.25u), Menu(1u), RCtrl(1.25u), Fn(1u)
        "Left Control", "Left Windows", "Left Alt", "Space", "Right Alt", "Application", "Right Control", "Function"
    ];
}

// LED positions - each key is positioned by its center in pixels
// Canvas is 600x240, each unit = 40px
// Standard 60% layout with proper key widths
export function LedPositions() {
    var u = 40; // unit size in pixels
    var positions = [];

    // Row 0: Esc(1u), 1-0(10x1u), -(1u), =(1u), Bksp(2u) = 14 keys
    // Total width: 1+10+1+1+2 = 15u = 600px
    var x = 0;
    positions.push([x + u/2, u/2]); x += u;      // Esc
    for (var i = 0; i < 10; i++) {                 // 1-0
        positions.push([x + u/2, u/2]);
        x += u;
    }
    positions.push([x + u/2, u/2]); x += u;       // -
    positions.push([x + u/2, u/2]); x += u;       // =
    positions.push([x + u, u/2]); x += u*2;       // Backspace (2u wide)

    // Row 1: Tab(1.5u), Q-P(10x1u), [{(1u), ]}(1u), \(1u) = 13 keys
    // Total width: 1.5+10+1+1+1 = 14.5u... need to adjust
    x = 0;
    positions.push([x + u*0.75, u*1.5]); x += u*1.5;  // Tab (1.5u)
    for (var i = 0; i < 10; i++) {                        // Q-P
        positions.push([x + u/2, u*1.5]);
        x += u;
    }
    positions.push([x + u/2, u*1.5]); x += u;            // [{
    positions.push([x + u/2, u*1.5]); x += u;            // ]}
    positions.push([x + u/2, u*1.5]); x += u;            // \

    // Row 2: Caps(1.75u), A-L(9x1u), ;:(1u), '"(1u), Enter(2.25u) = 13 keys
    // Total width: 1.75+9+1+1+2.25 = 15u
    x = 0;
    positions.push([x + u*0.875, u*2.5]); x += u*1.75;  // Caps (1.75u)
    for (var i = 0; i < 9; i++) {                         // A-L
        positions.push([x + u/2, u*2.5]);
        x += u;
    }
    positions.push([x + u/2, u*2.5]); x += u;            // ;:
    positions.push([x + u/2, u*2.5]); x += u;            // '
    positions.push([x + u*1.125, u*2.5]); x += u*2.25;  // Enter (2.25u)

    // Row 3: LShift(2.25u), Z-M(7x1u), ,<(1u), .>(1u), /?(1u), RShift(1.75u) = 12 keys
    // Total width: 2.25+7+1+1+1+1.75 = 14u... need 15u
    x = 0;
    positions.push([x + u*1.125, u*3.5]); x += u*2.25;  // LShift (2.25u)
    for (var i = 0; i < 7; i++) {                         // Z-M
        positions.push([x + u/2, u*3.5]);
        x += u;
    }
    positions.push([x + u/2, u*3.5]); x += u;            // ,<
    positions.push([x + u/2, u*3.5]); x += u;            // .>
    positions.push([x + u/2, u*3.5]); x += u;            // /?
    positions.push([x + u*0.875, u*3.5]); x += u*1.75;  // RShift (1.75u)

    // Row 4: LCtrl(1.25u), LWin(1.25u), LAlt(1.25u), Space(6.25u), RAlt(1.25u), Menu(1u), RCtrl(1.25u), Fn(1u) = 8 keys
    // Total width: 1.25+1.25+1.25+6.25+1.25+1+1.25+1 = 14.5u... adjust
    x = 0;
    positions.push([x + u*0.625, u*4.5]); x += u*1.25;  // LCtrl (1.25u)
    positions.push([x + u*0.625, u*4.5]); x += u*1.25;  // LWin (1.25u)
    positions.push([x + u*0.625, u*4.5]); x += u*1.25;  // LAlt (1.25u)
    positions.push([x + u*3.125, u*4.5]); x += u*6.25;  // Space (6.25u)
    positions.push([x + u*0.625, u*4.5]); x += u*1.25;  // RAlt (1.25u)
    positions.push([x + u/2, u*4.5]); x += u;            // Menu (1u)
    positions.push([x + u*0.625, u*4.5]); x += u*1.25;  // RCtrl (1.25u)
    positions.push([x + u/2, u*4.5]); x += u;            // Fn (1u)

    return positions;
}

// Physical LED index mapping (verified via hardware testing)
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
          "values": ["Per-Key", "Backlight", "Underglow"], "default": "Per-Key" }
    ];
}

// Initialize device
export function Initialize() {
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
    sendAllBlack();
}

// Send all-black using device.write() (Interrupt OUT)
function sendAllBlack() {
    for (var chunk = 0; chunk < 8; chunk++) {
        var packet = [];
        packet[0] = 0x01;  // Report ID
        packet[1] = 0x09;  // Per-key command
        packet[2] = 0x01;  // Sub-command
        packet[4] = chunk; // Chunk index
        packet[5] = (chunk === 7) ? 0x12 : 0x36; // Length
        // Rest is zeros (black)
        for (var i = packet.length; i < 64; i++) packet.push(0);
        device.write(packet, 64);
    }
}

// Send per-key colors using device.write() (Interrupt OUT)
function sendPerKeyColors() {
    var colors = new Array(126 * 3).fill(0);

    for (var i = 0; i < PHYSICAL_MAP.length; i++) {
        var physIdx = PHYSICAL_MAP[i];
        var pos = LedPositions()[i];
        var color = device.color(pos[0], pos[1]);

        var r = (color >> 16) & 0xFF;
        var g = (color >> 8) & 0xFF;
        var b = color & 0xFF;

        colors[physIdx * 3] = r;
        colors[physIdx * 3 + 1] = g;
        colors[physIdx * 3 + 2] = b;
    }

    for (var chunk = 0; chunk < 8; chunk++) {
        var packet = [];
        packet[0] = 0x01;  // Report ID
        packet[1] = 0x09;  // Per-key command
        packet[2] = 0x01;  // Sub-command
        packet[4] = chunk; // Chunk index
        packet[5] = (chunk === 7) ? 0x12 : 0x36; // Length

        var keysInChunk = (chunk === 7) ? 6 : 18;
        var startKey = chunk * 18;

        for (var i = 0; i < keysInChunk; i++) {
            var globalIdx = startKey + i;
            packet[6 + i * 3] = colors[globalIdx * 3];
            packet[6 + i * 3 + 1] = colors[globalIdx * 3 + 1];
            packet[6 + i * 3 + 2] = colors[globalIdx * 3 + 2];
        }

        for (var j = packet.length; j < 64; j++) packet.push(0);
        device.write(packet, 64);
    }
}

// Send backlight color using device.write()
function sendBacklight() {
    var packet = [];
    packet[0] = 0x01;  // Report ID
    packet[1] = 0x07;  // Backlight command
    packet[5] = 0x0E;  // Length
    packet[6] = 0x00;  // Mode: Static
    packet[7] = 0x04;
    packet[8] = 0x04;  // Zone type: Backlight

    var color = device.color(300, 120);
    packet[9] = (color >> 16) & 0xFF;
    packet[10] = (color >> 8) & 0xFF;
    packet[11] = color & 0xFF;

    for (var i = packet.length; i < 64; i++) packet.push(0);
    device.write(packet, 64);
}

// Send underglow color using device.write()
function sendUnderglow() {
    var packet = [];
    packet[0] = 0x01;  // Report ID
    packet[1] = 0x08;  // Underglow command
    packet[5] = 0x0E;  // Length
    packet[6] = 0x00;  // Mode: Static
    packet[7] = 0x04;
    packet[8] = 0x03;  // Zone type: Underglow

    var color = device.color(300, 120);
    packet[9] = (color >> 16) & 0xFF;
    packet[10] = (color >> 8) & 0xFF;
    packet[11] = color & 0xFF;

    for (var i = packet.length; i < 64; i++) packet.push(0);
    device.write(packet, 64);
}

// Conflicting software
export function ConflictingProcesses() {
    return [];
}