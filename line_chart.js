google.charts.load("current", { packages: ["corechart"] });
google.charts.setOnLoadCallback(drawChart);

function drawChart() {
  var data = google.visualization.arrayToDataTable([
    ["Year", "Count"],
    ["1 May", 1000],
    ["2 May", 1170],
    ["3 May", 660],
    ["4 May", 1030],
  ]);

  var options = {
    //title: 'Company Performance',
    curveType: "function",
    legend: { position: "bottom" },
  };

  var chart = new google.visualization.LineChart(
    document.getElementById("curve_chart")
  );

  chart.draw(data, options);
}
