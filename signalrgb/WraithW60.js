import { systeminfo } from "@SignalRGB/systeminfo";

// Wraith W60 RGB Keyboard Plugin for Signal RGB
// VID: 0x2E3C, PID: 0xC365, Interface: 2, Usage Page: 0xFF1B
// Uses device.write() (Output report / interrupt OUT)
//
// Underglow: ayri kanal olarak gosterilir (anakart fan header'i gibi)
// RAM Usage modu ile otomatik renk degisimi desteklenir

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

export function Size()        { return [30, 11]; }
export function DefaultPosition(){return [0, 0];}
export function DefaultScale(){return 10.0;}

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
    var u = 2;
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
          "values": ["Per-Key", "Backlight"], "default": "Per-Key" },
        { "property": "UnderglowMode", "label": "Underglow Mode", "type": "combobox",
          "values": ["Channel", "RAM Usage"], "default": "RAM Usage" }
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
    } catch (e) {}
    if (typeof LightingZone !== "undefined" && LightingZone) {
        return LightingZone;
    }
    return "Per-Key";
}

function getUnderglowMode() {
    if (typeof UnderglowMode !== "undefined" && UnderglowMode) {
        return UnderglowMode;
    }
    return "RAM Usage";
}

// RAM kullanim yuzdesine gore renk: Yesil(0%) -> Sari(50%) -> Kirmizi(100%)
function getRamColor() {
    try {
        var ramPercent = 50;
        if (typeof systeminfo !== "undefined" && systeminfo !== null) {
            if (typeof systeminfo.getMemoryUsage === "function") {
                ramPercent = systeminfo.getMemoryUsage();
            } else if (typeof systeminfo.memoryUsage === "number") {
                ramPercent = systeminfo.memoryUsage;
            } else if (typeof systeminfo.getRamUsage === "function") {
                ramPercent = systeminfo.getRamUsage();
            } else if (typeof systeminfo.getMemory === "function") {
                var mem = systeminfo.getMemory();
                if (mem && typeof mem.percentUsed === "number") {
                    ramPercent = mem.percentUsed;
                }
            }
        }
        ramPercent = Math.max(0, Math.min(100, ramPercent));

        var r, g;
        if (ramPercent < 50) {
            r = Math.round((ramPercent / 50) * 255);
            g = 255;
        } else {
            r = 255;
            g = Math.round(((100 - ramPercent) / 50) * 255);
        }
        return [r, g, 0];
    } catch (e) {
        return [0, 255, 0];
    }
}

var renderCallCount = 0;

export function Initialize() {
    device.SetLedLimit(233);
    device.addChannel("Underglow", 1);
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
        dlog("Render() #" + renderCallCount + " zone=" + zone + " underglowMode=" + getUnderglowMode());
    }

    sendPerKeyColors();
    device.pause(1);

    sendUnderglow();
    device.pause(1);

    if (zone === "Backlight" && renderCallCount % 30 === 0) {
        sendBacklight();
        device.pause(1);
    }
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

// Underglow: ayri kanaldan renk okur veya RAM Usage modu kullanir
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

    var rgb;
    if (getUnderglowMode() === "RAM Usage") {
        rgb = getRamColor();
    } else {
        // Kanaldan renk oku (component atanmissa)
        var ch = device.channel("Underglow");
        if (ch && ch.LedCount() > 0) {
            var colorData = ch.getColors("Inline", "RGB");
            if (colorData && colorData.length >= 3) {
                rgb = [colorData[0], colorData[1], colorData[2]];
            } else {
                rgb = [0, 0, 0];
            }
        } else {
            // Kanalda component yoksa canvas'tan oku
            rgb = [0, 255, 0];
        }
    }

    pkt[9]  = rgb[0];
    pkt[10] = rgb[1];
    pkt[11] = rgb[2];
    for (var i = 12; i < 65; i++) pkt.push(0);
    device.write(pkt, 65);
}

function sendPowerBar() {
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
    var rgb = [0, 0, 255];
    pkt[9]  = rgb[0];
    pkt[10] = rgb[1];
    pkt[11] = rgb[2];
    pkt[12] = 0x80;
    for (var i = 13; i < 65; i++) pkt.push(0);
    device.write(pkt, 65);
}

export function ImageUrl() {
    return "https://raw.githubusercontent.com/crowroser/WraithW60Library/master/signalrgb/klavye_square.png";
}

export function ConflictingProcesses() { return []; }
