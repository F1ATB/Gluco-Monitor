const char* OTAupdateHtml = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
  <meta charset="UTF-8">
  <title>OTA Update</title>
  <script src="/JS_Commun"></script>
  <script src="/JS_Traduction"></script>
  <style>
    body {
      font-family: Arial;
      padding: 0 1rem;
    }

    a {
      color: black;
      text-decoration: none;
    }

    a:hover {
      color: blue;
    }

    h1 {
      font-size: 1.4rem;
      margin-bottom: 1.5rem;
    }

    input[type=file] {
      display: block;
      margin-bottom: 1rem;
    }

    button {
      padding: .6rem 1.4rem;
      background: #333;
      color: #fff;
      border: none;
      border-radius: 6px;
      cursor: pointer;
    }

    #status {
      margin-top: 1rem;
      font-size: .9rem;
      color: #555;
    }

    progress {
      width: 100%;
      margin-top: .8rem;
      display: none;
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

    .centre {
      text-align: center;
      margin: auto;
    }

    .liste {
      display: flex;
      justify-content: center;
      text-align: left;
    }

    .infoOTA {
      font-style: italic;
      font-size: smaller;
    }
    .bouton{
      margin:auto;
      margin-bottom: 10px;
    }
    .LeBas{
      display:flex;
      justify-content: space-between;
    }
  </style>
</head>

<body onload="init();">
  <div class="top">
    <div class="Menugauche"><img src="/favicon.ico" />
      <h1>Gluco-Monitor</h1>
    </div>
    <div class="Menudroite">
      <div class="MiniMenu">
        <div><a href="/" data-i18n="Glucose">-Glyc-</a></div>
                <div><a href="/Brute" data-i18n="dataLibreview" id="menuBrute">-Libreview-</a></div>
                <div class="MenuChoisi"><a href="/OTA" data-i18n="Update">-M à jour-</a></div>
                <div><a href="/Restart" data-i18n="Restart">-Restart-</a></div>
      </div>
    </div>
  </div>
  <div class="centre">
    <h1 data-i18n="UpdateOTA">-M Jour OTA-</h1>
    <div class="infoOTA">OTA = On The Air</div>
    <br>
    <div class="liste">
      <span data-i18n="VersionNow">Votre version actuelle de Gluco-Monitor : </span><span id="Version_actu"></span>
    </div>
    <br>
    <div class="liste" data-i18n="VersionDispo">
      -Version dispo-
    </div>
    <div class="liste">
      <iframe src="https://f1atb.fr/web_tool_GlucoMonit/scan_dir_bin.php" style="width:350px; height:150px"></iframe>
    </div>
    <div class="liste">
      <ul>
        <li data-i18n="Telecharge">1 - Téléchargez sur votre ordinateur, la version binaire du logiciel du Gluco-Monitor souhaitée
          <br>(Gluco-Monitor_x.x.x.bin) <span data-i18n="ClickBin"> -cliquant dessus-</span></li>
        <li data-i18n="SelectFile">--SelectFile--</li>
        <li ><span data-i18n="SendBin">3 - Cliquez sur </span>'<span data-i18n="SendBinBut">---</span>'</li>
      </ul>
    </div>
    <form id="form">
      <input class="bouton" type="file" id="bin" accept=".bin" required>
      
      <button type="submit" id="boutonSubmit" data-i18n="SendBinBut">Submit</button>
    </form>
    <progress id="prog" max="100" value="0"></progress>
    <p id="status"></p>
  </div>
  <div class="LeBas">
    <div>Version : <span id="version"></span></div>
    <div><a href="https://f1atb.fr">https://F1ATB.fr</a></div>
  </div>
  <script>
    document.getElementById('form').onsubmit = async e => {
      e.preventDefault();
      const file = document.getElementById('bin').files[0];
      if (!file) return;

      const prog = document.getElementById('prog');
      const status = document.getElementById('status');
      prog.style.display = 'block';
      status.textContent = 'Upload in progress…';

      const xhr = new XMLHttpRequest();
      xhr.open('POST', '/update');

      xhr.upload.onprogress = ev => {
        if (ev.lengthComputable) {
          prog.value = Math.round(ev.loaded / ev.total * 100);
        }
      };

      xhr.onload = () => {
        if (xhr.status === 200) {
          status.textContent = 'Success! The ESP32 is restarting…';
        } else {
          status.textContent = 'Error : ' + xhr.responseText;
        }
      };

      const fd = new FormData();
      fd.append('firmware', file, file.name);
      xhr.send(fd);
    };
    function init(){
       // Fetch sensor type and update menu label
       fetch('/ajaxGlycemie')
           .then(response => response.json())
           .then(data => {
               const isDexcom = data.sensorType === 1; // 1 = SENSOR_DEXCOM
               if (isDexcom) {
                   document.getElementById('menuBrute').setAttribute('data-i18n', 'dataDexcom');
               }
               SetTraduction();
           })
           .catch(error => {
               console.error('Error fetching sensor type:', error);
               SetTraduction();
           });
       GH("Version_actu",Version);
       GH("version",Version);
    }
  </script>
</body>

</html>
)rawliteral";