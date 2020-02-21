

fun http_get(hostname, port) {

  var conn = connect(hostname, port, "tcp");
  if (error(conn)) {
    print("Connection failed: ", error(conn) ,"\n");
    return false;
  }

  // Send HTTP request
  write(conn, "GET / HTTP/1.0\r\nHost: "+hostname+"\r\n\r\n");

  // Get HTTP response header
  var line;
  while (!eof(conn)) {
    line = readln(conn);
    print(line);
    if (line == "\r\n") break; // Empty line == end of headers
  }

  // Close connection
  close(conn);

  return true;
}


var count = 1;
while (true) {
  var ok = http_get("www.atc.no", 80);
  print("Test ", count++, ": ", (ok ? "ok" : "FAILED"), "\n");
  print("="*40, "\n");
  sleep(3000);
}

