var inputUsername = document.getElementById("inputUsername");
var inputPassword = document.getElementById("inputPassword");
var inputButtonLogIn = document.getElementById("inputButtonLogIn");
var outputInformation = document.getElementById("information");

async function checkLoginData() {
    const username = inputUsername.value.toString();
    const password = inputPassword.value.toString();
    
    const data = {
        username,
        password
    };

    const options = {
        method: 'POST',
        header: {
            'Content-Type': 'opplication/json'
        },
        body: JSON.stringify(data)
    }

    return await fetch(window.location + "api/posts/login", options)
    .then(response => response.text())
    .then(data =>{
        console.log(data);
        if(data === "true"){
            return true;
        }else{
            return false;
        }
    })
    .catch(err => console.log(err));
}

async function requestConfigPage(){
    var checkedLoginData = await checkLoginData();
    if (checkedLoginData) {
        window.location.href = "html/redirect";
    }else{
        outputInformation.innerText = "Wrong username or password!";
    }
}

function enableLoginButton() {
    const username = inputUsername.value.toString();
    const password = inputPassword.value.toString();
    if (username.length > 0 && password.length > 0) {
        inputButtonLogIn.disabled = false;
    } else {
        inputButtonLogIn.disabled = true;
    }
}