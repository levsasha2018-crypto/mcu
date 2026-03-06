import serial
import matplotlib.pyplot as plt


def read_value(ser):
    while True:
        try:
            line = ser.readline().decode('ascii').strip()
            uptime_ms, voltage_V, temp_C = map(float, line.split())
            return uptime_ms, voltage_V, temp_C
        except ValueError:
            continue


def main():
    ser = serial.Serial(port='COM8', baudrate=115200, timeout=0.0)

    if ser.is_open:
        print(f"Port {ser.name} opened")
    else:
        print(f"Port {ser.name} closed")

    measure_temperature_C = []
    measure_voltage_V = []
    measure_ts = []

    ser.write("tm_start\n".encode('ascii'))

    try:
        while True:
            uptime_ms, voltage_V, temp_C = read_value(ser)

            measure_ts.append(uptime_ms / 1000.0)
            measure_voltage_V.append(voltage_V)
            measure_temperature_C.append(temp_C)

            print(f'{voltage_V:.3f} V - {temp_C:.1f}C - {uptime_ms/1000.0:.2f}s')

    except KeyboardInterrupt:
        print("\nStopped by user")

    finally:
        ser.write("tm_stop\n".encode('ascii'))
        ser.close()
        print("Port closed")
        print(f"Collected points: {len(measure_ts)}")

        plt.subplot(2, 1, 1)
        plt.plot(measure_ts, measure_voltage_V)
        plt.title('График зависимости напряжения от времени')
        plt.xlabel('время, с')
        plt.ylabel('напряжение, В')

        plt.subplot(2, 1, 2)
        plt.plot(measure_ts, measure_temperature_C)
        plt.title('График зависимости температуры от времени')
        plt.xlabel('время, с')
        plt.ylabel('температура, C')

        plt.tight_layout()
        plt.show()


if __name__ == "__main__":
    main()
