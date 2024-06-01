Note: remember one thing. we can't check it through the postman without running the server. after running the server only like flask and python django we can test it with the postman.

//Step-1 first make a flask server which sends the static data status="success" and "data": "LED should toggle" to my esp32
from flask import Flask, jsonify
app = Flask(__name__)

@app.route('/esp32/boom/', methods=['GET'])
def handle_get():
    response = {
        "status": "success",
        "data": "LED should toggle"
    }
    return jsonify(response)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=8000)

//Step-2 esp32.ino code micropython
from machine import Pin
from time import sleep
import network
import urequests as requests
import ujson as json

device_id = "Device0001"
ssid = "Redmi10"
password = "abcd1234"

HTTPS_GET_URL = "http://192.168.178.76:8000/esp32/boom/"        #use your system ip

LED_PIN = 2  # GPIO 2 for the built-in LED

def setup():
    global led
    led = Pin(LED_PIN, Pin.OUT)
    setup_wifi()

def loop():
    response = HTTPS_GET(HTTPS_GET_URL)
    if response and response.get("status") == "success":
        led.on()
        sleep(1)
        led.off()

    sleep(1)

def setup_wifi():
    sta_if = network.WLAN(network.STA_IF)
    if not sta_if.isconnected():
        print('Connecting to network...')
        sta_if.active(True)
        sta_if.connect(ssid, password)
        while not sta_if.isconnected():
            pass
    print('Network config:', sta_if.ifconfig())

def HTTPS_GET(HTTPS_GET_URL):
    print("\nGetting from:", HTTPS_GET_URL)

    print("Connecting to server...")
    try:
        response = requests.get(HTTPS_GET_URL)
        if response.status_code == 200:
            print("ServerResponse:", response.text)
            return response.json()  # Parse and return the JSON response
        else:
            print("Failed to GET. Error:", response.text)
            return None
    except Exception as e:
        print("Exception occurred:", e)
        return None

setup()
while True:
    loop()

//Step-3 pizero w code in micropython
import requests
import json
from time import sleep
import RPi.GPIO as GPIO

device_id = "Device0001"
ssid = "Redmi10"
password = "abcd1234"

HTTPS_GET_URL = "http://192.168.1.56:8000/esp32/boom/"                       # remember always use the desktop wifi ipv4 address in this

LED_PIN = 2  # GPIO 2 for the built-in LED

def setup():
    global led
    GPIO.setmode(GPIO.BCM)  # Use Broadcom (BCM) pin numbering
    GPIO.setup(LED_PIN, GPIO.OUT)  # Set pin as output
    setup_wifi()

def loop():
    response = HTTPS_GET(HTTPS_GET_URL)
    if response and response.get("status") == "success":
        GPIO.output(LED_PIN, GPIO.HIGH)  # Turn on LED
        sleep(1)
        GPIO.output(LED_PIN, GPIO.LOW)  # Turn off LED

    sleep(1)

def setup_wifi():
    # Assuming the Raspberry Pi is already connected to Wi-Fi
    print(f'Connected to network {ssid}')

def HTTPS_GET(HTTPS_GET_URL):
    print("\nGetting from:", HTTPS_GET_URL)

    print("Connecting to server...")
    try:
        response = requests.get(HTTPS_GET_URL)
        if response.status_code == 200:
            print("ServerResponse:", response.text)
            return response.json()  # Parse and return the JSON response
        else:
            print("Failed to GET. Error:", response.text)
            return None
    except Exception as e:
        print("Exception occurred:", e)
        return None

setup()
while True:
    loop()
