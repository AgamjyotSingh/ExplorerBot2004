void movement(char c)
{
  switch(c)
  {
    case'w':
    driveWheels(200,200);
    state = c;
    BT1.println(" Moving forward ");
      
    break;

    case'd':
    driveWheels(-100,100);
    BT1.println(" Turning right ");
    
    break;

    case'a':
    driveWheels(100,-100);
    BT1.println(" Turning left ");
    
    break;

    case's':
    driveWheels(-200,-200);
    state = c;
    BT1.println(" Moving backward ");
     
    break;

    case'b':
    driveStop();
    BT1.println(" Stop ");
     
    break;

   
  }
}
