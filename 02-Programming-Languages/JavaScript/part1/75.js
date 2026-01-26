/*
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 */

// arrow functions 

const user1 = {
    firstName : "harshit",
    age: 8,
    about: () => {
        console.log(this.firstName, this.age);
    }   
}

user1.about(user1);