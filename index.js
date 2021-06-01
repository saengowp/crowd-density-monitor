var cnt = document.getElementById('NumberCount')
function sleep(ms) {
  return new Promise((resolve) => setTimeout(resolve, ms))
}

async function Counter(newValue) {
  var start = Number(cnt.innerHTML) //get from recent count
  var end = newValue //get new count from storage
  if (start < end)
    for (var i = start; i <= end; i++) {
      await sleep(7)
      cnt.innerHTML = i.toString()
    }
  else {
    for (var i = start; i >= end; i--) {
      await sleep(7)
      cnt.innerHTML = i.toString()
    }
  }
}
Counter(50)

const delay = 2000
const clientID = '40a845fd-6f0d-4fdb-8ade-305b40b02ae8'
const token = 'VPGoWcFX6Ai1zmDC92yxMKAcJdEvXfp5'
async function updateData() {
  document.getElementById(
    'footage'
  ).src = `https://storage.googleapis.com/crowd-density-monitor/footage.jpg?t=${Date.now()}`
  const response = await fetch('https://api.netpie.io/v2/device/shadow/data', {
    headers: {
      Authorization: `Device ${clientID}:${token}`,
      'Content-Type': 'application/json',
    },
  })
  const data = await response.json()
  await Counter(data.data.peopleCount)
}

setInterval(updateData, delay)
