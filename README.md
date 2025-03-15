# WATER ATM
Water ATM firmware for STM32F103RB on platform io.
## MQTT Communication Protocol

The following commands are used to configure the Water ATM via MQTT:

The following commands are used to configure the Water ATM via MQTT on the command line with the mosquitto client. Note that any client can be used, such as [MQTTBox](https://mqttbox.com). Here is a brief explanation of the arguments used in the commands:
- `-h`: Specifies the hostname or IP address of the MQTT broker.
- `-t`: Specifies the topic to which the message is published.
- `-m`: Specifies the message payload to be sent.
- `-u`: Specifies the username for authentication.
- `-P`: Specifies the password for authentication.

1. **Card Configuration:**
    ```sh
    mosquitto_pub -h <broker_address> -p 1883 -t "w/s/66dff49-57508886-67233757" -m '{"ev":"card_config","admin":"80 D1 BD 2B","service":"F3 79 B3 18"}' -u <username> -P <password>
    ```
    This command configures the admin and service cards for the Water ATM.

2. **Time Configuration:**
    ```sh
    mosquitto_pub -h <broker_address> -p 1883 -t "w/s/66dff49-57508886-67233757" -m '{"ev":"time_config","hour": 23 ,"minute": 35}' -u <username> -P <password>
    ```
    This command sets the time on the Water ATM to 23:35.

3. **Tariff Configuration:**
    ```sh
    mosquitto_pub -h <broker_address> -p 1883 -t "w/s/66dff49-57508886-67233757" -m '{"ev":"tariff_config","tariff1": 11 ,"tariff2": 10,"tariff3":10,"tariff4":10}' -u <username> -P <password>
    ```
    This command sets the tariffs for the Water ATM. tap1 is set to 11, and tap2, tap3, and tap4 are set to 10.

4. **Calibration Configuration:**
    ```sh
    mosquitto_pub -h <broker_address> -p 1883 -t "w/s/66dff49-57508886-67233757" -m '{"ev":"calibration_config","calib1":50,"calib2":50,"calib3":50,"calib4":50}' -u <username> -P <password>
    ```
    This command sets the calibration values for the Water ATM. Calibration values for the 4 taps are all set to 50.

5. **Payment Event:**
    ```sh
    mosquitto_pub -h <broker_address> -p 1883 -t "w/s/66dff49-57508886-67233757" -m '{"ev":"pay","a":"5","u":"1b853b01-f86c-4141-8efd-27afe2a05962"}' -u <username> -P <password>
    ```
    This command sends a payment event to the Water ATM with an amount of ksh 5 to the device of ID `1b853b01-f86c-4141-8efd-27afe2a05962`.