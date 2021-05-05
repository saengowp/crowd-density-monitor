import express, { Request, Response } from 'express'
import './mqtt'

const app = express()
const port = 3000

app.get('/', (req: Request, res: Response) => {
  res.send('Crowd Density Monitor API')
})

app.listen(port, () => {
  console.log(`APP - App running on http://localhost:${port}`)
})
