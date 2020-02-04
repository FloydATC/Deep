
/*
 Example: substr()

 Note that in FUN, substr() counts
 UTF8 characters, *NOT* bytes
 For bytes, use sub() instead
*/  

var u8c = chr(927); // Greek capital letter Omicron
var u8s = chr(959); // Greek small letter Omicron

print("This should print '"+u8s+"k' 8 times:\n");

// Skip 0 characters (pointless call)
print( substr(""+u8s+"k", 0), "\n" );

// Skip 6 characters, return rest
print( substr("ERR"+u8c+"R:"+u8s+"k", 6), "\n" );

// Skip 6 characters, return max 2
print( substr("ERR"+u8c+"R:"+u8s+"k,ERR"+u8c+"R", 6, 2), "\n" );

// Skip 0 characters, return max 2
print( substr(""+u8s+"k,ERR"+u8c+"R", 0, 2), "\n" );

// Skip 0 characters, cut 6 characters off end
print( substr(""+u8s+"k,ERR"+u8c+"R", 0, -6), "\n" );

// Start 2 characters from end, return rest
print( substr("ERR"+u8c+"R:"+u8s+"k", -2), "\n" );

// Start 8 characters from end, return max 2
print( substr("ERR"+u8c+"R:"+u8s+"k,ERR"+u8c+"R", -8, 2), "\n" );

// Start 8 characters from end, cut 6 characters off end
print( substr("ERR"+u8c+"R:"+u8s+"k,ERR"+u8c+"R", -8, -6), "\n" );


