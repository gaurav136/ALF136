import { WebSocketServer, WebSocket } from "ws";
let msgs = 0;
const wss = new WebSocketServer({ port: 8080 });
console.log("Server started");


wss.on("connection", function connection_callback(ws) {
  console.log("Connected :" + ws._socket.remoteAddress + ":" + ws._socket.remotePort)
  ws.on("message", function message_callback(data) {
    console.log(data.toString())
  });
});
