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

client.on('connect', (connack) => {
  client.subscribe('@msg/footage', { qos: 1 }, (err, granted) => {
    if (err) {
      throw err
    }
    console.log('MQTT - Subscribed to "@msg/footage"!')
  })
  console.log('MQTT - Connected to MQTT!')
})

client.on('message', (topic, message) => {
  console.log(`Recieved ${message.toString()}`)
})

export { client }
