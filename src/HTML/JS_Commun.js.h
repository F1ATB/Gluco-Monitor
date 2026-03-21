// language=JavaScript
static const char JS_Commun[] PROGMEM = R"JS(
function GID(id) { return document.getElementById(id); }
function GH(id, T) {
    if (GID(id)) {
        GID(id).innerHTML = T;
    }
}
function GV(id, T) { GID(id).value = T; }
)JS";