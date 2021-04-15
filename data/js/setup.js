var options = document.getElementById("options-content-wrapper").children;

function enableSetupOption(elementId){
    Array.from(options).map((child) => {
        child.style.display = "none";
    });
    options[elementId].style.display = "unset";
}

function getWindowPathTo(val){
    return window.location.toString() + val;
}


/* ########################################################################### */
/*                          Login Options                                      */
/* ########################################################################### */

// Functions that test if the username is correct
async function requestUserData() {
    const data = {
        username:"username",
        password:"password"
    };
    const options = {
        method: 'POST',
        header: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    }
    var userData = await fetch(getWindowPathTo("/api/posts/userdata"), options)
        .then(response => response.json())
        .then(data => {
            return data;
        })
        .catch(err => console.log(err));
    document.getElementById("inputUsername").value = userData.username;
    document.getElementById("inputOldPassword").value = userData.password;
}

requestUserData();

// Commit username
async function commitNewUserData() {    
    var newUsername = document.getElementById("inputUsername");
    var newPassword = document.getElementById("inputNewPassword");

    var data = {
        username: newUsername.value.toString(),
        password: newPassword.value.toString()
    }

    var options = {
        method: "POST", 
        header: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    }

    await fetch(getWindowPathTo("/api/posts/new_userdata"), options)
    .catch(err => console.error(err));

    requestUserData();
}

// Check if the username does not exceed the max. string length
function checkIfUsernameToLong(container){
    return (container.value.length > 25);
}

function usernameCorrect(){
    var inputUsername = document.getElementById("inputUsername");
    var usernameMessage = document.getElementById("spanUsername");

    if (checkIfUsernameToLong(inputUsername)) {
        inputUsername.style.border = "1px solid red";
        usernameMessage.innerText = "Der Nutzername darf nicht länger als 25 Zeichen sein!";
    }else{
        inputUsername.style.border = "";
        usernameMessage.innerText = "";
    }
}

// Check if new password is correct
function checkIfPasswordLengthCorrect(minVal, maxVal, container){
    return !( (container.value.length >= minVal) && (container.value.length <= maxVal) );
}

function checkIfPasswordStartsWithALetter(container){
    var firstLetter = container.value.toString().charCodeAt(0);
    return ((firstLetter > 64 && firstLetter < 91) ||
    (firstLetter > 96 && firstLetter < 123) );
}

function checkForOneUpperLetter(container){
    for(var i = 0; i < container.value.toString().length; i++){
        var charecter = container.value.toString().charCodeAt(i);
        if(charecter > 64 && charecter < 91){
            return true;
        }
    }
}

function checkForNewPasswordCorrect(){
    var oldPassword = document.getElementById("inputOldPassword");
    var newPassword = document.getElementById("inputNewPassword");
    var passwordMessage = document.getElementById("spanNewPasswordMessage");

    if(checkIfPasswordLengthCorrect(8, 15, newPassword)){
        newPassword.style.border = "1px solid red";
        if(newPassword.value.length < 8){
            passwordMessage.innerText = "Das neue Password ist zu kurz!";
        }else{
            passwordMessage.innerText = "Das neue Password ist zu lang!";
        }
    } else if (newPassword.value === oldPassword.value) {
        newPassword.style.border = "1px solid red";
        passwordMessage.innerText = "Das neue Password darf nicht den alten Password entsprechen!";
    } else if (!checkIfPasswordStartsWithALetter(newPassword)){
        passwordMessage.innerText = "Das neue Password darf nicht mit einer Zahl oder einen Sonderzeichen beginnen!";
    } else if(!checkForOneUpperLetter(newPassword)){
        passwordMessage.innerText = "Das neue Password muss mindestens einen Großbuchstaben enthalten!";
    } else {
        newPassword.style.border = "";
        passwordMessage.innerText = "";
    }
}

// Check if repeat password is correct
function checkForRepeatPasswordCorrect() {
    var newPassword = document.getElementById("inputNewPassword");
    var repeatPassword = document.getElementById("inputRepeatPassword");
    var passwordMessage = document.getElementById("spanRepeatPasswordMessage");

    if(!(repeatPassword.value === newPassword.value)){
        repeatPassword.style.border = "1px solid red";
        passwordMessage.innerText = "Das wiederholte Password entspricht nicht den neuen Password!";
    }else{
        passwordMessage.innerText = "";
        repeatPassword.style.border = "";
    }
}

function loginFilledOut() {
    var username = document.getElementById("inputUsername");
    var oldPassword = document.getElementById("inputOldPassword");
    var newPassword = document.getElementById("inputNewPassword");
    var repeatPassword = document.getElementById("inputRepeatPassword");

    return  username.value.length > 0 &&
            oldPassword.value.length > 0 &&
            newPassword.value.length > 0 &&
            repeatPassword.value.length > 0;
}

function loginCorrect() {
    var username = document.getElementById("inputUsername");
    var oldPassword = document.getElementById("inputOldPassword");
    var newPassword = document.getElementById("inputNewPassword");
    var repeatPassword = document.getElementById("inputRepeatPassword");
    return  (checkIfPasswordStartsWithALetter(newPassword)) &&
            (checkForOneUpperLetter(newPassword)) &&
            (!checkIfPasswordLengthCorrect(8, 15, newPassword)) &&
            !(newPassword.value === oldPassword.value) &&
            (repeatPassword.value == newPassword.value) &&
            (!checkIfUsernameToLong(username));
}

var intervalTimerLogin = setInterval(() => {
    if (loginFilledOut() && loginCorrect()) {
        document.getElementById("buttonCommitLogin").disabled = false;  
    }
}, 200);

// Check if all inputs are filled
function formFilledOut(params) {
    for (var i = 0; i < params.length; i++) {
        if (params[i].value.length == 0) {
            return false;
        }
    }
    return true;
}

// Check if the wifi password and repear wifi password are equal
function checkForEquality(input1, input2) {
    return input1.value === input2.value;
}

/* ########################################################################### */
/*                          Wifi Options                                       */
/* ########################################################################### */

var wifiInterval = setInterval(() => {
    var wifiName = document.getElementById("inputWifiName");
    var wifiPassword = document.getElementById("inputWifiPassword");
    var repeatWifiPassword = document.getElementById("inputWifiRepeatPassword");
    var commitButton = document.getElementById("buttonCommitWifi");

    if (formFilledOut([wifiName, wifiPassword, repeatWifiPassword]) && checkForEquality(wifiPassword, repeatWifiPassword)){
        commitButton.disabled = false;
    }else{
        commitButton.disabled = true;
    }
}, 200);

// Send network information to the esp32
async function commitWifiData(){
    var networkName = document.getElementById("inputWifiName");
    var wifiPassword = document.getElementById("inputWifiPassword");
    
    const data = {
        wifi_name: networkName.value,
        wifi_password: wifiPassword.value
    };

    const options = {
        method: "POST",
        header: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    };

    await fetch(getWindowPathTo("/api/posts/new_wifi_data"), options)
    .catch(err => console.error(err));
}

/* ########################################################################### */
/*                          Room Options                                       */
/* ########################################################################### */

var roomInterval = setInterval(() => {
    var companyName = document.getElementById("inputRoomCompanyName");
    var roomName = document.getElementById("inputRoomName");
    var roomSize = document.getElementById("inputRoomSize");
    var maxRoomSersonCount= document.getElementById("inputRoomMaxPersonCount");

    var commitButton = document.getElementById("buttonCommitRoom");

    if (formFilledOut([companyName, roomName, roomSize, maxRoomSersonCount])){
        commitButton.disabled = false;
    }else{
        commitButton.disabled = true;
    }

}, 200);

async function commitRoomData(){
    var companyName = document.getElementById("inputRoomCompanyName");
    var roomName = document.getElementById("inputRoomName");
    var roomSize = document.getElementById("inputRoomSize");
    var maxRoomSersonCount = document.getElementById("inputRoomMaxPersonCount");

    const data = {
        companyName: companyName.value,
        roomName: roomName.value,
        roomSize: roomSize.value,
        maxRoomSersonCount: maxRoomSersonCount.value
    };

    const options = {
        method: "POST",
        header: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    }

    await fetch(getWindowPathTo("/api/posts/new_room_data"), options)
    .catch(err => console.error(err));
}

/* ########################################################################### */
/*                         Database Options                                    */
/* ########################################################################### */

var databaseInterval = setInterval(() => {
    var databaseIp = document.getElementById("ip-wrapper");
    var databaseUsername = document.getElementById("inputDatabaseUsername");
    var databasePassword = document.getElementById("inputDatabasePassword");
    var databaseRepeatPassword = document.getElementById("inputDatabaseRenewPassword");

    var commitButton = document.getElementById("buttonCommitDatabase");

    var elements = [
        databaseIp.children[0],
        databaseIp.children[1],
        databaseIp.children[2],
        databaseIp.children[3],
        databaseUsername,
        databasePassword,
        databaseRepeatPassword
    ];

    if (checkForEquality(databasePassword, databaseRepeatPassword) &&
        formFilledOut(elements) ){
        commitButton.disabled = false;
    }else{
        commitButton.disabled = true;
    }

}, 200);

async function commitDatabaseData(){
    var databaseIp = document.getElementById("ip-wrapper");
    var username = document.getElementById("inputDatabaseUsername");
    var password = document.getElementById("inputDatabasePassword");
    
    const data = {
        ip:{
            byte_1: databaseIp.children[0].value,
            byte_2: databaseIp.children[1].value,
            byte_3: databaseIp.children[2].value,
            byte_4: databaseIp.children[3].value
        },
        username: username.value,
        password: password.value
    }

    const options = {
        method: "POST",
        header: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
    }

    await fetch(getWindowPathTo("/api/posts/new_database_data"), options)
    .catch(err => console.error(err));
}

/* ########################################################################### */
/*                                Device Options                               */
/* ########################################################################### */

async function sendCommitRequest(){
    const data = {
        reset: true
    };

    const options = {
        method: "POST",
        header: {
            'Content-Type': 'application/json'
        },
        data: JSON.stringify(data)
    }
    await fetch(getWindowPathTo("/api/posts/commit"), options)
    .catch(err => console.log(err));
}