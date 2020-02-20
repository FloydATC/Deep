

fun http_get(hostname, port) {

  print("Connecting...\n");
  var conn = connect(hostname, port, "tcp");
  print("handle=", conn, "\n");

  // Send HTTP request
  write(conn, "GET / HTTP/1.0\r\nHost: "+hostname+"\r\n\r\n");

  // Write HTTP response header
  var line;
  while (!eof(conn)) {
    line = readln(conn);
    print(line);
    if (line == "\r\n") break; // Empty line == end of headers
  }
  
  // Close connection
  print("result=", error(conn), "\n");
  close(conn);
}


while (true) {
  http_get("www.atc.no", 80);
  sleep(3000);
}

