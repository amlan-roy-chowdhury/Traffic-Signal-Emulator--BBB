#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <sys/utsname.h>

// Function to set the GPIO pin as an output
void set_gpio_direction(int gpio_pin) {
    char gpio_direction_path[100];
    sprintf(gpio_direction_path, "/sys/class/gpio/gpio%d/direction", gpio_pin);
    FILE *file = fopen(gpio_direction_path, "w");
    if (file == NULL) {
        perror("Error setting GPIO direction to out");
        exit(1);
    }
    fprintf(file, "out");
    fclose(file);
}

// Function to set the GPIO pin value (on or off)
void set_gpio_value(int gpio_pin, int value) {
    char gpio_value_path[100];
    sprintf(gpio_value_path, "/sys/class/gpio/gpio%d/value", gpio_pin);
    FILE *file = fopen(gpio_value_path, "w");
    if (file == NULL) {
        perror("Error setting GPIO pin value");
        exit(1);
    }
    fprintf(file, "%d", value);
    fclose(file);
}

// Function to set traffic light state
void set_light(int red, int yellow, int green, int red_value, int yellow_value, int green_value) {
    set_gpio_value(red, red_value);
    set_gpio_value(yellow, yellow_value);
    set_gpio_value(green, green_value);
}

// Function to flash green light
void flashing(int green_pin, int duration_ms) {
    for (int i = 0; i < duration_ms / 1000; i++) {
        set_gpio_value(green_pin, 1);
        usleep(500000);  
        set_gpio_value(green_pin, 0);
        usleep(500000);
    }
}

int main() {
    struct utsname sysInfo;
    int north_red,north_yellow,north_green,south_red,south_yellow,south_green;
    float duration;
    time_t start_time;

    // Get system information
    if (uname(&sysInfo) == -1) {
        perror("uname");
        return 1;
    }

    // Print system information
    printf("System name: %s\n", sysInfo.sysname);
    printf("Node name: %s\n", sysInfo.nodename);
    printf("Machine: %s\n", sysInfo.machine);
    printf("Team Members:\n");
    printf("Nanda Kishore Nallagopu G01447294\n");
    printf("Amlan Choudhary G01465085\n");
    printf("Enter GPIO Pin for North Red: ");
    scanf("%d",&north_red);
    printf("Enter GPIO Pin for North Yellow: ");
    scanf("%d",&north_yellow);
    printf("Enter GPIO Pin for North Green: ");
    scanf("%d",&north_green);
    printf("Enter GPIO Pin for South Red: ");
    scanf("%d",&south_red);
    printf("Enter GPIO Pin for South Yellow: ");
    scanf("%d",&south_yellow);
    printf("Enter GPIO Pin for South Green: ");
    scanf("%d",&south_green);

    // Set GPIO pins as outputs
    set_gpio_direction(north_red);
    set_gpio_direction(north_yellow);
    set_gpio_direction(north_green);
    set_gpio_direction(south_red);
    set_gpio_direction(south_yellow);
    set_gpio_direction(south_green);
    // Initialize both traffic lights to red, others off
    set_light(north_red, north_yellow, north_green, 1, 0, 0);
    set_light(south_red, south_yellow, south_green, 1, 0, 0);
    // Ask your for the duratiion
    printf("Enter green light duration (in minutes, can be fractional): ");
    scanf("%f", &duration);
    printf("Initializing: Both directions Red\n");
    // Wait for 2 seconds in the initial state
    sleep(2);  
    // we will start the timer
    time(&start_time);

    // Main traffic light control loop
    while (1) {
        // North Green, South Red
        set_light(north_red, north_yellow, north_green, 0, 0, 1);
        set_light(south_red, south_yellow, south_green, 1, 0, 0);
        printf("North: Green\t-South: Red\n");
        printf("Time: %ld seconds\n", time(NULL) - start_time);
        sleep((int)(duration * 60));

        // North Flashing Green
        flashing(north_green, 5000);

        // North Yellow, South Red
        set_light(north_red, north_yellow, north_green, 0, 1, 0);
        set_light(south_red, south_yellow, south_green, 1, 0, 0);
        printf("North: Yellow\t-South: Red\n");
        printf("Time: %ld seconds\n", time(NULL) - start_time);
        sleep(5);  // 5 seconds yellow

        // Both Red (2-second transition)
        set_light(north_red, north_yellow, north_green, 1, 0, 0);
        set_light(south_red, south_yellow, south_green, 1, 1, 0);  
        printf("North: Red\t-South: Red+Yellow\n");
        printf("Time: %ld seconds\n", time(NULL) - start_time);
        sleep(2);  // 2 seconds transition

        // North Red, South Green
        set_light(north_red, north_yellow, north_green, 1, 0, 0);
        set_light(south_red, south_yellow, south_green, 0, 0, 1);
        printf("North: Red\t-South: Green\n");
        printf("Time: %ld seconds\n", time(NULL) - start_time);
        sleep((int)(duration * 60));

        // East-West Flashing Green
        flashing(south_green, 5000);  // 5 seconds flashing

        // North-South Red, East-West Yellow
        set_light(north_red, north_yellow, north_green, 1, 0, 0);
        set_light(south_red, south_yellow, south_green, 0, 1, 0);
        printf("North: Red\t-South: Yellow\n");
        printf("Time: %ld seconds\n", time(NULL) - start_time);
        sleep(5);  // 5 seconds yellow

        // Both Red (2-second transition)
        set_light(north_red, north_yellow, north_green, 1, 1, 0);  
        set_light(south_red, south_yellow, south_green, 1, 0, 0);
        printf("North: Red+Yellow\t-South: Red\n");
        printf("Time: %ld seconds\n", time(NULL) - start_time);
        sleep(2);  // 2 seconds transition

        // Reset timer
        time(&start_time);
    }

    // Turn off all lights
    set_light(north_red, north_yellow, north_green, 0, 0, 0);
    set_light(south_red, south_yellow, south_green, 0, 0, 0);
    printf("Thankyou");
    return 0;
}