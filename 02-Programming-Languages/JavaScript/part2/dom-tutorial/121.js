/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// this keyword
const btn = document.querySelector(".btn-headline");

btn.addEventListener("click",function(){
    console.log("you clicked me !!!!");
    console.log("value of this")
    console.log(this);
});