var myData = [],
  colors = ["#1695A3", "#ACF0F2"],
  width = 345,
  height = 320,
  padding = 45,
  barWidth = 8,
  outerPadding = 0.5,
  barPadding = 0.7;

for (var i = 0; i < 14; i++) {
  myData.push(Math.ceil(Math.random() * 100) + 20);
}

var colorScale = d3.scale
  .linear()
  .domain([0, d3.max(myData)])
  .range(colors);

var xScale = d3.scale
  .ordinal()
  .domain(d3.range(0, myData.length))
  .rangeBands([padding, width - padding], barPadding, outerPadding);

var yScale = d3.scale
  .linear()
  .domain([d3.max(myData), 0])
  .range([height - padding * 2, 0]);

var yAxisScale = d3.scale
  .linear()
  .domain([Math.round(d3.max(myData)), 0])
  .range([0, height - padding * 2]);

var xAxis = d3.svg.axis().scale(xScale).orient("bottom");

var yAxis = d3.svg.axis().scale(yAxisScale).orient("left").ticks(7);

var chart = d3
  .select("#chart")
  //.style("background", "rgb(240, 240, 240)")
  .append("svg")
  .attr("width", width)
  .attr("height", height);

var chartBars = chart
  .selectAll("rect")
  .data(myData)
  .enter()
  .append("rect")
  .attr("class", "chart-bar")
  .attr("width", function (d) {
    return xScale.rangeBand();
  })
  .attr("height", 0)
  .attr("fill", colorScale)
  .attr("x", function (d, i) {
    return xScale(i);
  })
  .attr("y", height - padding);

var labelSVG = chart
  .selectAll("svg")
  .data(myData)
  .enter()
  .append("svg")
  .attr("class", "chart-label-svg")
  .attr("width", 80)
  .attr("height", 30)
  .attr("x", function (d, i) {
    return xScale(i) + xScale.rangeBand();
  })
  .attr("y", function (d) {
    return height - yScale(d) - padding - 30;
  })
  .style("opacity", "0")
  .append("g");

labelSVG
  .append("rect")
  .attr("class", "chart-label-rect")
  .attr("width", 80)
  .attr("height", 30)
  .attr("x", 0)
  .attr("y", 0)
  .attr("fill", "white");

labelSVG
  .append("text")
  .attr("x", "50%")
  .attr("y", "50%")
  .attr("text-anchor", "middle")
  .attr("alignment-baseline", "middle")
  .attr("fill", "#1695A3")
  .text(function (d) {
    return d;
  });

var xAxisG = chart
  .append("g")
  .attr("class", "axis")
  .attr("transform", "translate(0," + (height - padding) + ")")
  .call(xAxis);

var yAxisG = chart
  .append("g")
  .attr("class", "axis")
  .attr("transform", "translate(" + padding + ", " + padding + ")")
  .call(yAxis);

var rectTransitions = chartBars
  .on("mouseenter", function (d, i) {
    d3.select(this).style("fill", d3.rgb(colorScale(d)).brighter(0.3));
    d3.selectAll(".chart-label-svg")
      .filter(function (e, j) {
        if (i === j) {
          return this;
        }
      })
      .style("opacity", "1.0");
  })
  .on("mouseleave", function (d, i) {
    d3.select(this).style("fill", d3.rgb(colorScale(d)));
    d3.selectAll(".chart-label-svg")
      .filter(function (e, j) {
        if (i === j) {
          return this;
        }
      })
      .style("opacity", "0");
  })
  .transition()
  .duration(1000)
  .delay(function (d, i) {
    return i * 15;
  })
  .ease("elastic")
  .attr("height", function (d) {
    return yScale(d);
  })
  .attr("y", function (d) {
    return height - yScale(d) - padding;
  });
