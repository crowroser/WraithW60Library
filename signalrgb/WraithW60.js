// Wraith W60 RGB Keyboard Plugin for Signal RGB
// VID: 0x2E3C, PID: 0xC365, Interface: 2, Usage Page: 0xFF1B
// Uses device.write() (Output report / interrupt OUT) â€” NOT device.send_report()
//
// FIX: device.color(x,y) bir [r,g,b] ARRAY dÃ¶ndÃ¼rÃ¼yor, packed integer DEÄÄ°L.
// Eski kod bit kaydÄ±rma (>>16 vs) yapÄ±yordu, bu array Ã¼zerinde NaN'a dÃ¼ÅŸÃ¼p
// her zaman (0,0,0) siyah Ã¼retiyordu. ArtÄ±k colorToRGB() ile doÄŸru okunuyor.

function dlog(msg) {
    try {
        if (typeof console !== "undefined" && console.log) { console.log("[WraithW60] " + msg); return; }
    } catch (e) {}
    try {
        if (typeof service !== "undefined" && service.log) { service.log("[WraithW60] " + msg); return; }
    } catch (e) {}
    try {
        if (typeof device !== "undefined" && device.log) { device.log("[WraithW60] " + msg); return; }
    } catch (e) {}
}

// device.color() farklÄ± SignalRGB sÃ¼rÃ¼mlerinde farklÄ± formatlar dÃ¶ndÃ¼rebiliyor:
// - [r,g,b] array (bizim durumumuzda bu)
// - {r,g,b} nesnesi
// - 0xRRGGBB packed integer
// Bu fonksiyon hepsini normalize ediyor.
function colorToRGB(c) {
    if (Array.isArray(c)) {
        return [c[0] | 0, c[1] | 0, c[2] | 0];
    }
    if (c && typeof c === "object") {
        if ("r" in c) return [c.r | 0, c.g | 0, c.b | 0];
        if (0 in c && 1 in c && 2 in c) return [c[0] | 0, c[1] | 0, c[2] | 0];
    }
    if (typeof c === "number") {
        return [(c >> 16) & 0xFF, (c >> 8) & 0xFF, c & 0xFF];
    }
    return [0, 0, 0];
}

export function Name()        { return "Wraith W60 Keyboard"; }
export function Publisher()   { return "WraithW60Library"; }
export function VendorId()    { return 0x2E3C; }
export function ProductId()   { return 0xC365; }
export function Type()        { return "hid"; }
export function DeviceType()  { return "keyboard"; }

export function Size()        { return [600, 240]; }

export function LedNames() {
    return [
        "Esc","1","2","3","4","5","6","7","8","9","0","Minus","Equal","Backspace",
        "Tab","Q","W","E","R","T","Y","U","I","O","P","Left Brace","Right Brace",
        "Caps Lock","A","S","D","F","G","H","J","K","L","Semicolon","Quote","Enter",
        "Left Shift","Z","X","C","V","B","N","M","Comma","Period","Slash","Right Shift",
        "Left Control","Left Windows","Left Alt","Space","Right Alt","Application","Right Control","Function"
    ];
}

export function LedPositions() {
    var u = 40;
    var p = [];
    var x;

    x = 0;
    p.push([x+u/2,u/2]);x+=u;
    for(var i=0;i<10;i++){p.push([x+u/2,u/2]);x+=u;}
    p.push([x+u/2,u/2]);x+=u;
    p.push([x+u/2,u/2]);x+=u;
    p.push([x+u,u/2]);x+=u*2;

    x = 0;
    p.push([x+u*0.75,u*1.5]);x+=u*1.5;
    for(var i=0;i<10;i++){p.push([x+u/2,u*1.5]);x+=u;}
    p.push([x+u/2,u*1.5]);x+=u;
    p.push([x+u/2,u*1.5]);x+=u;

    x = 0;
    p.push([x+u*0.875,u*2.5]);x+=u*1.75;
    for(var i=0;i<9;i++){p.push([x+u/2,u*2.5]);x+=u;}
    p.push([x+u/2,u*2.5]);x+=u;
    p.push([x+u/2,u*2.5]);x+=u;
    p.push([x+u*1.125,u*2.5]);x+=u*2.25;

    x = 0;
    p.push([x+u*1.125,u*3.5]);x+=u*2.25;
    for(var i=0;i<7;i++){p.push([x+u/2,u*3.5]);x+=u;}
    p.push([x+u/2,u*3.5]);x+=u;
    p.push([x+u/2,u*3.5]);x+=u;
    p.push([x+u/2,u*3.5]);x+=u;
    p.push([x+u*0.875,u*3.5]);x+=u*1.75;

    x = 0;
    p.push([x+u*0.625,u*4.5]);x+=u*1.25;
    p.push([x+u*0.625,u*4.5]);x+=u*1.25;
    p.push([x+u*0.625,u*4.5]);x+=u*1.25;
    p.push([x+u*3.125,u*4.5]);x+=u*6.25;
    p.push([x+u*0.625,u*4.5]);x+=u*1.25;
    p.push([x+u/2,u*4.5]);x+=u;
    p.push([x+u*0.625,u*4.5]);x+=u*1.25;
    p.push([x+u/2,u*4.5]);x+=u;

    return p;
}

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

function getLightingZone() {
    try {
        if (typeof device !== "undefined" && device.getProperty) {
            var v = device.getProperty("LightingZone");
            if (typeof v === "string" && v) return v;
            if (v && typeof v === "object") {
                if (typeof v.value === "string") return v.value;
                if (typeof v.toString === "function") {
                    var s = v.toString();
                    if (s && s !== "[object Object]") return s;
                }
            }
        }
    } catch (e) {
        dlog("getLightingZone(): threw: " + e);
    }
    if (typeof LightingZone !== "undefined" && LightingZone) {
        return LightingZone;
    }
    return "Per-Key";
}

var renderCallCount = 0;

export function Initialize() {
    sendTestPacket();
}

function sendTestPacket() {
    var pkt = [];
    pkt[0] = 0x01;
    pkt[1] = 0x07;
    pkt[2] = 0x00;
    pkt[3] = 0x00;
    pkt[4] = 0x00;
    pkt[5] = 0x0E;
    pkt[6] = 0x00;
    pkt[7] = 0x04;
    pkt[8] = 0x04;
    pkt[9] = 255;
    pkt[10] = 0;
    pkt[11] = 0;
    pkt[12] = 0;
    for (var i = 13; i < 65; i++) pkt.push(0);
    device.write(pkt, 65);
}

export function Render() {
    renderCallCount++;
    var zone = getLightingZone();

    if (renderCallCount <= 5 || renderCallCount % 120 === 0) {
        dlog("Render() #" + renderCallCount + " zone=" + zone);
    }

    if (zone === "Backlight") {
        sendBacklight();
    } else if (zone === "Underglow") {
        sendUnderglow();
    } else {
        sendPerKeyColors();
    }
    device.pause(1);
}

export function Shutdown(s) {
    sendAllBlack();
}

function sendAllBlack() {
    for (var c = 0; c < 8; c++) {
        var pkt = [];
        pkt[0] = 0x01;
        pkt[1] = 0x09;
        pkt[2] = 0x01;
        pkt[3] = 0x00;
        pkt[4] = c;
        pkt[5] = (c === 7) ? 0x12 : 0x36;
        for (var i = 6; i < 65; i++) pkt.push(0);
        device.write(pkt, 65);
    }
}

function sendPerKeyColors() {
    var colors = [];
    for (var i = 0; i < 132 * 3; i++) colors.push(0);

    var positions = LedPositions();
    var nonZeroCount = 0;
    var sampleRGB = null;

    for (var i = 0; i < PHYSICAL_MAP.length; i++) {
        var pi = PHYSICAL_MAP[i];
        var pos = positions[i];
        if (!pos) continue;

        var raw = device.color(pos[0], pos[1]);
        var rgb = colorToRGB(raw);

        if (rgb[0] !== 0 || rgb[1] !== 0 || rgb[2] !== 0) nonZeroCount++;
        if (sampleRGB === null) sampleRGB = rgb;

        colors[pi * 3]     = rgb[0];
        colors[pi * 3 + 1] = rgb[1];
        colors[pi * 3 + 2] = rgb[2];
    }

    if (renderCallCount <= 5 || renderCallCount % 120 === 0) {
        dlog("sendPerKeyColors(): nonZeroCount=" + nonZeroCount + "/" + PHYSICAL_MAP.length +
             " sampleRGB=" + JSON.stringify(sampleRGB));
    }

    for (var c = 0; c < 8; c++) {
        var pkt = [];
        pkt[0] = 0x01;
        pkt[1] = 0x09;
        pkt[2] = 0x01;
        pkt[3] = 0x00;
        pkt[4] = c;
        pkt[5] = (c === 7) ? 0x12 : 0x36;
        var keys = (c === 7) ? 6 : 18;
        var start = c * 18;
        for (var i = 0; i < keys; i++) {
            var g = start + i;
            pkt[6 + i * 3]     = colors[g * 3];
            pkt[6 + i * 3 + 1] = colors[g * 3 + 1];
            pkt[6 + i * 3 + 2] = colors[g * 3 + 2];
        }
        for (var i = pkt.length; i < 65; i++) pkt.push(0);
        device.write(pkt, 65);
    }
}

function sendBacklight() {
    var pkt = [];
    pkt[0] = 0x01;
    pkt[1] = 0x07;
    pkt[2] = 0x00;
    pkt[3] = 0x00;
    pkt[4] = 0x00;
    pkt[5] = 0x0E;
    pkt[6] = 0x00;
    pkt[7] = 0x04;
    pkt[8] = 0x04;
    var rgb = colorToRGB(device.color(300, 120));
    pkt[9]  = rgb[0];
    pkt[10] = rgb[1];
    pkt[11] = rgb[2];
    for (var i = 12; i < 65; i++) pkt.push(0);
    device.write(pkt, 65);
}

function sendUnderglow() {
    var pkt = [];
    pkt[0] = 0x01;
    pkt[1] = 0x08;
    pkt[2] = 0x00;
    pkt[3] = 0x00;
    pkt[4] = 0x00;
    pkt[5] = 0x0E;
    pkt[6] = 0x00;
    pkt[7] = 0x04;
    pkt[8] = 0x03;
    var rgb = colorToRGB(device.color(300, 120));
    pkt[9]  = rgb[0];
    pkt[10] = rgb[1];
    pkt[11] = rgb[2];
    for (var i = 12; i < 65; i++) pkt.push(0);
    device.write(pkt, 65);
}

export function ImageUrl() {
    return "https://raw.githubusercontent.com/crowroser/WraithW60Library/master/signalrgb/klavye.png";
}

export function ConflictingProcesses() { return []; }