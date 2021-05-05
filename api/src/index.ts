import express, { Request, Response } from 'express'
const app = express()
const port = 3000

app.get('/', (req: Request, res: Response) => {
  res.send('Crowd Density Monitor API')
})

app.listen(port, () => {
  console.log(`App running on http://localhost:${port}`)
})
