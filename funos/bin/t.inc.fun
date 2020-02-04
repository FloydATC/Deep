
var i = 0; 

/* UNSUPPORTED
print("Prefix increment:\n");
  while ( i < 4 ) {
  print( -i, " -> ");
  print( "++i: ", ++i, " -> ", i, "\n");
}
*/

/* UNSUPPORTED
print("Prefix decrement:\n");
while ( i > 0 ) {
  print( "--i: ", --i, " -> ", i, "\n");
}
*/

print("Postfix increment:\n");
while ( i < 4 ) {
  print( "i++: ", i++, " -> ", i, "\n");
}

print("Postfix decrement:\n");
while ( i > 0 ) {
  print( "i--: ", i--, " -> ", i, "\n");
}


/* UNSUPPORTED
class A {}
var obj_a = A();
obj_a.attr = 0;
print("Attribute increment:\n");
  while ( obj_a.attr < 4 ) {
  print( obj_a.attr, " -> ");
  print( ".attr: ", obj_a.attr++, " -> ", obj_a.attr, "\n");
}
*/

/* UNSUPPORTED
class B {}
var obj_b = B();
obj_b.attr = 4;
print("Attribute increment:\n");
  while ( obj_b.attr > 0 ) {
  print( obj_b.attr, " -> ");
  print( ".attr: ", obj_b.attr--, " -> ", obj_b.attr, "\n");
}
*/




