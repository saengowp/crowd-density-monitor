import { raw as bodyParserRaw } from 'body-parser'
import { createCanvas } from 'canvas'
import express, { Request, Response } from 'express'
import multer from 'multer'
import { PORT } from './config'
import { constructImageData, drawBox, objectLocalize } from './image'
import { updatePeopleCount } from './mqtt'
import { uploadFootage } from './storage'

const app = express()
const upload = multer({ storage: multer.memoryStorage() })
const port = PORT

app.get('/', (req: Request, res: Response) => {
  res.send('Crowd Density Monitor API')
})

app.post('/footage', bodyParserRaw(), async (req: Request, res: Response) => {
  const buffer = req.body
  const start = new Date()
  console.log(`${start} - Received footage `)

  const canvas = createCanvas(174, 144)
  const ctx = canvas.getContext('2d')

  const imageData = await constructImageData(buffer)
  ctx.putImageData(imageData, 0, 0)

  const people = (await objectLocalize(canvas.toBuffer())).filter(
    (object) => object.name === 'Person'
  )
  for (const person of people) {
    drawBox(canvas, person.boundingPoly.normalizedVertices)
  }

  updatePeopleCount(people.length)
  await uploadFootage(canvas.toBuffer())

  const timeSpent = new Date().getTime() - start.getTime()
  console.log(`Total time spent: ${timeSpent} ms`)
  res.json({
    peopleCount: people.length,
    timeSpent: `${timeSpent} ms`,
  })
})

app.listen(port, () => {
  console.log(`APP - App running on http://localhost:${port}`)
})
