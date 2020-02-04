
var x = "error ";
fun outer() {
  var x = "ok ";

  fun inner() {
    print(x);
  }

  inner();
}

while (true) { 
  outer(); 
}
