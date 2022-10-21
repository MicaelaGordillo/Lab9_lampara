var hour = document.getElementById("hour");
var minute = document.getElementById("minute");
var seconds = document.getElementById("seconds");

var clock = setInterval(function time() {
  var date_now = new Date();
  var hr = date_now.getHours();
  var min = date_now.getMinutes();
  var sec = date_now.getSeconds();

  if (hr < 10) {
    hr = "0" + hr;
  }
  if (min < 10) {
    min = "0" + min;
  }
  if (sec < 10) {
    sec = "0" + sec;
  }

  hour.textContent = hr;
  minute.textContent = min;
  seconds.textContent = sec;
}, 1000);

function horario() {
  // Recolectamos datos
  var h1 = document.getElementById("horaEncendido").value;
  var m1 = document.getElementById("minutoEncendido").value;
  var h2 = document.getElementById("horaApagado").value;
  var m2 = document.getElementById("minutoApagado").value;

  //Trabajando con el tiempo actual
  var tiempoActual = new Date();
  var milis1 =
    new Date(
      tiempoActual.getFullYear(),
      tiempoActual.getMonth(),
      tiempoActual.getDate(),
      h1,
      m1,
      0,
      0
    ) - tiempoActual;
  var milis2 =
    new Date(
      tiempoActual.getFullYear(),
      tiempoActual.getMonth(),
      tiempoActual.getDate(),
      h2,
      m2,
      0,
      0
    ) - tiempoActual;

  if (milis1 < 0) {
    milis1 += 86400000; //Si la hora establecida de inicio ya pasó, se aplica desde el siguiente dia (86400000 = 24h)
  }
  if (milis2 < 0) {
    milis2 = 86400000; //Si la hora establecida de apagdo ya pasó, se aplica desde el siguiente dia (86400000 = 24h)
  }
  if (milis1 < milis2) {
    milis2 += 86400000;
  }

  //Establecemos los temporizadores para mandar señales al ESP32 en el momento en el que el foco deba cambiar de estado
  setInterval(function () {
    var xhttp = new XMLHttpRequest();
    xhttp.open("PUT", "/FocoEstado1", true);
    xhttp.send();
  }, milis1);
  
  setInterval(function () {
    var xhttp = new XMLHttpRequest();
    xhttp.open("PUT", "/FocoEstado0", true);
    xhttp.send();
  }, milis2);
}
