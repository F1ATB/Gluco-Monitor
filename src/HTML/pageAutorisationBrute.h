// language=JavaScript
//************************************************
// Page principale HTML et Javascript
//************************************************
const char *AutBruteHtml = R"====(
<!DOCTYPE html>
<html lang="fr">

<head>
    <meta charset="UTF-8">
    <title>Libreview DATA</title>
    <script src="/JS_Commun"></script>
    <script src="/JS_Traduction"></script>
    <style>
        body {
            background: #f5f5f5;
            font-family: Arial;
            text-align: center;
        }

        a {
            color: black;
            text-decoration: none;
        }

        .annul {
            font-size: 30px;
        }

        .top {
            width: 100%;
            display: flex;
        }

        .MiniMenu {
            display: flex;
            justify-content: space-around;
            margin: 10px;
            border: solid 4px lightblue;
            border-radius: 10px;
            padding: 4px;
            background-color: #d2dbe0;
            font-weight: bold;
        }

        .MiniMenu div {
            padding-top: 10px;
        }

        #Menudroite {
            text-align: right;
            width: 50%;

        }

        .Menugauche {
            text-align: left;
            display: inline-flex;
            width: 50%;
        }

        .LeBas {
            display: flex;
            justify-content: space-between;
            margin-top: 10px;
        }
    </style>
</head>

<body onload="Init();">
    <div class="top">
        <div class="Menugauche"><img src="/favicon.ico" />
            <h1>Gluco-Monitor</h1>
        </div>
        <div id="Menudroite">
            <div class="MiniMenu">
                <div><a href="/" data-i18n="Glucose">&nbsp;</a></div>
                <div><a href="/Brute" data-i18n="dataLibreview">&nbsp;</a></div>
                <div><a href="/OTA" data-i18n="Update">&nbsp;</a></div>
                <div><a href="/Restart" data-i18n="Restart">&nbsp;</a></div>
            </div>
        </div>
    </div>
    <h1>Gluco-Monitor Libreview DATA</h1>
    <h2 data-i18n="AutoOnMonitor">&nbsp; </h2>
    <button class="annul" type="button" onclick="window.location.href='/'"
        data-i18n="Cancel">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</button>

    <div class="LeBas">
        <div>Version : <span id="version"></span></div>
        <div><a href="https://f1atb.fr">https://F1ATB.fr</a></div>
    </div>
    <script>

        function ReLoad() {
            location.reload();
        }
        setInterval(ReLoad, 2500);
        function Init() {
            SetTraduction();
            GH("version",Version);
        }

    </script>

</body>

</html>
)====";