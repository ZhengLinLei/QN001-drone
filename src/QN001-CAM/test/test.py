# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import logging
import os

import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_blink(dut: IdfDut) -> None:
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'QN001-CAM.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('Size : {}KB'.format(bin_size // 1024))


'''
-------------------------------------------------------------------------
'''

# ESP32 Tester
import serial, sys, time

if len(sys.argv) < 9:
    print('Usage: python test.py <port> <ssid> <password> <dic> <key> <server> <port> <interval>')
    sys.exit(1) 

# UART
uart = serial.Serial(sys.argv[1], 115200)

def send_data(datos):
    uart.write(datos.encode())

def receive_data():
    while True:
        if uart.in_waiting > 0:
            data = uart.read(uart.in_waiting).decode()
            return data

# Test 1 - Bad command ------------------------------

send_data('000')
data = receive_data()

if data == '1':
    print('Test 1 - OK')


# Test 2 - Bad wifi ---------------------------------
send_data('00000000041;1;')
data = receive_data()

if data == '2':
    print('Test 2 - OK')


# Test 3 - Good -------------------------------------
send_data(f'0000{str(len(sys.argv[2])+len(sys.argv[3])+2).rjust(6, '0')}{sys.argv[2]};{sys.argv[3]};')

if receive_data() == '0':
    print('Test 3.1 - OK')


send_data(f'0001{str(len(sys.argv[4])+len(sys.argv[5])+2).rjust(6, '0')}{sys.argv[4]};{sys.argv[5]};')

if receive_data() == '0':
    print('Test 3.2 - OK')


send_data(f'0003{str(len(sys.argv[6])+len(sys.argv[7])+len(sys.argv[8])+2).rjust(6, '0')}{sys.argv[6]};{sys.argv[7]};')

if receive_data() == '0':
    print('Test 3.3 - OK')


time.sleep(30)

send_data('0002')

if receive_data() == '2':
    print('Test 3.4 - OK')

print('Tests finished')