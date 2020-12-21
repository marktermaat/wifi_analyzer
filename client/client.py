import os
import sys
import serial
import logging
from datetime import datetime

# Main method and loop
def main():
  init_logger()

  serial_client = open_serial_client()

  while True:
    message = get_next_message(serial_client)
    data = convert_message_to_data(message)
    if data is None: continue
    logging.info(data)

# Initializes the logger
def init_logger():
  log_level_str = os.environ.get('LOG_LEVEL') or 'INFO'
  log_level = getattr(logging, log_level_str)
  logging.basicConfig(level=log_level)
  logging.info('Wifi Analyzer logging started')

# Opens the serial device client
def open_serial_client():
  baud_rate_str = os.environ.get('BAUD_RATE') or '115200'
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
    logging.critical(f'Cannot open serial device: {err}')
    sys.exit(f'Cannot open serial device: {err}')

  return ser

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


main()