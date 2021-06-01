var cnt = document.getElementById("NumberCount");
function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms));
}
async function Counter() {
  for (var i = 0; i <= 68; i++) {
    await sleep(15);
    cnt.innerHTML = i.toString();
  }
}
Counter();
