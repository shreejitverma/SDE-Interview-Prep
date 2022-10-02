function returnFuncToCalPowerOf(power){
    return function(number){
        return number ** power
    }
}
const square = returnFuncToCalPowerOf(2);
console.log(square(3)) //9

const cube = returnFuncToCalPowerOf(3);
console.log(cube(2)) // 8