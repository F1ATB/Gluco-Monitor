// language=JavaScript
//************************************************
// Page principale HTML et Javascript
//************************************************
const char *BruteHtml = R"====(
<!DOCTYPE html>
<html lang="fr">

<head>
    <meta charset="UTF-8">
    <title>Visualisation JSON</title>
    <script src="/JS_Commun"></script>
    <style>
        body {
            background: #f5f5f5;
            font-family: Arial;
        }

        a {
            color: black;
            text-decoration: none;
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

        .MiniMenu {
            text-align: right;
            font-size: 30px;
        }
    </style>
</head>

<body onload="Init();">
    <div class="MiniMenu"><a href="/">...</a><br><a href="/Restart">Restart</a></div>
    <h1>Gluco-Monitor</h1>
    <h4>Cliquez sur {} pour ouvrir ou fermer l'arborescence</h4>
    <h2>Affichage Arborescent Messages Libreview JSON</h2>
    <h3>Login à Libreview</h3>
    <div id="LoginJSON"></div>
    <h3>Connexion à Libreview</h3>
    <div id="ConnectionJSON"></div>
    <h3>Graphe de Libreview</h3>
    <div id="GraphJSON"></div>

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
                    let obj = JSON.parse(this.responseText);
                    renderJSON(obj, "LoginJSON");
                }
            };
            xhttp.open('GET', '/LoginJSON', true);
            xhttp.send();
        }
        function LoadConnectionJSON() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    let obj = JSON.parse(this.responseText);
                    renderJSON(obj, "ConnectionJSON");
                }
            };
            xhttp.open('GET', '/ConnectionJSON', true);
            xhttp.send();
        }
        function LoadGraphJSON() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    let obj = JSON.parse(this.responseText);
                    renderJSON(obj, "GraphJSON");
                }
            };
            xhttp.open('GET', '/GraphJSON', true);
            xhttp.send();
        }

        function Init() {
            LoadLoginJSON();
            LoadConnectionJSON();
            LoadGraphJSON()
        }

    </script>

</body>

</html>
)====";