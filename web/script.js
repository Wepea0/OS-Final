eel.expose(homeselect_value); //Expose function for Python
async function homeselect_value(btn) {
    var homescreen_choice = btn.value;

    if (homescreen_choice == 2) {
        window.location.href = "./login_screen.html"
    } else {
        window.location.href = "./signup_screen.html"
    }
    // return homescreen_choice;
    let response = await send_login_signup_details(homescreen_choice);

}



async function py_homescreen_login(){
 let msg = await eel.py_Msg()(); // asynch calling python script
 document.getElementById('python').value = "Hi " + msg;
}