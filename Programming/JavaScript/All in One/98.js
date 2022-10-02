// closures
// closure : 30-40%
// analyse : 70-80%
// real example : 100%

// 1.) function can return function 

// hello -----> local memory (firstName, lastName)
// myFunc ---> using hello's local memory
// jab myFunc return hoga hello function se 
// to myFunc khali hath return nahi hoga
// vo apne sath hello function ki local memory 
// mei bane hue variables ko sath leke return 
// hoga 


function hello(firstName, lastName){
    function myFunc(){
        console.log(firstName, lastName);
        console.log("hello world")
    }
    return myFunc;
}

const ans = hello("harshit", "vashisth");
ans();