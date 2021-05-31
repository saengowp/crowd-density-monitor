google.charts.load("current", { packages: ["bar"] });
google.charts.setOnLoadCallback(drawChart);

function drawChart() {
  var data = google.visualization.arrayToDataTable([
    ["", "Max", "Avg"],
    ["MON", 1000, 400],
    ["TUE", 1170, 460],
    ["WED", 660, 1120],
    ["THU", 1030, 540],
    ["FRI", 1030, 540],
    ["SAT", 1030, 540],
    ["SUN", 1030, 540],
  ]);

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
  };

  var chart = new google.charts.Bar(
    document.getElementById("columnchart_material")
  );

  chart.draw(data, google.charts.Bar.convertOptions(options));
}
