google.charts.load('current', { packages: ['bar'] })
google.charts.setOnLoadCallback(drawChart)

function drawChart() {
  var data = google.visualization.arrayToDataTable([
    ['', 'Max', 'Avg'],
    ['MON', 27, 20],
    ['TUE', 22, 17],
    ['WED', 46, 34],
    ['THU', 35, 28],
    ['FRI', 50, 44],
    ['SAT', 10, 4],
    ['SUN', 40, 28],
  ])

  var options = {
    chart: {
      // title: 'Company Performance',
      // subtitle: 'Sales, Expenses, and Profit: 2014-2017',
    },
    hAxis: {
      textStyle: {
        fontSize: 8, // or the number you want
      },
    },
  }

  var chart = new google.charts.Bar(
    document.getElementById('columnchart_material')
  )

  chart.draw(data, google.charts.Bar.convertOptions(options))
}
