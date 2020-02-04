
var px1 = rand()*320;
var py1 = rand()*200;
var pr1 = rand()*100; 

var cr1 = rand();
var cg1 = rand();
var cb1 = rand(); 

var px2 = rand()*320;
var py2 = rand()*200;
var pr2 = rand()*100; 

var cr2 = rand();
var cg2 = rand();
var cb2 = rand(); 

while (true) {
  for (var i=0; i<100; i++) {
    rgb(
      (cr2*i/100) + (cr1*(100-i)/100), 
      (cg2*i/100) + (cg1*(100-i)/100), 
      (cb2*i/100) + (cb1*(100-i)/100)
    );
    disc(
      (px2*i/100) + (px1*(100-i)/100), 
      (py2*i/100) + (py1*(100-i)/100), 
      (pr2*i/100) + (pr1*(100-i)/100)
    );


  }
  px1 = px2;
  py1 = py2;
  pr1 = pr2;
  px2 = rand()*320;
  py2 = rand()*200;
  pr2 = rand()*100; 

  cr1 = cr2;
  cg1 = cg2;
  cb1 = cb2;
  cr2 = rand();
  cg2 = rand();
  cb2 = rand(); 
  sleep(1);
}
