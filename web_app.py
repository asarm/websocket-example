from flask import Flask, render_template, request, jsonify

app = Flask(__name__)

# Dictionary to store room data
rooms_data = {}
historical_data = {}

@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        # Get data from the JSON payload
        data = request.json
        update_rooms(data)
        return jsonify({"status": "success"})
    else:
        # Display the list of room names on the home page
        room_names = list(rooms_data.keys())
        return render_template('index.html', room_names=room_names)

@app.route('/<room_name>')
def room_detail(room_name):
    room_details = rooms_data.get(room_name, {})
    return render_template('room_detail.html', room_name=room_name, room_details=room_details)

@app.route('/api/room/<room_name>')
def api_room_detail(room_name):
    room_details = rooms_data.get(room_name, {})
    return jsonify(historical_data)

# Update the rooms_data dictionary with the received data
def update_rooms(data):
    global rooms_data
    rooms_data.update(data.get('rooms', {}))
    room_names = rooms_data.keys()

    for room_name in room_names:
        try:
            room_historic = historical_data[room_name]
            room_historic["co2"].append(rooms_data[room_name]["co2"])
            room_historic["temperature"].append(rooms_data[room_name]["temperature"])
            room_historic["humidity"].append(rooms_data[room_name]["humidity"])
            room_historic["light"].append(rooms_data[room_name]["light"])
            room_historic["pir"].append(rooms_data[room_name]["pir"])

            # keep only the last 10 value
            for key in room_historic.keys():
                room_historic[key] = room_historic[key][-10:]

            historical_data[room_name] = room_historic
        except:
            room_historic = {
                "co2": [rooms_data[room_name]["co2"]],
                "temperature": [rooms_data[room_name]["temperature"]],
                "humidity": [rooms_data[room_name]["humidity"]],
                "light": [rooms_data[room_name]["light"]],
                "pir": [rooms_data[room_name]["pir"]]}
            

            historical_data[room_name] = room_historic

    # print(historical_data["C319"])

if __name__ == '__main__':
    app.run(debug=True)
