import time
import paho.mqtt.client as mqtt
import numpy as np

# MQTT setup
broker = "test.mosquitto.org"
port = 1883
topic = "vibrator/matrix"

client = mqtt.Client()

# Connect to MQTT broker
client.connect(broker, port)
client.loop_start()

try:
    while True:
        # # Generate a random 3x3 matrix with binary values
        # matrix = np.random.randint(2, size=(3, 3)).astype(str)
        
        # # Convert the matrix to a string format
        # matrix_str = "\n".join([" ".join(row) for row in matrix])

        # # Publish the matrix to the topic
        # client.publish(topic, matrix_str)
        # print("Published matrix:\n", matrix_str)

        # # Wait before publishing the next matrix
        # time.sleep(2)

        for i in range(3):
            for j in range(3):
                # Create a 3x3 zero matrix
                matrix = np.zeros((3, 3), dtype=int)
                
                # Turn ON one vibrator at position (i, j)
                matrix[i, j] = 1
                
                # Convert the matrix to a string format
                matrix_str = "\n".join([" ".join(map(str, row)) for row in matrix])

                # Publish the matrix to the topic
                client.publish(topic, matrix_str)
                print("Published matrix:\n", matrix_str)

                # Wait before activating the next vibrator
                time.sleep(5)
                matrix[i, j] = 0

except KeyboardInterrupt:
    print("Exiting...")
finally:
    client.loop_stop()
    client.disconnect()
