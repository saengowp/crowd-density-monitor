import express from 'express'
const app = express()
const port = 3000

app.get('/', (req, res) => {
  res.send('Crowd Density Monitor API')
})

app.listen(port, () => {
  console.log(`App running on http://localhost:${port}`)
})
