// little practice with click event
const allButtons = document.querySelectorAll(".my-buttons button");
// console.log(allButtons);

allButtons.forEach((button)=>{
    button.addEventListener("click", (e)=>{
        const currentButton = e.currentTarget;
        currentButton.style.backgroundColor = "#222";
        currentButton.style.color = "yellow";

    })
})
