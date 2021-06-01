import { Storage } from '@google-cloud/storage'

const storage = new Storage({
  keyFilename: 'credentials.json',
})

export async function uploadFootage(buffer: Buffer) {
  const file = storage.bucket('crowd-density-monitor').file('footage.jpg')
  await file.save(buffer)
  console.log('Upload to GCS successful')
}
