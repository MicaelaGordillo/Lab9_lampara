var hour = document.getElementById("hour");
var minute = document.getElementById("minute");
var seconds = document.getElementById("seconds");

var clock = setInterval(
    function time(){
      var date_now = new Date();
      var hr = date_now.getHours();
      var min = date_now.getMinutes();
      var sec = date_now.getSeconds();
      
      if(hr < 10){
          hr = "0" + hr;
      } 
      if(min < 10){
          min = "0" + min;
      } 
      if(sec < 10){
          sec = "0" + sec;
      }

      hour.textContent = hr;
      minute.textContent = min;
      seconds.textContent = sec;



    },1000
);

function cambiarLed(color){
    var led = document.getElementById("led_status");
    led.src="https://raw.githubusercontent.com/MicaelaGordillo/Lab9_lampara/main/data/led_colores/"+color+"_led.png";
    document.getElementById('foco-imagen').src = 'https://raw.githubusercontent.com/MicaelaGordillo/Lab9_lampara/main/data/intensidad_foco/intensidad5.png';
    document.getElementById('estado-foco').value = 0;
    document.getElementById('switch-foco').src ='http://esploradores.com/Practicas_html/apagar_300x88.png';   
    var xhttp12 = new XMLHttpRequest();
    xhttp12.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
        console.log("todo bien");
        }
    };
    xhttp12.open("GET", "/FocoEstado"+document.getElementById('estado-foco').value, true);
    xhttp12.send(); 
    document.getElementById('foco-imagen2').src = 'https://raw.githubusercontent.com/MicaelaGordillo/Lab9_lampara/main/data/intensidad_foco/intensidad5.png';
    document.getElementById('estado-foco2').value = 0;
    document.getElementById('switch-foco2').src ='http://esploradores.com/Practicas_html/apagar_300x88.png';
    var xhttp22 = new XMLHttpRequest();
    xhttp22.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
        console.log("todo bien");
        }
    };
    xhttp22.open("GET", "/EstadoSensorLDR"+document.getElementById('estado-foco2').value, true);
    xhttp22.send();
    document.getElementById('foco-imagen3').src = 'https://raw.githubusercontent.com/MicaelaGordillo/Lab9_lampara/main/data/intensidad_foco/intensidad5.png';
    document.getElementById('estado-foco3').value = 0;
    document.getElementById('switch-foco3').src ='http://esploradores.com/Practicas_html/apagar_300x88.png';
    var xhttp42 = new XMLHttpRequest();
    xhttp42.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
        console.log("todo bien");
        }
    };
    xhttp42.open("GET", "/FocoEstado"+document.getElementById('estado-foco3').value, true);
    xhttp42.send();

    if (document.getElementById('#flagHorario').value == 1){
        document.getElementById('#flagHorario').value = 0;
        auxHorario = false;
        $("#establecerHorario").click();
    }
}

var auxHorario = true;
function horario(){
    if(auxHorario){
        document.getElementById('#flagHorario').value = 0;
        auxHorario = false;
    } else {
        document.getElementById('#flagHorario').value = 1;
        auxHorario = true;
    }
}