
var path = "/";

fun dir(p, depth) {
  var f;
  var dh = opendir(p);
  while (f = readdir(dh)) {
    var fname = p+"/"+f;
    if (fname.substr(0,2) == "//") { fname = fname.substr(1); }
    if (f == "." or f == "..") continue;
    if (file_type(fname) == "directory") {
      print("["+f+"]\n");
      dir(fname, depth+1);
    } else {
      print(f,"\t",file_size(fname),"\n");
    }
  }
  close(dh);
}

dir(path, 0);

