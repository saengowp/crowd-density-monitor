google.charts.load("current", { packages: ["corechart"] });
google.charts.setOnLoadCallback(drawChart);

function drawChart() {
  var data = google.visualization.arrayToDataTable([
    ["Year", "Count"],
    ["15:00", 28],
    ["16:00", 32],
    ["17:00", 40],
    ["Now", 18],
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
