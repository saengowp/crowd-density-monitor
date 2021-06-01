var cnt = document.getElementById("NumberCount");
function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

async function Counter() {
  var start = 0; //get from recent count
  var end = 68; //get new count from storage
  if (start < end)
    for (var i = start; i <= end; i++) {
      await sleep(15);
      cnt.innerHTML = i.toString();
    }
  else {
    for (var i = start; i >= end; i--) {
      await sleep(15);
      cnt.innerHTML = i.toString();
    }
  }
}
Counter();

var delayTime = 60000 * 1;
function updateData() {
  console.log("Update");
}
