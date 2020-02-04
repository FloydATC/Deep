
// This script tests recursive function calls

// Yes, I know it's a terrible algorithm but 
// this is the standard recursion example

// If there is a problem with function calls,
// this code will probably trigger it.

fun fibonacci(n) {
  return (n<3 ? 1 : fibonacci(n-1) + fibonacci(n-2) );
}

var i = 0;
cursor(1);
while (true) {
  print( fibonacci(i++), "\n" );
}

