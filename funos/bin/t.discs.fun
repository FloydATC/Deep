
var width = gl.width();
var height = gl.height();

var px1 = rand()*width;
var py1 = rand()*height;
var pr1 = rand()*100; 

var cr1 = rand();
var cg1 = rand();
var cb1 = rand(); 

var px2 = rand()*width;
var py2 = rand()*height;
var pr2 = rand()*100; 

var cr2 = rand();
var cg2 = rand();
var cb2 = rand(); 

while (getkey() != "[Escape]") {
  for (var i=0; i<100; i++) {
    gl.rgb(
      (cr2*i/100) + (cr1*(100-i)/100), 
      (cg2*i/100) + (cg1*(100-i)/100), 
      (cb2*i/100) + (cb1*(100-i)/100)
    );
    gl.disc(
      (px2*i/100) + (px1*(100-i)/100), 
      (py2*i/100) + (py1*(100-i)/100), 
      (pr2*i/100) + (pr1*(100-i)/100)
    );


  }
  px1 = px2;
  py1 = py2;
  pr1 = pr2;
  px2 = rand()*width;
  py2 = rand()*height;
  pr2 = rand()*100; 

  cr1 = cr2;
  cg1 = cg2;
  cb1 = cb2;
  cr2 = rand();
  cg2 = rand();
  cb2 = rand(); 
  sleep(1);
}
screen.clear();

