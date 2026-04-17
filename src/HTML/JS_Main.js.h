// language=JavaScript
static const char *JS_Main = R"rawliteral(

const cx = 150
const cy = 150
const r = 125
let targetLow = 70;
let targetHigh = 180;
let Boucle1s = 0;
let lastGlyUnixTime = 0;

function polar(cx, cy, r, angle) {
    let rad = (angle - 90) * Math.PI / 180
    return {
        x: cx + r * Math.cos(rad),
        y: cy + r * Math.sin(rad)
    }
}

function arc(start, end) {

    let s = polar(cx, cy, r, end)
    let e = polar(cx, cy, r, start)

    return `M ${s.x} ${s.y} A ${r} ${r} 0 0 0 ${e.x} ${e.y}`
}


/* aiguille */

function setValue(v, unitLabel) { //Valeur de la Glycemie
    let x1, y1, x2, y2, x3, y3;
    let angle = -90 + (v * 180 / 400);
    let angle1 = -90;
    let angle2 = -90 + (targetLow * 180 / 400);
    GID("z1").setAttribute("d", arc(angle1, angle2));
    angle1 = angle2;
    angle2 = -90 + (targetHigh * 180 / 400);
    GID("z2").setAttribute("d", arc(angle1, angle2));
    angle1 = angle2;
    angle2 = -90 + (300 * 180 / 400);
    GID("z3").setAttribute("d", arc(angle1, angle2));
    GID("z4").setAttribute("d", arc(angle2, 90));

    let p = polar(cx, cy, 80, angle - 5);
    x1 = p.x;
    y1 = p.y;
    p = polar(cx, cy, 150, angle);
    x2 = p.x;
    y2 = p.y;
    p = polar(cx, cy, 80, angle + 5);
    x3 = p.x;
    y3 = p.y;
    let aig = document.getElementById("aiguille");
    aig.setAttribute("points", `${x1},${y1} ${x2},${y2} ${x3},${y3}`);
    
    // Format value based on unit - gauge uses mg/dL for positioning, display shows correct unit
    let displayValue;
    if (unitLabel === "mmol/L") {
        displayValue = (v / 18.0).toFixed(1);
    } else {
        displayValue = Math.round(v);
    }
    GID("valeur").textContent = displayValue;
}

/* demo animation */



function init() {

    /* zones */

    GID("z1").setAttribute("d", arc(-90, -45)); //Arc par defaut
    GID("z2").setAttribute("d", arc(-45, 0));
    GID("z3").setAttribute("d", arc(0, 45));
    GID("z4").setAttribute("d", arc(45, 90));

    GID("valeur").textContent=".....";
    SetTraduction();
    GH("version",Version);
    setInterval(Sequenceur1s, 1000);
    

}
function Sequenceur1s() {
    if (Boucle1s%10 == 0) LoadLGlycemie();
    if (Boucle1s == 1) LoadGrapheGlycemie();
    Boucle1s = (1 + Boucle1s) % 100;
    if (lastGlyUnixTime != 0) {
        let unix = Math.floor(Date.now() / 1000); //Heure en s
        let sec = unix - lastGlyUnixTime;
        let minutes = Math.floor(sec / 60);
        let secondes = sec % 60;
        let age = minutes + ":" + secondes.toString().padStart(2, '0');
        if (minutes>=10 && minutes<15) age="<span color='orange'>" + age + "</span>";
        if (minutes>=15 ) age="<span color='red'>" + age + "</span>";
        GH("lAge", age);
    }
}
function TraceGraphe(glucoseHeure,glucoseValues) {
    let X0 = 50;
    let Y0 = 180;
    let W = 450;
    let H = 160;
    let S = ``;
    let X,Y,Coul="",NewCoul="";
    

    S += `<svg viewBox="0 0 550 220" id="SvgGraphe">`;

    //==== Rectangles du fond ================
    S += `<text  class="graduationD" x=${X0 - 10}   y=${Y0 - H + 5}  >400</text>`;
    S += `<line x1=${X0} y1=${Y0 - H} x2=${X0-5} y2=${Y0 - H} style="stroke:white;stroke-width:2" />`;
    S += `<rect width=${W} height=${H} x=${X0} y=${Y0 - H}  fill="rgba(44, 1, 1, 0.8)" />`;
    let H1 = H * 300 / 400;
    S += `<text  class="graduationD" x=${X0 - 10}   y=${Y0 - H1 + 5}  >300</text>`;
    S += `<line x1=${X0} y1=${Y0 - H1} x2=${X0-5} y2=${Y0 - H1} style="stroke:white;stroke-width:2" />`;
    S += `<rect width=${W} height=${H1} x=${X0} y=${Y0 - H1}  fill="rgba(56, 36, 1, 0.8)" />`;
    H1 = H * targetHigh / 400;
    S += `<text  class="graduationD" x=${X0 - 10}   y=${Y0 - H1 + 5}  >` + targetHigh + `</text>`;
    S += `<line x1=${X0} y1=${Y0 - H1} x2=${X0-5} y2=${Y0 - H1} style="stroke:white;stroke-width:2" />`;
    S += `<rect width=${W} height=${H1} x=${X0} y=${Y0 - H1}  fill="rgba(0, 29, 0, 0.8)" />`;
    H1 = H * targetLow / 400;
    S += `<text  class="graduationD" x=${X0 - 10}   y=${Y0 - H1 + 6}  >` + targetLow + `</text>`;
    S += `<line x1=${X0} y1=${Y0 - H1} x2=${X0-5} y2=${Y0 - H1} style="stroke:white;stroke-width:2" />`;
    S += `<rect width=${W} height=${H1} x=${X0} y=${Y0 - H1}  fill="rgba(0,0,64,0.8" />`;
    //========Courbe=================
    let Hdeb=glucoseHeure[0];
    let Hfin=glucoseHeure[glucoseHeure.length-1];
    let deltaHeure=Hfin-Hdeb;
    
    if (deltaHeure>0){
        let T="";
        let d0 = new Date(glucoseHeure[0] * 1000);
        let h0 = d0.getHours();
        let d,h;
        for (let i=0;i<glucoseHeure.length;i++){
            let gluco=glucoseValues[i];
            Y=Math.round(Y0-gluco*H/400);
            X=Math.round(X0 + W*(glucoseHeure[i]-Hdeb)/deltaHeure);
            if (gluco<targetLow){
                NewCoul="blue";
            } else if (gluco<targetHigh){
                NewCoul="green";
            } else if (gluco<300){
                NewCoul="orange";
            } else {
                NewCoul="red";
            }
            if (Coul!=NewCoul){
                if(i>0) S+=` ${X},${Y} ${X},${Y0}" />`;
                S += `<polyline  style="fill:${NewCoul};" points="${X},${Y0}`;
                Coul=NewCoul;
            }
            S +=` ${X},${Y}`;
            //======Ticks de l'heure ====
            d = new Date(glucoseHeure[i] * 1000);
            h = d.getHours();
            if (h!=h0){
                T += `<line x1=${X} y1=${Y0} x2=${X} y2=${Y0+5} style="stroke:white;stroke-width:2" />`;
                T += `<text  class="graduationG" x=${X+2}   y=${Y0  + 15}  >${h}</text>`;
            }
            h0=h;
        }
        
        S +=` ${X},${Y0}" />` +T;
        
    }
    //=======AXES=====
    S += `<line x1=${X0} y1=${Y0} x2=${X0 + W} y2=${Y0} style="stroke:white;stroke-width:2" />`;
    S += `<line x1=${X0} y1=${Y0} x2=${X0} y2=${Y0 - H} style="stroke:white;stroke-width:2" />`;
    S += `</svg>`;
    GH("svgGraphe", S);
}
function TraceTendance(Tendance) { //Flèche
    let X0 = 30;
    let Y0 = 30;
    let p1, p2, p3;

    let S = ``;
    let Teta = 45 * (5 - Tendance); //1=bas vertical,5=haut vertical

    S += `<svg viewBox="0 0 60 60" id="SvgFlecheTendance">`;
    p1 = polar(X0, Y0, 20, Teta);
    p2 = polar(X0, Y0, 15, Teta + 90);
    p3 = polar(X0, Y0, 15, Teta - 90);

    S += `<polyline  points="${p1.x},${p1.y} ${p2.x},${p2.y} ${p3.x},${p3.y}" style="fill:white;" />`;
    p2 = polar(X0, Y0, 30, Teta + 170);
    p3 = polar(X0, Y0, 30, Teta - 170);
    S += `<polyline  points="${p1.x},${p1.y} ${p2.x},${p2.y} ${p3.x},${p3.y}" style="fill:white;" />`;

    S += `</svg>`;
    GH("svgTendance", S);
}
function LoadLGlycemie() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            let obj = JSON.parse(this.responseText);
            if (obj.GlycemieVal > 0) {
                targetLow = obj.targetLow;
                targetHigh = obj.targetHigh;
                lastGlyUnixTime = obj.lastGlyUnixTime;
                setValue(obj.GlycemieVal, obj.GlucoseUnitLabel);
                TraceTendance(obj.TrendArrow);
                // Display unit label if available
                if (obj.GlucoseUnitLabel) {
                    GH("unit", obj.GlucoseUnitLabel);
                }
            }
        }
    };
    xhttp.open('GET', '/ajaxGlycemie', true);
    xhttp.send();
}
function LoadGrapheGlycemie() {
    fetch("/dataGly")
        .then(r => r.arrayBuffer())
        .then(buffer => {

            const view = new DataView(buffer);

            // lire la taille
            const pointCountGly = view.getInt16(0, true);
            const glucoseHeure = new Uint32Array(buffer, 4, pointCountGly);
            // lire les valeurs
            const offset = 4 + pointCountGly * 4;
            const glucoseValues = new Int16Array(buffer, offset, pointCountGly);

            if(pointCountGly>10) TraceGraphe(glucoseHeure,glucoseValues);
        });
}

)rawliteral"; 