#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "admin.h"
#include "user_auth.h"
#include "booking_system.h"
#include "payment_processing.h"

int main() {
    initializeSystem();
    initializeUsers();
    loadUserData();
    loadRoutesData();
    
    int choice;
    
    printf("=== Welcome to Transport Ticket Booking System ===\n");
    
    do {
        printf("\n=== MAIN MENU ===\n");
        printf("1. User Login\n");
        printf("2. User Signup\n");
        printf("3. Admin Login\n");
        printf("4. Exit\n");
        printf("=================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                userLogin();
                break;
            case 2:
                userSignup();
                break;
            case 3:
                if(authenticateAdmin()) {
                    adminPanel();
                }
                break;
            case 4:
                saveUserData();
                saveRoutesData();
                printf("Thank you for using our booking system. Goodbye!\n");
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while(choice != 4);
    
    return 0;
}
