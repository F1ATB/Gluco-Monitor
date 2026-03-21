const char* OTAupdateHtml = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <title>OTA Update</title>
  <style>
    body { font-family: sans-serif; max-width: 480px; margin: 60px auto; padding: 0 1rem; }
    h1   { font-size: 1.4rem; margin-bottom: 1.5rem; }
    input[type=file] { display: block; margin-bottom: 1rem; }
    button { padding: .6rem 1.4rem; background: #333; color: #fff;
             border: none; border-radius: 6px; cursor: pointer; }
    #status { margin-top: 1rem; font-size: .9rem; color: #555; }
    progress { width: 100%; margin-top: .8rem; display: none; }
  </style>
</head>
<body>
  <h1>Mise à jour firmware OTA</h1>
  <form id="form">
    <input type="file" id="bin" accept=".bin" required>
    <button type="submit">Envoyer le firmware</button>
  </form>
  <progress id="prog" max="100" value="0"></progress>
  <p id="status"></p>

  <script>
    document.getElementById('form').onsubmit = async e => {
      e.preventDefault();
      const file = document.getElementById('bin').files[0];
      if (!file) return;

      const prog  = document.getElementById('prog');
      const status = document.getElementById('status');
      prog.style.display = 'block';
      status.textContent = 'Envoi en cours…';

      const xhr = new XMLHttpRequest();
      xhr.open('POST', '/update');

      xhr.upload.onprogress = ev => {
        if (ev.lengthComputable) {
          prog.value = Math.round(ev.loaded / ev.total * 100);
        }
      };

      xhr.onload = () => {
        if (xhr.status === 200) {
          status.textContent = 'Succès ! L\'ESP32 redémarre…';
        } else {
          status.textContent = 'Erreur : ' + xhr.responseText;
        }
      };

      const fd = new FormData();
      fd.append('firmware', file, file.name);
      xhr.send(fd);
    };
  </script>
</body>
</html>
)rawliteral";

