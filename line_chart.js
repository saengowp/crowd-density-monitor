google.charts.load('current', { packages: ['corechart'] })
google.charts.setOnLoadCallback(drawChart)

function drawChart(values) {
  console.log([['Time', 'Count']].concat(values))
  var data = google.visualization.arrayToDataTable(
    [['Time', 'Count']].concat(values)
  )

  var options = {
    //title: 'Company Performance',
    curveType: 'function',
    legend: { position: 'bottom' },
  }

  var chart = new google.visualization.LineChart(
    document.getElementById('curve_chart')
  )

  chart.draw(data, options)
}
