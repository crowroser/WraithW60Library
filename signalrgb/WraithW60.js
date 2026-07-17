// Wraith W60 RGB Keyboard Plugin for Signal RGB
// VID: 0x2E3C, PID: 0xC365, Interface: 2, Usage Page: 0xFF1B

export function Name()        { return "Wraith W60 Keyboard"; }
export function Publisher()   { return "WraithW60Library"; }
export function VendorId()    { return 0x2E3C; }
export function ProductId()   { return 0xC365; }
export function Type()        { return "hid"; }

// Canvas: 600x240 pixels (15 units x 6 units, 40px per unit)
export function Size()        { return [600, 240]; }

// 60 keys total
export function LedNames() {
    return [
        // Row 0 (14 keys)
        "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "Minus", "Equal", "Backspace",
        // Row 1 (13 keys)
        "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "Left Brace", "Right Brace",
        // Row 2 (13 keys)
        "Caps Lock", "A", "S", "D", "F", "G", "H", "J", "K", "L", "Semicolon", "Quote", "Enter",
        // Row 3 (12 keys)
        "Left Shift", "Z", "X", "C", "V", "B", "N", "M", "Comma", "Period", "Slash", "Right Shift",
        // Row 4 (8 keys)
        "Left Control", "Left Windows", "Left Alt", "Space", "Right Alt", "Application", "Right Control", "Function"
    ];
}

// 60 positions - center of each key in pixels
export function LedPositions() {
    var u = 40; // unit = 40px
    var p = [];

    // Row 0: Esc(1u) + 10x1u + -(1u) + =(1u) + Bksp(2u) = 15u
    var x = 0;
    p.push([x+u/2, u/2]); x+=u;        // Esc
    for(var i=0;i<10;i++){p.push([x+u/2,u/2]);x+=u;} // 1-0
    p.push([x+u/2,u/2]); x+=u;          // -
    p.push([x+u/2,u/2]); x+=u;          // =
    p.push([x+u, u/2]); x+=u*2;         // Backspace 2u

    // Row 1: Tab(1.5u) + Q-P(10x1u) + [{(1u) + ]}(1u) + \(1u) = 14.5u
    x = 0;
    p.push([x+u*0.75, u*1.5]); x+=u*1.5; // Tab
    for(var i=0;i<10;i++){p.push([x+u/2,u*1.5]);x+=u;} // Q-P
    p.push([x+u/2,u*1.5]); x+=u;         // [{
    p.push([x+u/2,u*1.5]); x+=u;         // ]}
    p.push([x+u/2,u*1.5]); x+=u;         // \

    // Row 2: Caps(1.75u) + A-L(9x1u) + ;:(1u) + '"(1u) + Enter(2.25u) = 15u
    x = 0;
    p.push([x+u*0.875, u*2.5]); x+=u*1.75; // Caps
    for(var i=0;i<9;i++){p.push([x+u/2,u*2.5]);x+=u;} // A-L
    p.push([x+u/2,u*2.5]); x+=u;         // ;:
    p.push([x+u/2,u*2.5]); x+=u;         // '
    p.push([x+u*1.125,u*2.5]); x+=u*2.25; // Enter

    // Row 3: LShift(2.25u) + Z-M(7x1u) + ,<(1u) + .>(1u) + /?(1u) + RShift(1.75u) = 15u
    x = 0;
    p.push([x+u*1.125, u*3.5]); x+=u*2.25; // LShift
    for(var i=0;i<7;i++){p.push([x+u/2,u*3.5]);x+=u;} // Z-M
    p.push([x+u/2,u*3.5]); x+=u;         // ,<
    p.push([x+u/2,u*3.5]); x+=u;         // .>
    p.push([x+u/2,u*3.5]); x+=u;         // /?
    p.push([x+u*0.875,u*3.5]); x+=u*1.75; // RShift

    // Row 4: LCtrl(1.25u) + LWin(1.25u) + LAlt(1.25u) + Space(6.25u) + RAlt(1.25u) + Menu(1u) + RCtrl(1.25u) + Fn(1u) = 14.5u
    x = 0;
    p.push([x+u*0.625, u*4.5]); x+=u*1.25; // LCtrl
    p.push([x+u*0.625, u*4.5]); x+=u*1.25; // LWin
    p.push([x+u*0.625, u*4.5]); x+=u*1.25; // LAlt
    p.push([x+u*3.125, u*4.5]); x+=u*6.25; // Space
    p.push([x+u*0.625, u*4.5]); x+=u*1.25; // RAlt
    p.push([x+u/2, u*4.5]); x+=u;          // Menu
    p.push([x+u*0.625, u*4.5]); x+=u*1.25; // RCtrl
    p.push([x+u/2, u*4.5]); x+=u;          // Fn

    return p;
}

// Physical LED index mapping (verified via hardware testing)
var PHYSICAL_MAP = [
    22,23,24,25,26,27,28,29,30,31,32,33,34,36,
    44,45,46,47,48,49,50,51,52,53,54,55,56,
    66,68,69,70,71,72,73,74,75,76,77,78,80,
    88,90,91,92,93,94,95,96,97,98,99,100,
    110,111,112,116,119,120,121,122
];

export function Validate(endpoint) {
    return endpoint.interface === 2 && endpoint.usage_page === 0xFF1B;
}

export function ControllableParameters() {
    return [
        { "property": "LightingZone", "label": "Lighting Zone", "type": "combobox",
          "values": ["Per-Key", "Backlight", "Underglow"], "default": "Per-Key" }
    ];
}

export function Initialize() { sendAllBlack(); }

export function Render() {
    if (LightingZone === "Per-Key") sendPerKeyColors();
    else if (LightingZone === "Backlight") sendBacklight();
    else if (LightingZone === "Underglow") sendUnderglow();
}

export function Shutdown(s) { sendAllBlack(); }

function sendAllBlack() {
    for (var c = 0; c < 8; c++) {
        var pkt = [];
        pkt[0]=0x01; pkt[1]=0x09; pkt[2]=0x01; pkt[4]=c;
        pkt[5]=(c===7)?0x12:0x36;
        for (var i=pkt.length;i<64;i++) pkt.push(0);
        device.write(pkt, 64);
    }
}

function sendPerKeyColors() {
    var colors = [];
    for (var i=0;i<126*3;i++) colors.push(0);

    for (var i=0;i<PHYSICAL_MAP.length;i++) {
        var pi = PHYSICAL_MAP[i];
        var pos = LedPositions()[i];
        var color = device.color(pos[0], pos[1]);
        colors[pi*3]   = (color>>16)&0xFF;
        colors[pi*3+1] = (color>>8)&0xFF;
        colors[pi*3+2] = color&0xFF;
    }

    for (var c=0;c<8;c++) {
        var pkt=[];
        pkt[0]=0x01; pkt[1]=0x09; pkt[2]=0x01; pkt[4]=c;
        pkt[5]=(c===7)?0x12:0x36;
        var keys=(c===7)?6:18;
        var start=c*18;
        for (var i=0;i<keys;i++) {
            var g=start+i;
            pkt.push(colors[g*3], colors[g*3+1], colors[g*3+2]);
        }
        while(pkt.length<64) pkt.push(0);
        device.write(pkt, 64);
    }
}

function sendBacklight() {
    var pkt=[];
    pkt[0]=0x01; pkt[1]=0x07; pkt[5]=0x0E; pkt[6]=0x00; pkt[7]=0x04; pkt[8]=0x04;
    var color=device.color(300,120);
    pkt[9]=(color>>16)&0xFF; pkt[10]=(color>>8)&0xFF; pkt[11]=color&0xFF;
    while(pkt.length<64) pkt.push(0);
    device.write(pkt, 64);
}

function sendUnderglow() {
    var pkt=[];
    pkt[0]=0x01; pkt[1]=0x08; pkt[5]=0x0E; pkt[6]=0x00; pkt[7]=0x04; pkt[8]=0x03;
    var color=device.color(300,120);
    pkt[9]=(color>>16)&0xFF; pkt[10]=(color>>8)&0xFF; pkt[11]=color&0xFF;
    while(pkt.length<64) pkt.push(0);
    device.write(pkt, 64);
}

export function ConflictingProcesses() { return []; }