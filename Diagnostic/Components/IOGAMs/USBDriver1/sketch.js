function setup(){
  createCanvas(710, 400, WEBGL);
}

function draw(){
  background(250);
  
  translate(-240, 0, 0);
  normalMaterial();
  
  translate(240, variable, variable);
  push();
  rotateZ(frameCount * 0.01);
  rotateX(frameCount * 0.01);
  rotateY(frameCount * 0.01);
  box(70, 70, 70);
  pop();
  
  }