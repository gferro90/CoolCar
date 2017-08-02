

function setup(){
  createCanvas(canvasWidth, canvasHeight, WEBGL);
}

function draw(){
  background(250);
  
  normalMaterial();
 
  translate(0, -100, 0);
  if (keyIsDown(LEFT_ARROW)) {
    driveRef+=driveStep;
    driveGraphVal=driveRef*driveGraphFactor;
  } 
  if (keyIsDown(RIGHT_ARROW)) {
    driveRef-=driveStep;
    driveGraphVal=driveRef*driveGraphFactor;
  }
  if (keyIsDown(UP_ARROW)) {
    speedGraphVal+=speedGraphStep;
    speedRef+=speedStep;
  } 
  if (keyIsDown(DOWN_ARROW)) {
    speedGraphVal-=speedGraphStep;
    speedRef-=speedStep;
  } 
  if(!keyIsDown(UP_ARROW) && !keyIsDown(DOWN_ARROW)){
    if(speedRef<0){
       speedRef+=speedStep;
    }
    if(speedRef>0){
       speedRef-=speedStep;
    }
  }
  
  if(driveGraphVal>leftDriveSat){
      driveGraphVal=leftDriveSat;
  }
  if(driveGraphVal<rightDriveSat){
      driveGraphVal=rightDriveSat;
  }

  if(driveRef>driveRefSat){
      driveRef=driveRefSat;
  }
  if(driveRef<-driveRefSat){
      driveRef=-driveRefSat;
  }

  if(speedRef>speedRefSat){
      speedRef=speedRefSat;
  }
  if(speedRef<-speedRefSat){
      speedRef=-speedRefSat;
  }
  
  push();
  rotateX(carPitch);
  translate(0,0,carLength/2);
  
  rotateY(driveGraphVal);
//  rotateY(frameCount * 0.01);
   
     fill(255,0,0);
     strokeWeight(5.0);
     stroke(0,0,0);
     box(100, 50, carLength);
     push();
     translate(0,-25,0);
     fill(139,139,131);
     box(100, 2, carLength-10);
     pop();
     push();
     translate(-35,-30,-carLength/2+10);
     fill(255,215,0);
     sphere(10);
     pop();
     push();
     translate(35,-30,-carLength/2+10);
     fill(255,215,0);
     sphere(10);
     pop();
     
     translate(0,-35,47);
     fill(135,206,250);
     box(90, 30, carLength-120);
     translate(0,-15,0);
     fill(99,99,91);
     box(90, 2, carLength-120);
     translate(0,15,0);
     
     translate(0,35+20,-47);
  

     push();
     translate(-(carWidth+wheelHeight)/2,0,-carLength/2);
     rotateZ(3.14/2);
     //texture(wheelTexture);
     fill(0,0,0);
     rotateX(driveRef*driveFactor);
     rotateY(frameCount * speedGraphStep);
     cylinder(wheelRadius,wheelHeight);
     pop();

     push();
     translate((carWidth+wheelHeight)/2,0,-carLength/2);
     rotateZ(3.14/2);
     fill(0,0,0);
     rotateX(driveRef*driveFactor);
     rotateY(frameCount * speedGraphStep);
     cylinder(wheelRadius,wheelHeight);
     pop();
  
     push();
     translate(-(carWidth+wheelHeight)/2,0,carLength/2);
     rotateZ(3.14/2);
     fill(0,0,0);
     //rotateX(driveRef*driveFactor);
     rotateY(frameCount * speedGraphStep);
     cylinder(wheelRadius,wheelHeight);
     pop();

     push();
     translate((carWidth+wheelHeight)/2,0,carLength/2);
     rotateZ(3.14/2);
     fill(0,0,0);
     //rotateX(driveRef*driveFactor);
     rotateY(frameCount * speedGraphStep);
     cylinder(wheelRadius,wheelHeight);
     pop();

  pop();
  
}

