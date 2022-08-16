import paho.mqtt.client as mqtt
import datetime
import time
import mysql.connector

mydb = mysql.connector.connect(
    host='localhost',
    user='root',
    passwd='',
    database='database_local')

try:
    mydb2 = mysql.connector.connect(
        host='45.13.255.201',
        user='u7914443_usermelon2',
        passwd='usermelon2',
        database='u7914443_melon2')
except:
    print("internet error")

#IP Raspberry
MQTT_ADDRESS = '192.168.0.111'
#MQTT_USER = 'rippan'
#MQTT_PASSWORD = 'rippan'
MQTT_TOPIC = 'sensor'
MQTT_TOPIC2 = 'sensor2'

def on_connect(client, userdata, flags, rc):
    """ The callback for when the client receives a CONNACK response from the server."""
    print('Connected with result code ' + str(rc))
    client.subscribe(MQTT_TOPIC)
    client.subscribe(MQTT_TOPIC2)


def on_disconnect(client, userdata, rc):
    if rc != 0:
        print('Unexpected disconnect ' + str(rc))


def on_message(client, userdata, msg):
    """The callback for when a PUBLISH message is received from the server."""
    if (len(msg.payload.decode("utf-8")) > 0):
        x = datetime.datetime.now().strftime("%Y-%m-%d")
        y = datetime.datetime.now().strftime("%X")
        ph = 6
        mydb.reconnect()
        mycursor = mydb.cursor()
        data = msg.payload.decode("utf-8").split("#")
        print(data)
        
        if msg.topic == MQTT_TOPIC:
            #sensor
            sql = "INSERT INTO sensor_pembibitan (date, time, temp_udr, hum_udr, temp_tnh, hum_tnh, light, relay) VALUES(%s, %s, %s, %s, %s, %s, %s, %s)"
            sql2 = "UPDATE settings SET temp_udr_min = %s, hum_udr_min = %s, temp_tnh_min = %s, hum_tnh_min = %s, hum_tnh_max = %s WHERE type = 'pembibitan'"
            val = (x, y, data[0], data[1], data[2], data[3], data[4], data[5] )
            val2 = ( data[6], data[7], data[8], data[9], data[10] )       
            mycursor.execute(sql, val)
            mycursor.execute(sql2, val2)
        
            #sensor2
            sql3 = "INSERT INTO sensor_pertumbuhan (date, time, temp_udr, hum_udr, temp_tnh, hum_tnh, ph, light, relay) VALUES(%s, %s, %s, %s, %s, %s, %s, %s, %s)"
            #sql4 = "UPDATE settings SET temp_udr_min = %s, hum_udr_min = %s, temp_tnh_min = %s, hum_tnh_min = %s, hum_tnh_max = %s WHERE type = 'pertumbuhan'"
            val3 = (x, y, data[0], data[1], data[2], data[3], ph, data[4], data[5] )
            #val4 = ( data[6], data[7], data[8], data[8], data[9] )       
            mycursor.execute(sql3, val3)
            #mycursor.execute(sql4, val4)
        
        
        
        #Delay mengirim data ke database (second)
        time.sleep(60)
        mydb.commit()
        try:
            mydb2.reconnect()
            mycursor2 = mydb2.cursor()
            mycursor2.execute(sql, val)
            mycursor2.execute(sql2, val2)
            mycursor2.execute(sql3, val3)
            #mycursor2.execute(sql4, val4)

            mydb2.commit()
        except:
            print('internet error')


def main():
    mqtt_client = mqtt.Client()
    #mqtt_client.username_pw_set(MQTT_USER, MQTT_PASSWORD)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message
    mqtt_client.on_disconnect = on_disconnect
    mqtt_client.connect(MQTT_ADDRESS, 1883)
    mqtt_client.loop_forever()


if __name__ == '__main__':
    print('MQTT to InfluxDB bridge')
    main()

