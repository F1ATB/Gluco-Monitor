//************************************************
// Page principale HTML et Javascript
//************************************************
const char *MainHtml = R"====(
<!DOCTYPE html>
<html>

<head>
    <meta charset="UTF-8">
    <style>
        body {
            background: #111;
            color: white;
            font-family: Arial;
            text-align: center;
        }

        a {
            color: white;
            text-decoration: none;
        }

        svgGauge {
            width: 320px;
        }

        .zone {
            fill: none;
            stroke-width: 50;
        }

        #aiguille {
            fill: white;
        }

        #SvgGauge {

            width: 80%;
        }

        #SvgGraphe {
            width: 50%;
        }

        #SvgFlecheTendance {
            width: 50%;
        }

        .graduationD {
            font-size: 14px;
            fill: white;
            text-anchor: end;
        }

        .graduationM {
            font-size: 14px;
            fill: white;
            text-anchor: middle;
        }

        .graduationG {
            font-size: 14px;
            fill: white;
            text-anchor: start;
        }

        .flex3c {
            display: flex;
        }

        .w50p {
            width: 50%;
        }

        .w25p {
            width: 25%;
        }

        #lAge {
            position: relative;
            top: 50%;
            font-size: 30px;
        }

        .MiniMenu {
            text-align: right;
            font-size: 30px;
        }
    </style>
    <title>Gluco-Monitor</title>
    <script src="/JS_Commun"></script>
    <script src="/JS_Main"></script>
</head>

<body onload="init();">
    <div class="flex3c">
        <div id="svgTendance" class="w25p">

        </div>
        <div class="w50p">
            <svg viewBox="0 0 300 180" id="SvgGauge">

                <!-- zones couleur -->

                <path id="z1" class="zone" stroke="blue" />
                <path id="z2" class="zone" stroke="green" />
                <path id="z3" class="zone" stroke="orange" />
                <path id="z4" class="zone" stroke="red" />

                <!-- aiguille -->

                <polygon id="aiguille" points="150,150 150,150 150,150" />

                <!-- centre -->



                <!-- valeur -->

                <text id="valeur" x="150" y="150" font-size="60" text-anchor="middle" fill="white">0</text>

                <text id="mg" x="270" y="165" font-size="15" text-anchor="middle" fill="white">mg/dl</text>

            </svg>

        </div>
        <div class="w25p">
            <div class="MiniMenu"><a href="/Brute">...</a></div>
            <h1>Gluco-Monitor</h1>
            <div id="lAge"><span style="color:grey">Age glycémie</span></div>
        </div>
    </div>
    <div id="svgGraphe"></div>




</body>

</html>


)====";

const char *RestartHtml = R"====(
<!DOCTYPE html>
<html>

<head>
    <meta charset="UTF-8">
    <title>Restart</title>
</head>

<body>
    <h1>Restart</h1>
</body>

</html>

)====";

// icône panneaux solaire et soleil (Merci michy)
const char * Favicon = R"====(
<svg width="64" height="64" viewBox="0 20 180 90" xmlns="http://www.w3.org/2000/svg">
  <defs>
    <style>
      .gp { fill: none; stroke-width: 25; stroke-linecap: butt; }
    </style>
  </defs>
  <path class="gp" stroke="blue" d="M 22.5,80 A 67.5,67.5 0 0,1 42.27,32.27" />
  <path class="gp" stroke="green" d="M 42.27,32.27 A 67.5,67.5 0 0,1 90,12.5" />
  <path class="gp" stroke="orange" d="M 90,12.5 A 67.5,67.5 0 0,1 137.73,32.27" />
  <path class="gp" stroke="red" d="M 137.73,32.27 A 67.5,67.5 0 0,1 157.5,80" />
</svg>
)====";
// icône panneaux solaire et soleil 192pixels
const char * Favicon192 = R"====(
<svg width="192" height="192" viewBox="0 20 180 90" xmlns="http://www.w3.org/2000/svg">
  <defs>
    <style>
      .gp { fill: none; stroke-width: 25; stroke-linecap: butt; }
    </style>
  </defs>
  <path class="gp" stroke="blue" d="M 22.5,80 A 67.5,67.5 0 0,1 42.27,32.27" />
  <path class="gp" stroke="green" d="M 42.27,32.27 A 67.5,67.5 0 0,1 90,12.5" />
  <path class="gp" stroke="orange" d="M 90,12.5 A 67.5,67.5 0 0,1 137.73,32.27" />
  <path class="gp" stroke="red" d="M 137.73,32.27 A 67.5,67.5 0 0,1 157.5,80" />
</svg>
)====";
// Manifest pour Android
const char * Manifest = R"====(
{
  "name": "Routeur F1ATB",
  "short_name": "Routeur",
  "start_url": "/",
  "display": "standalone",
  "icons": [
    {
      "src": "/favicon192.ico",
      "sizes": "192x192",
      "type": "image/svg+xml"
    },
    {
      "src": "/favicon.ico",
      "sizes": "64x64",
      "type": "image/svg+xml"
    }
  ]
}
)====";