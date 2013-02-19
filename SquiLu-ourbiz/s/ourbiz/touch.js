var input = {
	right:false,
	up:false,
	left:false,
	down:false,
	quit:false,
	info:false,
	play:false,
	jump:false
}; //edit what ever you want with this

var triggerElementID = null;
var fingerCount = 0;
var startX = 0;
var startY = 0;
var curX = 0;
var curY = 0;
var deltaX = 0; //have not found a use for delta but I'm sure you will
var deltaY = 0;//have not found a use for delta but I'm sure you will
var horzDiff = 0;
var vertDiff = 0;
var minLength = 50;
var swipeLength = 0;
var swipeAngle = null;
var swipeDirection = null;

function touchStart(event,passedName)
{
    event.preventDefault();
    fingerCount = event.touches.length;
    if ( fingerCount == 1 ){
       startX = event.touches[0].pageX;
       startY = event.touches[0].pageY;
        triggerElementID = passedName;
    }else{
       touchCancel(event);
    }
}

function touchMove(event)
{
    event.preventDefault();
    if ( event.touches.length == 1 ){
        curX = event.touches[0].pageX;
        curY = event.touches[0].pageY;
    }else{
        touchCancel(event);
    }
}

function touchEnd(event)
{
    event.preventDefault();
    if ( fingerCount == 1 && curX != 0 ){
        swipeLength = Math.round(Math.sqrt(Math.pow(curX - startX,2) + Math.pow(curY - startY,2)));
        if ( swipeLength >= minLength ){
            caluculateAngle();
            determineSwipeDirection();
            processingRoutine();
            //touchCancel(event);
        }else{
            touchCancel(event);
        }
    }else{
        touchCancel(event);
    }
}

function getSpeed(){
    return swipeLength/10;
}

function touchCancel(event){
    fingerCount = 0;
    startX = 0;
    startY = 0;
    curX = 0;
    curY = 0;
    deltaX = 0;
    deltaY = 0;
    horzDiff = 0;
    vertDiff = 0;
    swipeLength = 0;
    swipeAngle = null;
    swipeDirection = null;
    triggerElementID = null;
    input.left = false;
    input.right = false;
    input.up = false;
    input.down = false;
}

function caluculateAngle(){
    var X = startX-curX;
    var Y = curY-startY;
    var Z = Math.round(Math.sqrt(Math.pow(X,2)+Math.pow(Y,2)));
    var r = Math.atan2(Y,X);
    swipeAngle = Math.round(r*180/Math.PI);
    if ( swipeAngle < 0 ){
        swipeAngle =  360 - Math.abs(swipeAngle);
    }
}


function determineSwipeDirection(){
    if ( (swipeAngle <= 45) && (swipeAngle >= 0) ){
        swipeDirection = 'left';
    }else if ( (swipeAngle <= 360) && (swipeAngle >= 315) ){
        swipeDirection = 'left';
    }else if ( (swipeAngle >= 135) && (swipeAngle <= 225) ){
        swipeDirection = 'right';
    }else if ( (swipeAngle > 45) && (swipeAngle < 135) ){
        swipeDirection = 'down';
    }else{
        swipeDirection = 'up';
    }
}

function processingRoutine(){
   var swipedElement = document.getElementById("canvas");

    if ( swipeDirection == 'left' ){
        //execute left arrow action
    }else if ( swipeDirection == 'right' ){
        //execute right arrow action
    }else if ( swipeDirection == 'up' ){
        //execute up arrow action
    }else if ( swipeDirection == 'down' ){
        //execute downward arrow action
    }
}
