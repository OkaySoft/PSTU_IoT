<!DOCTYPE html>
<html lang="ru">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Температура РИС-21-1БЗУ</title>
  <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.0.2/dist/css/bootstrap.min.css" rel="stylesheet">
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
  <div class="container" style="margin-top: 60px">
    <label for="unitCheckbox">Градусы Цельсии</label>
    <input type="checkbox" id="unitCheckbox" checked>
    <br><br>
    <button type="button" class="btn btn-primary mb-3" onclick="method = 'topToday'">Топ 10, день</button>
    <button type="button" class="btn btn-primary mb-3" onclick="method = 'topHour'">Топ 10, час</button>
    <button type="button" class="btn btn-primary mb-3" onclick="method = 'topMinute'">Топ 10, минута</button>
    <button type="button" class="btn btn-primary mb-3" onclick="method = 'last'">Последние 30</button>
    <br>
    <button type="button" class="btn btn-danger mb-3" onclick="apiRequest('stop')">Остановить</button>
    <button type="button" class="btn btn-success mb-3" onclick="apiRequest('start')">Запустить</button>
    <button type="button" class="btn btn-warning mb-3" onclick="apiRequest('clear')">Очистить всю историю</button>

    <?php $status = file_get_contents('status.conf') === 'on'; ?>
    <div class="alert alert-<?php echo($status ? 'success' : 'danger'); ?>" id="tempStatus" role="alert">
      Датчик температуры <?php echo($status ? 'включен' : 'выключен'); ?>
    </div>

    <div class="chart-container" style="position: relative; height:40vh; width:80vw">
        <canvas id="temperatureChart"></canvas>
    </div>

  </div>

  <script>
    let isCelsius = true;
    let method = 'last';

    function loadTemperatureData() {
      fetch('/api.php?'+method+'=1')
        .then(response => response.text())
        .then(data => {
          const temperatures = JSON.parse(data);
          const datas = [[], []];
          for (var i = 0; i < temperatures.length; i++) {
            datas[0].push(temperatures[i]['temperature']);
            datas[1].push(temperatures[i]['created_at']);
          }
          datas[0] = isCelsius ? datas[0] : datas[0].map(convertToFahrenheit);
          datas[0].reverse();
          datas[1].reverse();
          updateChart(datas);
        })
        .catch(error => console.error('Ошибка получения Ттемпературы:', error));
    }

    function updateChart(data) {
      const ctx = document.getElementById('temperatureChart').getContext('2d');
      
      if (window.myChart) {
        window.myChart.destroy();
      }

      window.myChart = new Chart(ctx, {
        type: 'line',
        data: {
          labels: data[1],
          datasets: [{
            label: `Температура (${isCelsius ? '°C' : '°F'})`,
            data: data[0],
            backgroundColor: 'rgba(75, 192, 192, 0.2)',
            borderColor: 'rgba(75, 192, 192, 1)',
            borderWidth: 1,
            fill: false,
          }]
        },
        options: {
          animations: {
            tension: {
              duration: 1000,
              easing: 'linear',
              from: 1,
              to: 0,
              loop: true
            }
          },
          scales: {
            y: {
              min: isCelsius ? 26 : 78,
              max: isCelsius ? 30 : 86
            }
          },
        }
      });
    }

    document.getElementById('unitCheckbox').addEventListener('change', function() {
      isCelsius = this.checked;
      loadTemperatureData();
    });

    loadTemperatureData();

    setInterval(loadTemperatureData, 5000);

    function convertToFahrenheit(celsius) {
      return (celsius * 9/5) + 32;
    }

    function apiRequest(method){
      fetch('/api.php?'+method+'=1');
      if (method == 'start'){
        document.getElementById('tempStatus').classList.remove('alert-danger');
        document.getElementById('tempStatus').classList.add('alert-success');
        document.getElementById('tempStatus').innerText = 'Датчик температуры включен';
      }
      else if (method == 'stop'){
        document.getElementById('tempStatus').classList.remove('alert-success');
        document.getElementById('tempStatus').classList.add('alert-danger');
        document.getElementById('tempStatus').innerText = 'Датчик температуры выключен';
      }
    }
  </script>
</body>
</html>