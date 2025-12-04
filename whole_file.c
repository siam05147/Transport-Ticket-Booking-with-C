#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define TOTAL_SEATS 40
#define NAME_LENGTH 50
#define SOURCE_LENGTH 30
#define DESTINATION_LENGTH 30
#define TIME_LENGTH 10
#define USERNAME_LENGTH 20
#define PASSWORD_LENGTH 20
#define MAX_USERS 100
#define MAX_ROUTES 50
#define PHONE_LENGTH 15
#define TRANSACTION_ID_LENGTH 20

typedef struct {
    int paymentID;
    char method[20];
    char transactionID[TRANSACTION_ID_LENGTH];
    float amount;
    float feePercent;
    float totalPaid;
    char status[20];
} Payment;

typedef struct {
    int routeID;
    char source[SOURCE_LENGTH];
    char destination[DESTINATION_LENGTH];
    char busTime[TIME_LENGTH];
    int seats[TOTAL_SEATS];
    int bookedCount;
    int isActive;
} Route;

typedef struct {
    int seatNo;
    char name[NAME_LENGTH];
    char phone[PHONE_LENGTH];
    int routeID;
    int paymentID;
    int isBooked;
} Booking;

typedef struct {
    char username[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];
    int isActive;
} User;

Booking bookings[TOTAL_SEATS * MAX_ROUTES];
User users[MAX_USERS];
Route routes[MAX_ROUTES];
Payment payments[TOTAL_SEATS * MAX_ROUTES];

int bookedSeats = 0;
int userCount = 0;
int routeCount = 0;
int paymentCount = 0;
int currentUserIndex = -1;

float BASE_FARE = 500.0;

void initializeSystem();
void initializeUsers();
void userSignup();
void userLogin();
int authenticateAdmin();
void adminPanel();
void userMenu();
void viewAvailableSeatsForRoute(char source[], char destination[]);
void bookTicket();
int findOrCreateRoute(char source[], char destination[]);
void showRouteSeats(int routeIndex);
void editReservation();
void cancelReservation();
void viewAllBookings();
void printTicket(int seatNumber, int routeIndex);
void adminViewPassengerDetails();
void adminSearchByPhone();
void adminSearchByDestination();
void showDestinationHints(char partialDest[]);
void adminCancelReservation();
void adminPrintTicket();
void adminSetBusDetails();
void adminLogout();
void processPayment(int bookingIndex, int routeIndex);
void generateTransactionID(char transID[]);
void calculatePayment(float *amount, float *fee, float *total, char method[]);
void clearInputBuffer();
void saveUserData();
void loadUserData();
void saveRoutesData();
void loadRoutesData();

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

void initializeSystem() {
    srand(time(0));
    
    for(int i = 0; i < TOTAL_SEATS * MAX_ROUTES; i++) {
        bookings[i].seatNo = (i % TOTAL_SEATS) + 1;
        bookings[i].isBooked = 0;
        strcpy(bookings[i].name, "");
        strcpy(bookings[i].phone, "");
        bookings[i].routeID = -1;
        bookings[i].paymentID = -1;
    }
    
    for(int i = 0; i < MAX_ROUTES; i++) {
        routes[i].routeID = i;
        routes[i].bookedCount = 0;
        routes[i].isActive = 0;
        for(int j = 0; j < TOTAL_SEATS; j++) {
            routes[i].seats[j] = 0;
        }
    }
    
    routeCount = 0;
    bookedSeats = 0;
    paymentCount = 0;
}

void initializeUsers() {
    if(userCount == 0) {
        strcpy(users[0].username, "testuser");
        strcpy(users[0].password, "password");
        users[0].isActive = 1;
        userCount = 1;
    }
}

int findOrCreateRoute(char source[], char destination[]) {
    for(int i = 0; i < routeCount; i++) {
        if(strcasecmp(routes[i].source, source) == 0 && 
           strcasecmp(routes[i].destination, destination) == 0) {
            return i;
        }
    }
    
    if(routeCount >= MAX_ROUTES) {
        printf("Maximum routes reached!\n");
        return -1;
    }
    
    int routeIndex = routeCount;
    routes[routeIndex].routeID = routeCount;
    strcpy(routes[routeIndex].source, source);
    strcpy(routes[routeIndex].destination, destination);
    
    if(routeCount == 0) {
        strcpy(routes[routeIndex].busTime, "08:00");
    } else {
        int hour = rand() % 6 + 6;
        int minute = rand() % 60;
        sprintf(routes[routeIndex].busTime, "%02d:%02d", hour, minute);
    }
    
    routes[routeIndex].bookedCount = 0;
    routes[routeIndex].isActive = 1;
    
    for(int i = 0; i < TOTAL_SEATS; i++) {
        routes[routeIndex].seats[i] = 0;
    }
    
    routeCount++;
    printf("New route created: %s to %s at %s\n", source, destination, routes[routeIndex].busTime);
    return routeIndex;
}

void viewAvailableSeatsForRoute(char source[], char destination[]) {
    int routeIndex = findOrCreateRoute(source, destination);
    if(routeIndex == -1) return;
    
    printf("\n=== AVAILABLE SEATS FOR %s to %s ===\n", source, destination);
    printf("Bus Time: %s\n", routes[routeIndex].busTime);
    printf("Available Seats: %d/%d\n", TOTAL_SEATS - routes[routeIndex].bookedCount, TOTAL_SEATS);
    
    int availableCount = 0;
    for(int i = 0; i < TOTAL_SEATS; i++) {
        if(routes[routeIndex].seats[i] == 0) {
            printf("Seat %02d ", i + 1);
            availableCount++;
            
            if((availableCount) % 4 == 0) {
                printf("\n");
            }
        }
    }
    
    if(availableCount == 0) {
        printf("No available seats on this bus!\n");
        
        char nextBusChoice;
        printf("Next bus available in 1 hour. Would you like to book on next bus? (y/n): ");
        scanf("%c", &nextBusChoice);
        clearInputBuffer();
        
        if(tolower(nextBusChoice) == 'y') {
            int nextRouteIndex = routeCount;
            if(nextRouteIndex >= MAX_ROUTES) {
                printf("Cannot create more routes!\n");
                return;
            }
            
            routes[nextRouteIndex].routeID = routeCount;
            strcpy(routes[nextRouteIndex].source, source);
            strcpy(routes[nextRouteIndex].destination, destination);
            
            int hour, minute;
            sscanf(routes[routeIndex].busTime, "%d:%d", &hour, &minute);
            hour = (hour + 1) % 24;
            sprintf(routes[nextRouteIndex].busTime, "%02d:%02d", hour, minute);
            
            routes[nextRouteIndex].bookedCount = 0;
            routes[nextRouteIndex].isActive = 1;
            
            for(int i = 0; i < TOTAL_SEATS; i++) {
                routes[nextRouteIndex].seats[i] = 0;
            }
            
            routeCount++;
            printf("Next bus created at %s\n", routes[nextRouteIndex].busTime);
            viewAvailableSeatsForRoute(source, destination);
        }
    } else {
        if(availableCount % 4 != 0) {
            printf("\n");
        }
    }
}

void bookTicket() {
    char source[SOURCE_LENGTH];
    char destination[DESTINATION_LENGTH];
    
    printf("\n=== BOOK TICKET ===\n");
    printf("Enter source: ");
    fgets(source, SOURCE_LENGTH, stdin);
    source[strcspn(source, "\n")] = 0;
    
    printf("Enter destination: ");
    fgets(destination, DESTINATION_LENGTH, stdin);
    destination[strcspn(destination, "\n")] = 0;
    
    int routeIndex = findOrCreateRoute(source, destination);
    if(routeIndex == -1) return;
    
    viewAvailableSeatsForRoute(source, destination);
    
    if(routes[routeIndex].bookedCount >= TOTAL_SEATS) {
        return;
    }
    
    int seatNumber;
    printf("\nEnter seat number to book: ");
    scanf("%d", &seatNumber);
    clearInputBuffer();
    
    if(seatNumber < 1 || seatNumber > TOTAL_SEATS) {
        printf("Invalid seat number! Please enter between 1 and %d.\n", TOTAL_SEATS);
        return;
    }
    
    if(routes[routeIndex].seats[seatNumber - 1] == 1) {
        printf("Seat %d is already booked on this bus!\n", seatNumber);
        return;
    }
    
    for(int i = 0; i < TOTAL_SEATS * MAX_ROUTES; i++) {
        if(!bookings[i].isBooked) {
            bookings[i].seatNo = seatNumber;
            bookings[i].routeID = routeIndex;
            
            printf("Enter passenger name: ");
            fgets(bookings[i].name, NAME_LENGTH, stdin);
            bookings[i].name[strcspn(bookings[i].name, "\n")] = 0;
            
            printf("Enter phone number: ");
            fgets(bookings[i].phone, PHONE_LENGTH, stdin);
            bookings[i].phone[strcspn(bookings[i].phone, "\n")] = 0;
            
            routes[routeIndex].seats[seatNumber - 1] = 1;
            routes[routeIndex].bookedCount++;
            bookings[i].isBooked = 1;
            bookedSeats++;
            
            processPayment(i, routeIndex);
            
            printf("\nTicket booked successfully!\n");
            printTicket(seatNumber, routeIndex);
            return;
        }
    }
    
    printf("Booking system error!\n");
}

void processPayment(int bookingIndex, int routeIndex) {
    printf("\n=== PAYMENT ===\n");
    printf("Base Fare: %.2f\n", BASE_FARE);
    printf("\nSelect payment method:\n");
    printf("1. Bkash (2%% fee)\n");
    printf("2. Nagad (1.5%% fee)\n");
    printf("3. Rocket (1%% fee)\n");
    printf("4. Card (1.8%% fee)\n");
    printf("5. Cash (0%% fee)\n");
    printf("Enter choice (1-5): ");
    
    int choice;
    scanf("%d", &choice);
    clearInputBuffer();
    
    char method[20];
    float feePercent;
    
    switch(choice) {
        case 1:
            strcpy(method, "Bkash");
            feePercent = 2.0;
            break;
        case 2:
            strcpy(method, "Nagad");
            feePercent = 1.5;
            break;
        case 3:
            strcpy(method, "Rocket");
            feePercent = 1.0;
            break;
        case 4:
            strcpy(method, "Card");
            feePercent = 1.8;
            break;
        case 5:
            strcpy(method, "Cash");
            feePercent = 0.0;
            break;
        default:
            printf("Invalid choice! Using Cash.\n");
            strcpy(method, "Cash");
            feePercent = 0.0;
    }
    
    float amount = BASE_FARE;
    float fee = (amount * feePercent) / 100.0;
    float total = amount + fee;
    
    payments[paymentCount].paymentID = paymentCount;
    strcpy(payments[paymentCount].method, method);
    payments[paymentCount].amount = amount;
    payments[paymentCount].feePercent = feePercent;
    payments[paymentCount].totalPaid = total;
    strcpy(payments[paymentCount].status, "Completed");
    
    if(strcmp(method, "Cash") != 0) {
        generateTransactionID(payments[paymentCount].transactionID);
        printf("Transaction ID: %s\n", payments[paymentCount].transactionID);
    } else {
        strcpy(payments[paymentCount].transactionID, "CASH");
    }
    
    bookings[bookingIndex].paymentID = paymentCount;
    paymentCount++;
    
    printf("\nPayment Summary:\n");
    printf("Method: %s\n", method);
    printf("Base Fare: %.2f\n", amount);
    printf("Fee (%.1f%%): %.2f\n", feePercent, fee);
    printf("Total Paid: %.2f\n", total);
    printf("Status: %s\n", payments[paymentCount-1].status);
}

void generateTransactionID(char transID[]) {
    time_t t = time(NULL);
    sprintf(transID, "TXN%ld%03d", t, rand() % 1000);
}

void userSignup() {
    if(userCount >= MAX_USERS) {
        printf("Maximum user limit reached!\n");
        return;
    }
    
    char username[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];
    char confirmPassword[PASSWORD_LENGTH];
    
    printf("\n=== USER SIGNUP ===\n");
    printf("Enter username: ");
    fgets(username, USERNAME_LENGTH, stdin);
    username[strcspn(username, "\n")] = 0;
    
    for(int i = 0; i < userCount; i++) {
        if(strcmp(users[i].username, username) == 0) {
            printf("Username already exists! Please choose another.\n");
            return;
        }
    }
    
    printf("Enter password: ");
    fgets(password, PASSWORD_LENGTH, stdin);
    password[strcspn(password, "\n")] = 0;
    
    printf("Confirm password: ");
    fgets(confirmPassword, PASSWORD_LENGTH, stdin);
    confirmPassword[strcspn(confirmPassword, "\n")] = 0;
    
    if(strcmp(password, confirmPassword) != 0) {
        printf("Passwords do not match!\n");
        return;
    }
    
    strcpy(users[userCount].username, username);
    strcpy(users[userCount].password, password);
    users[userCount].isActive = 1;
    userCount++;
    
    printf("User registered successfully!\n");
    saveUserData();
}

void userLogin() {
    char username[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];
    
    printf("\n=== USER LOGIN ===\n");
    printf("Enter username: ");
    fgets(username, USERNAME_LENGTH, stdin);
    username[strcspn(username, "\n")] = 0;
    
    printf("Enter password: ");
    fgets(password, PASSWORD_LENGTH, stdin);
    password[strcspn(password, "\n")] = 0;
    
    for(int i = 0; i < userCount; i++) {
        if(strcmp(users[i].username, username) == 0 && 
           strcmp(users[i].password, password) == 0) {
            currentUserIndex = i;
            printf("Login successful! Welcome %s\n", username);
            userMenu();
            return;
        }
    }
    
    printf("Invalid username or password!\n");
}

void userMenu() {
    int choice;
    
    do {
        printf("\n=== USER MENU ===\n");
        printf("1. Book a Ticket\n");
        printf("2. Edit My Reservation\n");
        printf("3. Cancel My Reservation\n");
        printf("4. Print My Ticket\n");
        printf("5. View All Bookings\n");
        printf("6. Logout\n");
        printf("=================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                bookTicket();
                break;
            case 2:
                editReservation();
                break;
            case 3:
                cancelReservation();
                break;
            case 4:
                {
                    int seatNumber;
                    printf("Enter your seat number: ");
                    scanf("%d", &seatNumber);
                    clearInputBuffer();
                    
                    int routeIndex = -1;
                    for(int i = 0; i < TOTAL_SEATS * MAX_ROUTES; i++) {
                        if(bookings[i].isBooked && bookings[i].seatNo == seatNumber) {
                            routeIndex = bookings[i].routeID;
                            break;
                        }
                    }
                    
                    if(routeIndex != -1) {
                        printTicket(seatNumber, routeIndex);
                    } else {
                        printf("Ticket not found!\n");
                    }
                }
                break;
            case 5:
                viewAllBookings();
                break;
            case 6:
                printf("Logged out successfully!\n");
                currentUserIndex = -1;
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while(choice != 6 && currentUserIndex != -1);
}

int authenticateAdmin() {
    char username[USERNAME_LENGTH];
    char password[PASSWORD_LENGTH];
    
    printf("\n=== ADMIN LOGIN ===\n");
    printf("Enter admin username: ");
    fgets(username, USERNAME_LENGTH, stdin);
    username[strcspn(username, "\n")] = 0;
    
    printf("Enter admin password: ");
    fgets(password, PASSWORD_LENGTH, stdin);
    password[strcspn(password, "\n")] = 0;
    
    if(strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0) {
        printf("Admin login successful!\n");
        return 1;
    }
    
    printf("Invalid admin credentials!\n");
    return 0;
}

void adminPanel() {
    int choice;
    
    do {
        printf("\n=== ADMIN PANEL ===\n");
        printf("1. Search Passenger by Phone\n");
        printf("2. Search Passenger by Destination\n");
        printf("3. View All Passenger Details\n");
        printf("4. Cancel Passenger Reservation\n");
        printf("5. Print Passenger Ticket\n");
        printf("6. View All Routes\n");
        printf("7. Admin Logout\n");
        printf("===================\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();
        
        switch(choice) {
            case 1:
                adminSearchByPhone();
                break;
            case 2:
                adminSearchByDestination();
                break;
            case 3:
                adminViewPassengerDetails();
                break;
            case 4:
                adminCancelReservation();
                break;
            case 5:
                adminPrintTicket();
                break;
            case 6:
                {
                    printf("\n=== ALL ACTIVE ROUTES ===\n");
                    for(int i = 0; i < routeCount; i++) {
                        if(routes[i].isActive) {
                            printf("Route %d: %s to %s | Time: %s | Booked: %d/%d\n",
                                   routes[i].routeID, routes[i].source, routes[i].destination,
                                   routes[i].busTime, routes[i].bookedCount, TOTAL_SEATS);
                        }
                    }
                }
                break;
            case 7:
                adminLogout();
                break;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    } while(choice != 7);
}

void adminSearchByPhone() {
    char phone[PHONE_LENGTH];
    
    printf("\n=== SEARCH BY PHONE NUMBER ===\n");
    printf("Enter phone number: ");
    fgets(phone, PHONE_LENGTH, stdin);
    phone[strcspn(phone, "\n")] = 0;
    
    printf("\n=== SEARCH RESULTS ===\n");
    int found = 0;
    
    for(int i = 0; i < TOTAL_SEATS * MAX_ROUTES; i++) {
        if(bookings[i].isBooked && strcmp(bookings[i].phone, phone) == 0) {
            found = 1;
            int routeIndex = bookings[i].routeID;
            int paymentID = bookings[i].paymentID;
            
            printf("\nPassenger Details:\n");
            printf("Name: %s\n", bookings[i].name);
            printf("Phone: %s\n", bookings[i].phone);
            printf("Seat: %d\n", bookings[i].seatNo);
            printf("Route: %s to %s\n", routes[routeIndex].source, routes[routeIndex].destination);
            printf("Bus Time: %s\n", routes[routeIndex].busTime);
            
            if(paymentID != -1) {
                printf("\nPayment Details:\n");
                printf("Method: %s\n", payments[paymentID].method);
                printf("Transaction ID: %s\n", payments[paymentID].transactionID);
                printf("Amount: %.2f\n", payments[paymentID].amount);
                printf("Fee: %.2f (%.1f%%)\n", 
                       payments[paymentID].totalPaid - payments[paymentID].amount,
                       payments[paymentID].feePercent);
                printf("Total Paid: %.2f\n", payments[paymentID].totalPaid);
                printf("Status: %s\n", payments[paymentID].status);
            }
            printf("-----------------------------\n");
        }
    }
    
    if(!found) {
        printf("No passenger found with phone number: %s\n", phone);
    }
}

void showDestinationHints(char partialDest[]) {
    char uniqueDests[MAX_ROUTES][DESTINATION_LENGTH];
    int destCount = 0;
    
    for(int i = 0; i < routeCount; i++) {
        if(routes[i].isActive) {
            int exists = 0;
            for(int j = 0; j < destCount; j++) {
                if(strcasecmp(uniqueDests[j], routes[i].destination) == 0) {
                    exists = 1;
                    break;
                }
            }
            if(!exists) {
                strcpy(uniqueDests[destCount], routes[i].destination);
                destCount++;
            }
        }
    }
    
    printf("\nDid you mean? (Destinations starting with '%s'):\n", partialDest);
    int hints = 0;
    for(int i = 0; i < destCount; i++) {
        if(strncasecmp(uniqueDests[i], partialDest, strlen(partialDest)) == 0) {
            printf("- %s\n", uniqueDests[i]);
            hints++;
        }
    }
    
    if(hints == 0) {
        printf("No matching destinations found.\n");
        printf("Available destinations:\n");
        for(int i = 0; i < destCount; i++) {
            printf("- %s\n", uniqueDests[i]);
        }
    }
}

void adminSearchByDestination() {
    char destination[DESTINATION_LENGTH];
    
    printf("\n=== SEARCH BY DESTINATION ===\n");
    printf("Enter destination: ");
    fgets(destination, DESTINATION_LENGTH, stdin);
    destination[strcspn(destination, "\n")] = 0;
    
    int exactMatch = 0;
    for(int i = 0; i < routeCount; i++) {
        if(routes[i].isActive && strcasecmp(routes[i].destination, destination) == 0) {
            exactMatch = 1;
            break;
        }
    }
    
    if(!exactMatch) {
        printf("\nNo exact match found for destination: %s\n", destination);
        showDestinationHints(destination);
        
        printf("\nEnter correct destination from list above: ");
        fgets(destination, DESTINATION_LENGTH, stdin);
        destination[strcspn(destination, "\n")] = 0;
    }
    
    printf("\n=== PASSENGERS GOING TO %s ===\n", destination);
    int found = 0;
    
    for(int i = 0; i < TOTAL_SEATS * MAX_ROUTES; i++) {
        if(bookings[i].isBooked) {
            int routeIndex = bookings[i].routeID;
            if(routeIndex != -1 && strcasecmp(routes[routeIndex].destination, destination) == 0) {
                found = 1;
                int paymentID = bookings[i].paymentID;
                
                printf("\nPassenger %d:\n", found);
                printf("Name: %s\n", bookings[i].name);
                printf("Phone: %s\n", bookings[i].phone);
                printf("Seat: %d\n", bookings[i].seatNo);
                printf("Route: %s to %s\n", routes[routeIndex].source, routes[routeIndex].destination);
                printf("Bus Time: %s\n", routes[routeIndex].busTime);
                
                if(paymentID != -1) {
                    printf("Payment: %s (TXN: %s)\n", payments[paymentID].method, payments[paymentID].transactionID);
                    printf("Amount: %.2f (Fee: %.1f%%)\n", payments[paymentID].totalPaid, payments[paymentID].feePercent);
                }
                printf("-----------------------------\n");
            }
        }
    }
    
    if(!found) {
        printf("No passengers found going to %s\n", destination);
    }
}

void adminViewPassengerDetails() {
    printf("\n=== ALL PASSENGER DETAILS ===\n");
    
    if(bookedSeats == 0) {
        printf("No bookings found.\n");
        return;
    }
    
    int count = 0;
    for(int i = 0; i < TOTAL_SEATS * MAX_ROUTES; i++) {
        if(bookings[i].isBooked) {
            count++;
            int routeIndex = bookings[i].routeID;
            int paymentID = bookings[i].paymentID;
            
            printf("\nPassenger %d:\n", count);
            printf("Name: %s\n", bookings[i].name);
            printf("Phone: %s\n", bookings[i].phone);
            printf("Seat: %d\n", bookings[i].seatNo);
            
            if(routeIndex != -1) {
                printf("Route: %s to %s\n", routes[routeIndex].source, routes[routeIndex].destination);
                printf("Bus Time: %s\n", routes[routeIndex].busTime);
            }
            
            if(paymentID != -1) {
                printf("Payment: %s | TXN: %s\n", payments[paymentID].method, payments[paymentID].transactionID);
                printf("Paid: %.2f (Fee: %.1f%%)\n", payments[paymentID].totalPaid, payments[paymentID].feePercent);
            }
            printf("-----------------------------\n");
        }
    }
    
    printf("Total passengers: %d\n", count);
}

void adminCancelReservation() {
    int seatNumber;
    char destination[DESTINATION_LENGTH];
    
    printf("\n=== CANCEL RESERVATION ===\n");
    printf("Enter destination: ");
    fgets(destination, DESTINATION_LENGTH, stdin);
    destination[strcspn(destination, "\n")] = 0;
    
    printf("Enter seat number: ");
    scanf("%d", &seatNumber);
    clearInputBuffer();
    
    int found = 0;
    for(int i = 0; i < TOTAL_SEATS * MAX_ROUTES; i++) {
        if(bookings[i].isBooked && bookings[i].seatNo == seatNumber) {
            int routeIndex = bookings[i].routeID;
            if(routeIndex != -1 && strcasecmp(routes[routeIndex].destination, destination) == 0) {
                found = 1;
                
                printf("\nFound passenger:\n");
                printf("Name: %s\n", bookings[i].name);
                printf("Phone: %s\n", bookings[i].phone);
                printf("Seat: %d\n", bookings[i].seatNo);
                printf("Route: %s to %s\n", routes[routeIndex].source, routes[routeIndex].destination);
                
                char confirm;
                printf("Are you sure you want to cancel? (y/n): ");
                scanf("%c", &confirm);
                clearInputBuffer();
                
                if(tolower(confirm) == 'y') {
                    bookings[i].isBooked = 0;
                    routes[routeIndex].seats[seatNumber - 1] = 0;
                    routes[routeIndex].bookedCount--;
                    bookedSeats--;
                    
                    printf("Reservation canceled successfully.\n");
                } else {
                    printf("Cancellation aborted.\n");
                }
                return;
            }
        }
    }
    
    if(!found) {
        printf("No reservation found for seat %d to %s\n", seatNumber, destination);
    }
}

void adminPrintTicket() {
    int seatNumber;
    char destination[DESTINATION_LENGTH];
    
    printf("\n=== PRINT PASSENGER TICKET ===\n");
    printf("Enter destination: ");
    fgets(destination, DESTINATION_LENGTH, stdin);
    destination[strcspn(destination, "\n")] = 0;
    
    printf("Enter seat number: ");
    scanf("%d", &seatNumber);
    clearInputBuffer();
    
    int found = 0;
    for(int i = 0; i < TOTAL_SEATS * MAX_ROUTES; i++) {
        if(bookings[i].isBooked && bookings[i].seatNo == seatNumber) {
            int routeIndex = bookings[i].routeID;
            if(routeIndex != -1 && strcasecmp(routes[routeIndex].destination, destination) == 0) {
                found = 1;
                printTicket(seatNumber, routeIndex);
                return;
            }
        }
    }
    
    if(!found) {
        printf("No booking found for seat %d to %s\n", seatNumber, destination);
    }
}

void adminSetBusDetails() {
    char source[SOURCE_LENGTH];
    char destination[DESTINATION_LENGTH];
    
    printf("\n=== SET BUS DETAILS ===\n");
    printf("Enter source: ");
    fgets(source, SOURCE_LENGTH, stdin);
    source[strcspn(source, "\n")] = 0;
    
    printf("Enter destination: ");
    fgets(destination, DESTINATION_LENGTH, stdin);
    destination[strcspn(destination, "\n")] = 0;
    
    int routeIndex = -1;
    for(int i = 0; i < routeCount; i++) {
        if(strcasecmp(routes[i].source, source) == 0 && 
           strcasecmp(routes[i].destination, destination) == 0) {
            routeIndex = i;
            break;
        }
    }
    
    if(routeIndex == -1) {
        printf("Route not found!\n");
        return;
    }
    
    printf("Current bus time: %s\n", routes[routeIndex].busTime);
    printf("Enter new bus time (HH:MM): ");
    fgets(routes[routeIndex].busTime, TIME_LENGTH, stdin);
    routes[routeIndex].busTime[strcspn(routes[routeIndex].busTime, "\n")] = 0;
    
    printf("Bus time updated to %s\n", routes[routeIndex].busTime);
}

void adminLogout() {
    printf("Admin logged out successfully!\n");
}

void editReservation() {
    char phone[PHONE_LENGTH];
    
    printf("\n=== EDIT RESERVATION ===\n");
    printf("Enter your phone number: ");
    fgets(phone, PHONE_LENGTH, stdin);
    phone[strcspn(phone, "\n")] = 0;
    
    int found = 0;
    int bookingIndex = -1;
    
    for(int i = 0; i < TOTAL_SEATS * MAX_ROUTES; i++) {
        if(bookings[i].isBooked && strcmp(bookings[i].phone, phone) == 0) {
            found = 1;
            bookingIndex = i;
            break;
        }
    }
    
    if(!found) {
        printf("No reservation found with phone number: %s\n", phone);
        return;
    }
    
    printf("\nCurrent Details:\n");
    printf("Name: %s\n", bookings[bookingIndex].name);
    printf("Phone: %s\n", bookings[bookingIndex].phone);
    
    int routeIndex = bookings[bookingIndex].routeID;
    if(routeIndex != -1) {
        printf("Route: %s to %s\n", routes[routeIndex].source, routes[routeIndex].destination);
    }
    
    printf("\nEnter new details:\n");
    printf("Enter new Name: ");
    fgets(bookings[bookingIndex].name, NAME_LENGTH, stdin);
    bookings[bookingIndex].name[strcspn(bookings[bookingIndex].name, "\n")] = 0;
    
    printf("Enter new Phone: ");
    fgets(bookings[bookingIndex].phone, PHONE_LENGTH, stdin);
    bookings[bookingIndex].phone[strcspn(bookings[bookingIndex].phone, "\n")] = 0;
    
    printf("Reservation edited successfully.\n");
}

void cancelReservation() {
    char phone[PHONE_LENGTH];
    
    printf("\n=== CANCEL RESERVATION ===\n");
    printf("Enter your phone number: ");
    fgets(phone, PHONE_LENGTH, stdin);
    phone[strcspn(phone, "\n")] = 0;
    
    int found = 0;
    int bookingIndex = -1;
    
    for(int i = 0; i < TOTAL_SEATS * MAX_ROUTES; i++) {
        if(bookings[i].isBooked && strcmp(bookings[i].phone, phone) == 0) {
            found = 1;
            bookingIndex = i;
            break;
        }
    }
    
    if(!found) {
        printf("No reservation found with phone number: %s\n", phone);
        return;
    }
    
    printf("\nYour Booking Details:\n");
    printf("Name: %s\n", bookings[bookingIndex].name);
    printf("Phone: %s\n", bookings[bookingIndex].phone);
    printf("Seat: %d\n", bookings[bookingIndex].seatNo);
    
    int routeIndex = bookings[bookingIndex].routeID;
    if(routeIndex != -1) {
        printf("Route: %s to %s\n", routes[routeIndex].source, routes[routeIndex].destination);
        printf("Bus Time: %s\n", routes[routeIndex].busTime);
    }
    
    char confirm;
    printf("Are you sure you want to cancel? (y/n): ");
    scanf("%c", &confirm);
    clearInputBuffer();
    
    if(tolower(confirm) == 'y') {
        bookings[bookingIndex].isBooked = 0;
        
        if(routeIndex != -1) {
            routes[routeIndex].seats[bookings[bookingIndex].seatNo - 1] = 0;
            routes[routeIndex].bookedCount--;
        }
        
        bookedSeats--;
        printf("Your reservation canceled successfully.\n");
    } else {
        printf("Cancellation aborted.\n");
    }
}

void viewAllBookings() {
    printf("\n=== ALL BOOKINGS ===\n");
    
    if(bookedSeats == 0) {
        printf("No bookings found.\n");
        return;
    }
    
    int count = 0;
    for(int i = 0; i < TOTAL_SEATS * MAX_ROUTES; i++) {
        if(bookings[i].isBooked) {
            count++;
            int routeIndex = bookings[i].routeID;
            
            printf("%d. Seat %02d | %s | ", count, bookings[i].seatNo, bookings[i].name);
            if(routeIndex != -1) {
                printf("%s to %s | %s", routes[routeIndex].source, routes[routeIndex].destination, routes[routeIndex].busTime);
            }
            printf("\n");
        }
    }
    
    printf("Total bookings: %d\n", count);
}

void printTicket(int seatNumber, int routeIndex) {
    printf("\n");
    printf("=========================================\n");
    printf("           TRANSPORT TICKET\n");
    printf("=========================================\n");
    
    for(int i = 0; i < TOTAL_SEATS * MAX_ROUTES; i++) {
        if(bookings[i].isBooked && bookings[i].seatNo == seatNumber && bookings[i].routeID == routeIndex) {
            printf(" Passenger:   %s\n", bookings[i].name);
            printf(" Phone:       %s\n", bookings[i].phone);
            printf(" Seat:        %d\n", seatNumber);
            
            if(routeIndex != -1) {
                printf(" From:        %s\n", routes[routeIndex].source);
                printf(" To:          %s\n", routes[routeIndex].destination);
                printf(" Bus Time:    %s\n", routes[routeIndex].busTime);
            }
            
            int paymentID = bookings[i].paymentID;
            if(paymentID != -1) {
                printf(" Payment:     %s\n", payments[paymentID].method);
                printf(" TXN ID:      %s\n", payments[paymentID].transactionID);
                printf(" Amount:      %.2f\n", payments[paymentID].totalPaid);
                printf(" Status:      CONFIRMED\n");
            }
            break;
        }
    }
    
    printf("=========================================\n");
    printf("    Thank you for choosing our service!\n");
    printf("=========================================\n\n");
}

void saveUserData() {
    FILE *file = fopen("users.dat", "wb");
    if(file != NULL) {
        fwrite(&userCount, sizeof(int), 1, file);
        fwrite(users, sizeof(User), userCount, file);
        fclose(file);
    }
}

void loadUserData() {
    FILE *file = fopen("users.dat", "rb");
    if(file != NULL) {
        fread(&userCount, sizeof(int), 1, file);
        fread(users, sizeof(User), userCount, file);
        fclose(file);
    }
}

void saveRoutesData() {
    FILE *file = fopen("routes.dat", "wb");
    if(file != NULL) {
        fwrite(&routeCount, sizeof(int), 1, file);
        fwrite(routes, sizeof(Route), routeCount, file);
        
        fwrite(&bookedSeats, sizeof(int), 1, file);
        fwrite(&paymentCount, sizeof(int), 1, file);
        
        fwrite(bookings, sizeof(Booking), TOTAL_SEATS * MAX_ROUTES, file);
        fwrite(payments, sizeof(Payment), paymentCount, file);
        fclose(file);
    }
}

void loadRoutesData() {
    FILE *file = fopen("routes.dat", "rb");
    if(file != NULL) {
        fread(&routeCount, sizeof(int), 1, file);
        fread(routes, sizeof(Route), routeCount, file);
        
        fread(&bookedSeats, sizeof(int), 1, file);
        fread(&paymentCount, sizeof(int), 1, file);
        
        fread(bookings, sizeof(Booking), TOTAL_SEATS * MAX_ROUTES, file);
        fread(payments, sizeof(Payment), paymentCount, file);
        fclose(file);
    }
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}