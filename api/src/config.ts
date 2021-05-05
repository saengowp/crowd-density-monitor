import dotenv from 'dotenv'

dotenv.config()

export const MQTT_HOST = process.env.MQTT_HOST
export const MQTT_CLIENTID = process.env.MQTT_CLIENTID
export const MQTT_USERNAME = process.env.MQTT_USERNAME
export const MQTT_PASSWORD = process.env.MQTT_PASSWORD
export const MQTT_PORT = parseInt(process.env.MQTT_PORT) || 1883
