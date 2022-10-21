/*Función para cambiar la imagen en base a la conexión WiFi*/
function actualizarPotencia(){
    var valor = parseInt(document.getElementById("valorPotencia").innerText);
    if (valor >= (-40)) {
        document.getElementById("imgPotencia").src = "https://raw.githubusercontent.com/MicaelaGordillo/images_IoT/main/intensidad_wifi/wifi4.png";
    } else if (valor <(-40) && valor >= (-50)) {
        document.getElementById("imgPotencia").src = "https://raw.githubusercontent.com/MicaelaGordillo/images_IoT/main/intensidad_wifi/wifi3.png";
    } else if (valor < (-50) && valor >= (-60)){
        document.getElementById("imgPotencia").src = "https://raw.githubusercontent.com/MicaelaGordillo/images_IoT/main/intensidad_wifi/wifi2.png";
    } else if (valor < (-60)){
        document.getElementById("imgPotencia").src = "https://raw.githubusercontent.com/MicaelaGordillo/images_IoT/main/intensidad_wifi/wifi1.png";
    }
}
/*Función para pedir el valor de la conexión WiFi - cada 10 segundos*/
setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            document.getElementById("valorPotencia").innerText = this.responseText;
            document.getElementById("valorRSSI").innerText = this.responseText;
            actualizarPotencia();
        }
    };
    xhttp.open("GET", "/RSSI", true);
    xhttp.send();
}, 10000);

/*Primera pestaña*/
/*Funcion que permite cambiar los iconos*/
function cambiarIconos(idValor, idImagen, idSwitch, ruta){
    if (document.getElementById(idValor).value == 0){
        document.getElementById(idImagen).src = 'https://raw.githubusercontent.com/MicaelaGordillo/images_IoT/main/intensidad_foco/intensidad1.png';
        document.getElementById(idValor).value = 1;
        document.getElementById(idSwitch).src ='http://esploradores.com/Practicas_html/encender_300x88.png';
    } else {
        document.getElementById(idImagen).src = 'https://raw.githubusercontent.com/MicaelaGordillo/images_IoT/main/intensidad_foco/intensidad5.png';
        document.getElementById(idValor).value = 0;
        document.getElementById(idSwitch).src ='http://esploradores.com/Practicas_html/apagar_300x88.png';
    }
    var xhttpCambio = new XMLHttpRequest();
    xhttpCambio.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
        console.log("todo bien");
        }
    };
    xhttpCambio.open("GET", "/"+ruta+document.getElementById(idValor).value, true);
    xhttpCambio.send();
}

/*Segunda pestaña*/
/*Función para actualizar los valores en base al slider o input*/
function updateTextInput(val, pwmInput, textInput) {
    document.getElementById(pwmInput).value = val; 
    document.getElementById(textInput).value = val; 
}

/*Tercera pestaña*/
/*Gráfica para el sensor LDR*/
var sensorLDR = new Highcharts.Chart({
    chart:{ 
        renderTo:'sensor-LDR' 
    },
    title: { 
        text: 'Sensor LDR' 
    },
    series: [{
      showInLegend: false,
      data: []
    }],
    plotOptions: {
      line: { animation: false,
        dataLabels: { enabled: true }
      },
      series: { color: '#18009c' }
    },
    xAxis: {
      type: 'datetime',
      dateTimeLabelFormats: { second:'%S' }
    },
    yAxis: {
      title: { text: 'Datos del Sensor LDR' }
    },
    credits: { enabled: false }
});
/*Función para pedir el valor del sensor LDR cada 1 segundo*/
setInterval(function () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var x = (new Date()).getTime(), y = parseFloat(this.responseText);
            if(sensorLDR.series[0].data.length > 40) {
                sensorLDR.series[0].addPoint([x, y], true, true, true);
            } else {
                sensorLDR.series[0].addPoint([x, y], true, false, true);
            }
        }
    };
    xhttp.open("GET", "/SensorLDR", true);
    xhttp.send();
}, 1000 );
