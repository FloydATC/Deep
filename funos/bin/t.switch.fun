
// This script tries to go through all legal permutations
// of the SWITCH..CASE statement

// Nonsense statements are permitted because the user
// may have commented out important cases for debugging

// These must all compile and each test must print "ok "

var i = 2;

while(true){

// Null switch; nonsense but legal
switch (i) {
}
print("ok ");


// Default only switch; also nonsense but legal
switch (i) {
  default: print("ok ");
}


// Single case; really just a cumbersome 'if' but legal
switch (i) {
  case 2: print("ok ");
}

//stop();

// Alternate cases with fallthrough
switch (i) {
  case 1: print("error ");
  case 2: print("ok ");
}

//stop(); 


// Alternate cases with fallthrough/break mix
switch (i) {
  case 1: print("error ");
  case 2: print("ok "); break; 
  case 3: print("error ");
}

//stop(); 

// Alternate cases with fallthrough/break mix and a default
switch (i) {
  case 1: print("error ");
  case 2: print("ok "); break; 
  case 3: print("error ");
  default: print("error ");
}

//stop();

// Hit default
switch (i) {
  case 1: print("error ");
  case 3: print("error ");
  default: print("ok ");
}

//stop();

// Fallthrough but no default
switch (i) {
  case 1:
  case 2:
  case 3: print("ok ");
}

//stop(); 

// Fallthrough but empty default
switch (i) {
  case 1:
  case 2:
  case 3: print("ok ");
  default: 
}

//stop();

// Fallthrough, then break to avoid default fallthrough
switch (i) {
  case 1:
  case 2:
  case 3: print("ok "); break;
  default: print("error ");
}

// Fallthrough, then break to avoid empty default fallthrough
switch (i) {
  case 1:
  case 2:
  case 3: print("ok "); break;
  default:
}

} // end while