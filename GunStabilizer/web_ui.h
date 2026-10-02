#pragma once

static const char INDEX_HTML[] = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>火炮稳定器</title>
<style>
body{font-family:system-ui,sans-serif;background:#111;color:#eee;margin:0;padding:16px;max-width:540px;margin:0 auto}
h1{font-size:20px;text-align:center;margin:8px 0 16px}
.card{background:#1c1c1e;border-radius:12px;padding:16px;margin:12px 0}
.row{display:flex;align-items:center;justify-content:space-between;margin:10px 0}
.row span{font-size:15px}
label.ctl{display:block;margin:14px 0 4px;font-size:14px;color:#aaa}
.val{float:right;color:#38bdf8;font-variant-numeric:tabular-nums}
input[type=range]{width:100%;accent-color:#38bdf8}
input[type=checkbox]{width:20px;height:20px;accent-color:#38bdf8}
.grid{display:grid;grid-template-columns:1fr 1fr;gap:10px}
.stat{background:#2a2a2c;border-radius:10px;padding:12px;font-size:13px;color:#aaa}
.stat b{display:block;color:#38bdf8;font-size:20px;margin-top:4px;font-weight:600}
.hint{font-size:12px;color:#777;text-align:center;margin-top:12px}
</style>
</head>
<body>
<h1>坦克火炮稳定器</h1>
<div class="card">
  <div class="row"><span>WiFi 接管控制</span><input type="checkbox" id="wifiMode"></div>
  <div class="row"><span>稳定器使能</span><input type="checkbox" id="stabEn" checked></div>
  <label class="ctl">火炮俯仰 <span class="val" id="elevVal">0°</span></label>
  <input type="range" id="elev" min="-30" max="60" step="0.5" value="0">
  <label class="ctl">炮塔角度 <span class="val" id="travVal">0°</span></label>
  <input type="range" id="trav" min="-170" max="170" step="1" value="0">
</div>
<div class="card grid">
  <div class="stat">车体俯仰角<b id="pitch">0°</b></div>
  <div class="stat">车体横滚角<b id="roll">0°</b></div>
  <div class="stat">横摆角速度<b id="yaw">0°/s</b></div>
  <div class="stat">当前控制源<b id="mode">RC</b></div>
</div>
<div class="hint">俯仰 = 稳定后的惯性仰角；转向速度 = 炮塔相对车体的回转速率</div>
<script>
const $=id=>document.getElementById(id);
let t=null;
function send(){
  fetch('/set?e='+$('elev').value+'&t='+$('trav').value+'&mode='+($('wifiMode').checked?'wifi':'rc')+'&stab='+($('stabEn').checked?1:0));
}
function sched(){clearTimeout(t);t=setTimeout(send,60);}
$('elev').oninput=()=>{$('elevVal').textContent=$('elev').value+'°';sched();};
$('trav').oninput=()=>{$('travVal').textContent=$('trav').value+'°';sched();};
$('wifiMode').onchange=send;
$('stabEn').onchange=send;
async function poll(){
  try{
    const r=await fetch('/state');const s=await r.json();
    $('pitch').textContent=s.pitch.toFixed(1)+'°';
    $('roll').textContent=s.roll.toFixed(1)+'°';
    $('yaw').textContent=s.yaw.toFixed(1)+'°/s';
    $('mode').textContent=s.mode;
  }catch(e){}
}
setInterval(poll,150);poll();
</script>
</body>
</html>
)rawliteral";
