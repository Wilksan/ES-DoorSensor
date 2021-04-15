var options = document.getElementById("options-content-wrapper").children;

function enableSetupOption(elementId) {
    Array.from(options).map((child) => {
        child.style.display = "none";
    });
    options[elementId].style.display = "unset";
}

function getWindowPathTo(val) {
    return window.location.toString() + val;
}

/* ########################################################################### */
/*                                Device Options                               */
/* ########################################################################### */

async function sendResetRequest() {
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
    await fetch(getWindowPathTo("/api/posts/reset"), options)
        .catch(err => console.log(err));
}