import os
import sys
import serial
import logging
from queue import Queue 
from threading import Thread 
from datetime import datetime
from influxdb import InfluxDBClient

# Main method and loop
def main():
  init_logger()

  serial_client = open_serial_client()
  influxdb_client = open_influxdb_client()

  queue = Queue()

  t1 = Thread(target = serial_consumer, args =(serial_client, queue, ))
  t1.start()

  while True:
    message = queue.get()
    data = convert_message_to_data(message)
    if data is None: continue
    send_data_to_influxdb(influxdb_client, data)

def serial_consumer(serial_client, queue):
  while True:
    message = get_next_message(serial_client)
    queue.put(message)

# Initializes the logger
def init_logger():
  log_level_str = os.environ.get('LOG_LEVEL') or 'WARNING'
  log_level = getattr(logging, log_level_str)
  logging.basicConfig(level=log_level)
  # logging.basicConfig(filename='/var/log/wifi_analyzer.log', level=log_level)
  logging.info('Wifi Analyzer logging started')

# Opens the serial device client
def open_serial_client():
  baud_rate_str = os.environ.get('BAUD_RATE') or '576000'
  baud_rate = int(baud_rate_str)
  device = os.environ.get('DEVICE') or "/dev/tty.SLAB_USBtoUART"

  ser = serial.Serial()
  ser.baudrate = baud_rate
  ser.bytesize=serial.SEVENBITS
  ser.parity=serial.PARITY_EVEN
  ser.stopbits=serial.STOPBITS_ONE
  ser.xonxoff=0
  ser.rtscts=0
  ser.timeout=20
  ser.port=device

  try:
    ser.open()
  except:
    err = sys.exc_info()[0]
    logging.critical(f'Cannot open serial device {device}: {err}')
    sys.exit(f'Cannot open serial device {device}: {err}')

  return ser

def open_influxdb_client():
  host = os.environ.get('INFLUXDB_HOST') or 'localhost'
  port = int(os.environ.get('INFLUXDB_PORT') or '8086')
  user = os.environ.get('INFLUXDB_USER') or ''
  password = os.environ.get('INFLUXDB_PASSWORD') or ''
  database = os.environ.get('INFLUXDB_DATABASE')

  if database == None:
    logging.critical('No database set, use environment variable INFLUXDB_DATABASE to set it.')
    sys.exit('No database set, use environment variable INFLUXDB_DATABASE to set it.')

  try:
    client = InfluxDBClient('192.168.2.4', 8086, '', '', 'wifi')
    return client
  except:
    err = sys.exc_info()[0]
    logging.critical(f'Cannot open InfluxDB Client: {err}')
    sys.exit(f'Cannot open InfluxDB Client: {err}')

# Reads the next message from the serial device
def get_next_message(ser):
  try:
    raw_line = ser.readline()
    return str(raw_line, 'utf-8').strip()
  except:
    err = sys.exc_info()[0]
    logging.error(f'Cannot read from serial device: {err}')
    return None

def convert_message_to_data(message):
  if message is None: return None;
  data = {}
  parts = list(filter(None, message.split("|")))
  if len(parts) != 6:
    logging.critical(f'Incorrect message format: {message}')
    return None
  
  data['ssid'] = parts[0].strip()
  data['mac'] = parts[1].strip()
  data['up_down'] = parts[2].strip()
  data['total_bytes'] = int(parts[3].strip())
  data['packet_type'] = parts[4].strip()
  data['sequence_number'] = int(parts[5].strip())
  
  return data

def send_data_to_influxdb(client, data):
  influx_data = [
    {
      "measurement": "usage",
      "time": datetime.utcnow().isoformat(),
      "fields": data
    }
  ]
  client.write_points(influx_data)
  logging.info('Data send to InfluxDB')

main()