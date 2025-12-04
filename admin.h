#ifndef ADMIN_H
#define ADMIN_H

#include "booking_system.h"
#include "payment_processing.h"

// Function declarations
int authenticateAdmin();
void adminPanel();
void adminSearchByPhone();
void adminSearchByDestination();
void showDestinationHints(char partialDest[]);
void adminViewPassengerDetails();
void adminCancelReservation();
void adminPrintTicket();
void adminSetBusDetails();
void adminLogout();

#endif
