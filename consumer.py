import socket
import struct
import requests

SERVER_PORT = 8080
SENSOR_DATA_SIZE = 2155
APP_PORT = 5000

# Function to unpack received data into SensorData structure
def unpack_sensor_data(data):
    sensor_data = {"rooms":{}}

    str_data = data.decode("utf-8")
    room_arrays = str_data.split("||") # room_name, co2, humidity, light, pir, temperature
    
    for room_data in room_arrays:
        room_data = room_data.split(",")
        room_data = [s.replace('\x00', '') for s in room_data] # remove null character if exists

        sensor_data["rooms"][room_data[0]] = {
                "co2": room_data[1],
                "humidity": room_data[2],
                "light": room_data[3],
                "pir": room_data[4],
                "temperature": room_data[5]
            }
    return sensor_data

# Function to handle the client connection
def handle_client(client_socket, client_address):
    try:
        while True:
            # Receive data from the client
            data = client_socket.recv(SENSOR_DATA_SIZE)
            if not data:
                break

            # Unpack the received data into SensorData structure
            try:
                sensor_data = unpack_sensor_data(data)
                response = requests.post(f"http://127.0.0.1:{APP_PORT}", json=sensor_data)
            except:
                print("START THE WEB SERVER")

            room_names = sensor_data["rooms"].keys()

            for room in room_names:
                room_data = sensor_data["rooms"][room]
                # print(f"{room}\tCO2={room_data['co2']}, Humidity={room_data['humidity']}")

    except Exception as e:
        print(f"Error: {e}")

    finally:
        # Close the client socket
        client_socket.close()
        print(f"Connection with {client_address} closed.")

def main():
    # Create a socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        # Bind the socket to a specific address and port
        server_socket.bind(('127.0.0.1', SERVER_PORT))

        # Listen for incoming connections
        server_socket.listen(1)
        print(f"Server listening on port {SERVER_PORT}")

        # Accept a connection from a client
        while True:
            client_socket, client_address = server_socket.accept()
            print(f"Accepted connection from {client_address}")

            # Handle the client connection in a separate function
            handle_client(client_socket, client_address)

    except KeyboardInterrupt:
        print("Server terminated by the user.")

    finally:
        # Close the server socket
        server_socket.close()

if __name__ == "__main__":
    main()