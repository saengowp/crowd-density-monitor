import { createCanvas } from 'canvas'
import express, { Request, Response } from 'express'
import multer from 'multer'
import { constructImageData, drawBox, objectLocalize } from './image'
import { updatePeopleCount } from './mqtt'
import { uploadFootage } from './storage'

const app = express()
const upload = multer({ storage: multer.memoryStorage() })
const port = 3000

app.get('/', (req: Request, res: Response) => {
  res.send('Crowd Density Monitor API')
})

app.post(
  '/footage',
  upload.single('image'),
  async (req: Request, res: Response) => {
    const buffer = req.file.buffer
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
  }
)

app.listen(port, () => {
  console.log(`APP - App running on http://localhost:${port}`)
})
