

print("listen\n");
var server = listen(9000,"tcp");
var clients = [];
while (!error(server)) {
  var msg_queue = [];
  var msg;

  // Check for new clients
  var client = accept(server);
  if (client) {
    clients.push(client);
    msg = client.base(10)+" joined\n";
    msg_queue.push(msg);
  } else {
    sleep(10);
  }

  for (var i=0; i<clients.length; i++) {
    if (msg = readln(clients[i])) {
      msg = clients[i].base(10) + ": " + msg;
      msg_queue.push(msg);
    }
  }

  if (msg_queue) {
    msg = msg_queue.join();
    print(msg);
    for (var i=0; i<clients.length; i++) {
      write(clients[i], msg);
    }
  }
  msg_queue = [];

}
print("server failed: ", error(server), "\n");
