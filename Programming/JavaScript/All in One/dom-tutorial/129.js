const todoList = document.querySelector(".todo-list");
// console.log(todoList);

todoList.addEventListener("click", (e) => {
  if (e.target.classList.contains("remove")) {
    console.log("so you want to remove something");
    const targetedLi = e.target.parentNode.parentNode;
    targetedLi.remove();
  }
  if (e.target.classList.contains("done")) {
    console.log("great ");
    const targetedTodoText = e.target.parentNode.previousElementSibling;
    targetedTodoText.style.textDecoration = "line-through";
    targetedTodoText.style.color = "#ddd";
  }
});

const todoForm = document.querySelector(".form-todo");
const todoInput = document.querySelector(".form-todo input[type = 'text']");
todoForm.addEventListener("submit", (e) => {
  e.preventDefault();
  const newTodoInput = todoInput.value;
  const newLiElement = document.createElement("li");
  newLiElement.innerHTML = `<span class="text">${newTodoInput}</span>
          <div class="todo-buttons">
          <button class="todo-btn done">Done</button>
            <button class="todo-btn remove">Remove</button>    
          </div>`;
  todoList.append(newLiElement);
  todoInput.value = "";
});
