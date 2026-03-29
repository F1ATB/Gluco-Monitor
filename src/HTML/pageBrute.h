// language=JavaScript
//************************************************
// Page principale HTML et Javascript
//************************************************
const char *BruteHtml = R"====(
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
        }

        a {
            color: black;
            text-decoration: none;
        }

        a:hover {
            color: blue;
        }

        .json-key {
            color: #922;
        }

        .json-string {
            color: #080;
        }

        .json-number {
            color: #00f;
        }

        .json-boolean {
            color: #a52a2a;
        }

        .json-null {
            color: #777;
        }

        .json-toggle {
            cursor: pointer;
            font-weight: bold;
        }

        ul {
            list-style-type: none;
            padding-left: 20px;
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

        .Menudroite {
            text-align: right;
            width: 50%;
        }

        .Menugauche {
            text-align: left;
            display: inline-flex;
            width: 50%;
        }

        .MenuChoisi {
            border-radius: 6px;
            margin: 2px;
            border: inset 2px grey;
            background-color: lightgrey;
            padding: 2px;
        }
        .LeBas{
            display:flex;
            justify-content: space-between;
            margin-top:10px;
            }
    </style>
</head>

<body onload="Init();">
    <div class="top">
        <div class="Menugauche"><img src="/favicon.ico" />
            <h1>Gluco-Monitor</h1>
        </div>
        <div class="Menudroite">
            <div class="MiniMenu">
                <div><a href="/" data-i18n="Glucose">-Glyc-</a></div>
                <div class="MenuChoisi"><a href="/Brute" data-i18n="dataLibreview">-Libreview-</a></div>
                <div><a href="/OTA" data-i18n="Update">-M à jour-</a></div>
                <div><a href="/Restart" data-i18n="Restart">-Restart-</a></div>
            </div>
        </div>
    </div>


    
    <h2 data-i18n="DisplayArbo">-Affichage Arbo-</h2>
    <h4 data-i18n="Arbo">-arborescence-</h4>
    <h3 data-i18n="LogLibreview">-Log Libreview-</h3>
    <div id="LoginJSON"></div>
    <h3 data-i18n="ConnectLibreview">-ConnexionLibreview-</h3>
    <div id="ConnectionJSON"></div>
    <h3 data-i18n="GraphLibreview">Graphe de Libreview</h3>
    <div id="GraphJSON"></div>
    <div class="LeBas">
    <div>Version : <span id="version"></span></div>
    <div><a href="https://f1atb.fr">https://F1ATB.fr</a></div>
  </div>
    <script>
        function createNode(key, value) {
            const li = document.createElement("li");

            if (typeof value === "object" && value !== null) {
                const isArray = Array.isArray(value);

                const toggle = document.createElement("span");
                toggle.textContent = isArray ? "[ ]" : "{ }";
                toggle.className = "json-toggle";

                const keySpan = document.createElement("span");
                keySpan.className = "json-key";
                keySpan.textContent = key ? key + ": " : "";

                const container = document.createElement("ul");
                container.style.display = "none";

                toggle.onclick = () => {
                    container.style.display =
                        container.style.display === "none" ? "block" : "none";
                };

                li.appendChild(keySpan);
                li.appendChild(toggle);
                li.appendChild(container);

                for (let k in value) {
                    container.appendChild(createNode(k, value[k]));
                }

            } else {
                const keySpan = document.createElement("span");
                keySpan.className = "json-key";
                keySpan.textContent = key ? key + ": " : "";

                const valueSpan = document.createElement("span");

                if (typeof value === "string") {
                    valueSpan.className = "json-string";
                    valueSpan.textContent = `"${value}"`;
                } else if (typeof value === "number") {
                    valueSpan.className = "json-number";
                    valueSpan.textContent = value;
                } else if (typeof value === "boolean") {
                    valueSpan.className = "json-boolean";
                    valueSpan.textContent = value;
                } else if (value === null) {
                    valueSpan.className = "json-null";
                    valueSpan.textContent = "null";
                }

                li.appendChild(keySpan);
                li.appendChild(valueSpan);
            }

            return li;
        }

        function renderJSON(json, containerId) {
            const container = document.getElementById(containerId);
            container.innerHTML = "";
            const ul = document.createElement("ul");

            for (let key in json) {
                ul.appendChild(createNode(key, json[key]));
            }

            container.appendChild(ul);
        }
        //LoginJSON = "", GraphJSON = "",ConnectionJSON 
        function LoadLoginJSON() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    if (this.responseText.indexOf("lastName")) {// user reconnu
                        let obj = JSON.parse(this.responseText);
                        renderJSON(obj, "LoginJSON");
                    } else {
                        GH("LoginJSON", this.responseText);
                    }
                }

            };
            xhttp.open('GET', '/LoginJSON', true);
            xhttp.send();
        }
        function LoadConnectionJSON() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    if (this.responseText.indexOf("lastName")) {// patient reconnu
                        let obj = JSON.parse(this.responseText);
                        renderJSON(obj, "ConnectionJSON");
                    } else {
                        GH("ConnectionJSON", this.responseText);
                    }
                }
            };
            xhttp.open('GET', '/ConnectionJSON', true);
            xhttp.send();
        }
        function LoadGraphJSON() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    if (this.responseText.indexOf("lastName")) {// patient reconnu
                        let obj = JSON.parse(this.responseText);
                        renderJSON(obj, "GraphJSON");
                    } else {
                        GH("GraphJSON", this.responseText);
                    }
                }
            };
            xhttp.open('GET', '/GraphJSON', true);
            xhttp.send();
        }

        function Init() {
            SetTraduction();
            LoadLoginJSON();
            LoadConnectionJSON();
            LoadGraphJSON();
            GH("version",Version);
        }

    </script>

</body>

</html>
)====";