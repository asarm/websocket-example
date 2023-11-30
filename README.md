# websocket-example

<h4>Youtube Demo Url: <a href="https://youtu.be/wEzvr50AKkM" target="_blank">https://youtu.be/wEzvr50AKkM</a></h4>

An app that simulates an IoT ecosystem. The server-side was written in C. It functions by periodically reading data from five different sensors across 30 different rooms. The server aggregates this data and sends it through a WebSocket. I developed a middleware that listens to the socket, and when new data arrives, it sends this information to the web application. The web app is written using Flask, JavaScript, and HTML. It lists the available rooms on the home page, and the room detail page displays the latest data of the selected room, including a real-time updating CO2 chart. 

Follow these steps to run locally:<br>
1) Run the web app (using "python web_app.py")<br>
2) Run the consumer side (using "python consumer.py")<br>
3) Run the server finally (use following command to build code "gcc -o server.exe main.c -lws2_32")<br>

<div>
  <img src="images/list.jpg" height="60%" width="80%"> </img>
  <img src="images/detail.jpg" height="60%" width="80%"> </img>
</div>
