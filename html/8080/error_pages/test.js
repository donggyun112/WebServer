// Get the element you want to make blink
const element = document.getElementById('blinkingElement');

// Set the initial visibility state
let isVisible = true;

// Function to toggle the visibility of the element
function toggleVisibility() {
  isVisible = !isVisible;
  element.style.visibility = isVisible ? 'visible' : 'hidden';
}

// Call the toggleVisibility function every 500 milliseconds (0.5 seconds)
setInterval(toggleVisibility, 500);