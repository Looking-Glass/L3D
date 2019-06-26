// Example usage for L3D library by Alex Hornstein <alex@lookingglassfactory.com>.

#include "L3D.h"

// Initialize objects from the lib
L3D l3D;

void setup() {
    // Call functions on initialized library objects that require hardware
    l3D.begin();
}

void loop() {
    // Use the library's initialized objects and functions
    l3D.process();
}
