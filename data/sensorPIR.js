/*Quinta pestaña*/
/*Gráfica para el sensor PIR*/
var sensorPIR = new Highcharts.Chart({
    chart:{ 
        renderTo:'sensor-PIR' 
    },
    title: { 
        text: 'Sensor PIR' 
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
      title: { text: 'Datos del Sensor PIR' }
    },
    credits: { enabled: false }
});
/*Función para pedir el valor del sensor PIR cada 1 segundo*/
setInterval(function () {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
            var x = (new Date()).getTime(), y = parseFloat(this.responseText);
            if(sensorPIR.series[0].data.length > 40) {
                sensorPIR.series[0].addPoint([x, y], true, true, true);
            } else {
                sensorPIR.series[0].addPoint([x, y], true, false, true);
            }
        }
    };
    xhttp.open("GET", "/SensorPIR", true);
    xhttp.send();
}, 1000 );