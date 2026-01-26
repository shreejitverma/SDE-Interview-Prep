/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// function returning function 

function myFunc(){
    function hello(){
        return "hello world"
    }
    return hello;
}

const ans = myFunc();
console.log(ans());