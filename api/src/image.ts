import vision from '@google-cloud/vision'
import { Canvas, createImageData } from 'canvas'
import { clamp } from './util'
import './config'

const client = new vision.ImageAnnotatorClient()

function ycbcrToRGBA(y: number, cb: number, cr: number): number[] {
  const c = y - 16
  const d = cb - 128
  const e = cr - 128
  const r = clamp(1.164 * c + 1.596 * e, 0, 255)
  const g = clamp(1.164 * c + -0.392 * d + -0.813 * e, 0, 255)
  const b = clamp(1.164 * c + 2.017 * d, 0, 255)
  return [r, g, b, 255]
}

export async function constructImageData(buffer: Buffer): Promise<ImageData> {
  const ycbcr = [...buffer]
  if (ycbcr.length != 174 * 144 * 2) {
    throw new Error(`Incorrect byte length: ${ycbcr.length}`)
  }
  const pixels: number[] = []
  for (let i = 0; i < ycbcr.length; i += 2) {
    let y, cb, cr
    y = ycbcr[i * 2 + 1]
    cb = ycbcr[i * 2]
    cr = ycbcr[i * 2 + 2]
    pixels.push(...ycbcrToRGBA(y, cb, cr))
    y = ycbcr[i * 2 + 3]
    cb = ycbcr[i * 2]
    cr = ycbcr[i * 2 + 2]
    pixels.push(...ycbcrToRGBA(y, cb, cr))
  }
  const imageData = createImageData(new Uint8ClampedArray(pixels), 174, 144)
  return imageData
}

export async function objectLocalize(fileContent: Buffer) {
  const request = {
    image: { content: fileContent },
  }
  console.log('Localizing objects')
  const [result] = await client.objectLocalization(request)
  const objects = result.localizedObjectAnnotations
  for (const object of objects) {
    console.log(`Object ${object.name}, score ${object.score}`)
    for (const vertex of object.boundingPoly.normalizedVertices) {
      if (typeof vertex.x === 'undefined') {
        vertex.x = 0
      }
      if (typeof vertex.y === 'undefined') {
        vertex.y = 0
      }
    }
  }
  console.log('Objects localized')
  return objects
}

interface Vertex {
  x?: number
  y?: number
}

export async function drawBox(canvas: Canvas, vertices: Vertex[]) {
  const { width, height } = canvas
  const ctx = canvas.getContext('2d')
  ctx.lineWidth = 2
  ctx.strokeStyle = 'rgba(91,194,54,1)'
  ctx.beginPath()
  for (const { x, y } of vertices) {
    ctx.lineTo(x * width, y * height)
  }
  ctx.lineTo(vertices[0].x * width, vertices[0].y * height)
  ctx.stroke()
  console.log('Box drawn')
}
