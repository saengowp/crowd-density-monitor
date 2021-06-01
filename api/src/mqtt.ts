import mqtt from 'mqtt'
import {
  MQTT_CLIENTID,
  MQTT_HOST,
  MQTT_PASSWORD,
  MQTT_PORT,
  MQTT_USERNAME,
} from './config'

console.log('MQTT - Connecting to MQTT...')
const client = mqtt.connect(MQTT_HOST, {
  clientId: MQTT_CLIENTID,
  username: MQTT_USERNAME,
  password: MQTT_PASSWORD,
  port: MQTT_PORT,
})

const footageTopic = '@msg/footage'
const shadowUpdateTopic = '@shadow/data/update'
const shadowUpdateResponseTopic = '@shadow/data/updated'

export function updatePeopleCount(peopleCount: number): void {
  client.publish(
    shadowUpdateTopic,
    JSON.stringify({
      data: { peopleCount },
    })
  )
}

export async function messageHandler(topic: string, message: Buffer) {
  if (topic === shadowUpdateResponseTopic) {
    console.log('Updated shadow data')
    console.log(JSON.parse(message.toString()))
  }
}

client.on('error', (error) => {
  console.error(error)
})

client.on('connect', (connack) => {
  client.subscribe(
    [footageTopic, shadowUpdateResponseTopic],
    { qos: 2 },
    (err, _) => {
      if (err) {
        throw err
      }
      console.log(
        `MQTT - Subscribed to "${footageTopic}", "${shadowUpdateResponseTopic}"!`
      )
    }
  )
  console.log('MQTT - Connected to MQTT!')
})

client.on('message', messageHandler)

export { client }
