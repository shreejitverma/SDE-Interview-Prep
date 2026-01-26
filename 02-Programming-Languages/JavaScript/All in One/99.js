/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

function hello(x){
    const a  = "varA";
    const b = "varB";
    return function(){
        console.log(a,b,x);
    }
}

const ans = hello("arg");
ans();