const mainButton = document.querySelector(".container button");
const body = document.body;
const currentColor = document.querySelector(".current-color");
const colors = ["yellow", "red", "orange", "purple", "pink", "blue", "black", "green", "cyan"];

function getRandomIndex(arrayLength){
    return Math.floor(Math.random() * arrayLength)
}

mainButton.addEventListener("click", ()=>{
    const randomIndex = getRandomIndex(colors.length);
    body.style.backgroundColor = colors[randomIndex];
    currentColor.textContent = colors[randomIndex];
})

