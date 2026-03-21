// language=JavaScript
//************************************************
// Page principale HTML et Javascript
//************************************************
const char *AutBruteHtml = R"====(
<!DOCTYPE html>
<html lang="fr">

<head>
    <meta charset="UTF-8">
    <title>Visualisation Libreview Data JSON</title>
    <style>
        body {
            background: #f5f5f5;
            font-family: Arial;
            text-align:center;
        }

        a {
            color: black;
            text-decoration: none;
        }

        .annul{
            font-size:30px;
        }
        .MiniMenu {
            text-align: right;
            font-size: 30px;
        }
    </style>
</head>

<body onload="Init();">
    <div class="MiniMenu"><a href="/">...</a></div>
    <h1>Gluco-Monitor Libreview DATA</h1>
    <h2>Authorize Access on Display Monitor</h2>
    <button class="annul" type="button" onclick="window.location.href='/'">Annuler</button>
    

    <script>
        
        function ReLoad(){
            location.reload();
        }
        setInterval(ReLoad, 2000);
    </script>

</body>

</html>
)====";