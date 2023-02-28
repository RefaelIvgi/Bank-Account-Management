//Created by:
// Damian Fain - 320518996 
// Refael Ivgi - 316363449
//28/12/2022


#define _CRT_SECURE_NO_WARNINGS


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define ITEM_FILE "items.dat"
#define EMPLOYEE_FILE "employees.dat"
#define CUSTOMER_FILE "customers.dat"
#define LOG_FILE "log.txt"

// Employee struct
typedef struct employee
{
    char username[15];
    char password[15];
    int level;
    int employee_number;
    char fullname[20];
} emp;

// Customer struct
typedef struct customer
{
    char name[20];
    int account_number;
    char owner_type;//חשבון משפחתי, סטודנט, יחיד
    int branch_number;// סניף
    double management_fees;
    int balance;
} cust;

// Item struct
typedef struct items
{
    char name[15];
    char type;
    double quantity; //dollars , euro 
    int serial_number;

}item;

typedef struct bin_tree {
    cust data;
    struct bin_tree* right, * left, * parent;
}node;

typedef struct bin_tree2 {
    emp data;
    struct bin_tree2* right, * left, * parent;
}nodeEmployee;

typedef struct bin_tree3 {
    item data;
    struct bin_tree3* right, * left, * parent;
}nodeItem;

int num_employees = 0;
int num_customers = 0;
int num_logs = 0;
item itemtype;


void logEvent(char* event, int accountNumber, char* name) {
    // Open the log file in append mode
    FILE* log = fopen(LOG_FILE, "a");

    // Get the current timestamp
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    char timestamp[20];
    strftime(timestamp, 20, "%d-%m-%Y %H:%M:%S", timeinfo);

    // Write the event to the log file
    fprintf(log, "[%s] %s - Account Number: %d, Name: %s\n", timestamp, event, accountNumber, name);

    fclose(log);
}


node* find_customer(node* root, int account_number) {

    node* current = root;

    cust temp_customer;
    int found = 0;
    // Search for the customer in the binary search tree
    while (current != NULL) {

        if (current->data.account_number == account_number) {
            return current;
        }

        if (account_number < current->data.account_number) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }
    return NULL;
}
node* addCustomer(node** root, cust newCustomer) {
    // Create a new node for the customer
    node* newNode = (node*)malloc(sizeof(node));
    newNode->data = newCustomer;//to do
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->parent = NULL;

    // Insert the new node into the binary search tree
    if (*root == NULL) {
        *root = newNode;
    }
    else {
        node* current = *root;
        node* parent = NULL;

        while (current != NULL) {
            parent = current;
            if (newCustomer.account_number < current->data.account_number) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        if (newCustomer.account_number < parent->data.account_number) {
            parent->left = newNode;
            newNode->parent = parent;
        }
        else {
            parent->right = newNode;
            newNode->parent = parent;
        }
    }

    // Write the new customer data to the CUSTOMER_FILE
    FILE* fc = fopen(CUSTOMER_FILE, "ab");
    fwrite(&newCustomer, sizeof(cust), 1, fc);
    fclose(fc);
    printf("Customer added successfully.\n");
    logEvent("Customer added", newCustomer.account_number, newCustomer.name);
    num_customers++;

}
int deleteCustomer(node** root, int customer_number) {
    node* current = *root;
    node* parent = NULL;
    cust temp_customer;
    int found = 0;
    // Search for the customer in the binary search tree
    while (current != NULL && current->data.account_number != customer_number) {
        parent = current;
        if (customer_number < current->data.account_number) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }

    // //If the customer is not found, return a message indicating that the customer does not exist
    //if (current == NULL) {
    //    printf("Customer with account number %d does not exist.\n", customer_number);
    //    return 0;
    //} // TODO - uncomment..

    // Remove the customer's data from the CUSTOMER_FILE
    FILE* source = fopen(CUSTOMER_FILE, "rb");
    FILE* temp = fopen("temp.dat", "wb");
    while (fread(&temp_customer, sizeof(cust), 1, source)) {
        if (temp_customer.account_number != customer_number) {
            fwrite(&temp_customer, sizeof(cust), 1, temp);
        }
        else {
            found = 1;
            printf("Customer with account number %d was successfully deleted.\n", customer_number);
        }
    }
    fclose(source);
    fclose(temp);
    remove(CUSTOMER_FILE);
    rename("temp.dat", CUSTOMER_FILE);
    if (!found) {
        printf("Customer with account number %d does not exist.\n", customer_number);
        return 0;
    }

    // remove from binary tree and free memory
    // (Cases 1-3, as in your original code)
    if (current->left == NULL && current->right == NULL) {
        if (parent == NULL) {
            // The current node is the root
            *root = NULL;
        }
        else if (parent->left == current) {
            parent->left = NULL;
        }
        else {
            parent->right = NULL;
        }
    }
    else if (current->left == NULL) {
        if (parent == NULL) {
            *root = current->right;
        }
        else if (parent->left == current) {
            parent->left = current->right;
        }
        else {
            parent->right = current->right;
        }
    }
    else if (current->right == NULL) {
        if (parent == NULL) {
            *root = current->left;
        }
        else if (parent->left == current) {
            parent->left = current->left;
        }
        else {
            parent->right = current->left;
        }
    }


    time_t now = time(0);
    char* time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = 0;
    char log_str[100];
    sprintf(log_str, "Customer %d deleted at %s", customer_number, time_str);
    FILE* fl = fopen(LOG_FILE, "a");
    fprintf(fl, "%s\n", log_str);
    fclose(fl);
    printf("Customer deleted successfully.\n");
    free(current);
    return 1;
}
void update_customer_balance(node** root, int account_number, double balance) {
    node* current = find_customer(root, account_number);
    if (current == NULL) {
        printf("Customer with account number %d does not exist.\n", account_number);
        return;
    }
    current->data.balance = balance;
    FILE* customerFile = fopen(CUSTOMER_FILE, "wb+");
    fseek(customerFile, sizeof(cust), SEEK_SET);
    fwrite(&(current->data), sizeof(cust), 1, customerFile);
    fclose(customerFile);
}
void printCustomers()
{
    cust c;
    FILE* fc = fopen(CUSTOMER_FILE, "rb");
    if (fc == NULL)
    {
        printf("Error opening customer file\n");
        return;
    }
    while (fread(&c, sizeof(cust), 1, fc) == 1)
        printf("name: %s  account number: %d  balance: %d\n\n", c.name, c.account_number, c.balance);
    fclose(fc);
}




nodeEmployee* addEmployee(nodeEmployee** root, emp newEmployee)
{
    // Create a new node for the employee
    nodeEmployee* newNode = (nodeEmployee*)malloc(sizeof(nodeEmployee));
    newNode->data = newEmployee;//to do... vladi told me
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->parent = NULL;

    // Insert the new node into the binary search tree
    if (*root == NULL) {
        *root = newNode;
    }
    else {
        nodeEmployee* current = *root;
        nodeEmployee* parent = NULL;

        while (current != NULL) {
            parent = current;
            if (newEmployee.employee_number < current->data.employee_number) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        if (newEmployee.employee_number < parent->data.employee_number) {
            parent->left = newNode;
            newNode->parent = parent;
        }
        else {
            parent->right = newNode;
            newNode->parent = parent;
        }
    }

    // Write the new customer data to the employee file
    FILE* fe = fopen("employees.dat", "ab");
    if (fe == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    fwrite(&newEmployee, sizeof(emp), 1, fe);
    fclose(fe);
    printf("%d id is:", newEmployee.employee_number);
    num_employees++;

    time_t now = time(0);
    char* time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = 0;
    char log_str[100];
    sprintf(log_str, "Employee %d Added at %s", newEmployee.employee_number, time_str);
    FILE* fl = fopen(LOG_FILE, "a");
    fprintf(fl, "%s\n", log_str);
    fclose(fl);
    printf("Employee Added successfully.\n");

}
int deleteEmployee(nodeEmployee** root, int emp_number) {

    nodeEmployee* current = *root;
    nodeEmployee* parent = NULL;
    emp temp_emp;
    int found = 0;
    // Search for the customer in the binary search tree
    while (current != NULL && current->data.employee_number != emp_number) {
        parent = current;
        if (emp_number < current->data.employee_number) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }


    FILE* source = fopen(EMPLOYEE_FILE, "rb");
    FILE* temp = fopen("temp.dat", "wb");
    while (fread(&temp_emp, sizeof(emp), 1, source)) {
        if (temp_emp.employee_number != emp_number) {
            fwrite(&temp_emp, sizeof(emp), 1, temp);
        }
        else {
            found = 1;
            printf("employee with employee number %d was successfully deleted.\n", temp_emp.employee_number);
        }
    }
    fclose(source);
    fclose(temp);
    remove(EMPLOYEE_FILE);
    rename("temp.dat", EMPLOYEE_FILE);
    if (!found) {
        printf("employee with employee number %d does not exist.\n", temp_emp.employee_number);
        return 0;
    }


    // Case 1: The current node has no children
    if (current->left == NULL && current->right == NULL) {
        if (parent == NULL) {
            // The current node is the root
            *root = NULL;
        }
        else if (parent->left == current) {
            parent->left = NULL;
        }
        else {
            parent->right = NULL;
        }
    }
    // Case 2: The current node has one child
    else if (current->left == NULL) {
        if (parent == NULL) {
            *root = current->right;
        }
        else if (parent->left == current) {
            parent->left = current->right;
        }
        else {
            parent->right = current->right;
        }
    }
    else if (current->right == NULL) {
        if (parent == NULL) {
            *root = current->left;
        }
        else if (parent->left == current) {
            parent->left = current->left;
        }
        else {
            parent->right = current->left;
        }
    }
    //write the log event 
    time_t now = time(0);
    char* time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = 0;
    char log_str[100];
    sprintf(log_str, "Employee %d deleted at %s", emp_number, time_str);
    FILE* fl = fopen(LOG_FILE, "a");
    fprintf(fl, "%s\n", log_str);
    fclose(fl);
    printf("Employee deleted successfully.\n");
}
emp* searchEmployee(char* username, char* password)
{
    // Open the employee file for reading
    FILE* fp = fopen(EMPLOYEE_FILE, "rb");
    if (fp == NULL) {
        printf("Error opening employee file.\n");
        return NULL;
    }
    // Read each employee from the file until we find a match or reach the end of the file
    emp* e = (emp*)malloc(sizeof(emp));
    while (fread(e, sizeof(emp), 1, fp) == 1)
    {
        if (strcmp(e->username, username) == 0 && strcmp(e->password, password) == 0) {
            // We found a match, so close the file and return the employee
            fclose(fp);
            return e;
        }
    }

    // If we reach this point, the employee was not found
    fclose(fp);
    return NULL;
}
void printEmp()
{
    emp e;
    FILE* fe = fopen(EMPLOYEE_FILE, "rb");
    if (fe == NULL)
    {
        printf("Error opening employee file\n");
        return;
    }
    while (fread(&e, sizeof(emp), 1, fe) == 1)
        printf("full name: %s  level: %d employee number: %d username: %s password: %s \n\n", e.fullname, e.level,
            e.employee_number, e.username, e.password);


    fclose(fe);
}


nodeItem* addItem(nodeItem** root, item newItems)
{
    // Create a new node for the employee
    nodeItem* newNode = (nodeItem*)malloc(sizeof(nodeItem));
    newNode->data = newItems;//to do... vladi told me
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->parent = NULL;

    // Insert the new node into the binary search tree
    if (*root == NULL) {
        *root = newNode;
    }
    else {
        nodeItem* current = *root;
        nodeItem* parent = NULL;

        while (current != NULL) {
            parent = current;
            if (newItems.serial_number < current->data.serial_number) {
                current = current->left;
            }
            else {
                current = current->right;
            }
        }

        if (newItems.serial_number < parent->data.serial_number) {
            parent->left = newNode;
            newNode->parent = parent;
        }
        else {
            parent->right = newNode;
            newNode->parent = parent;
        }
    }

    // Write the new item
    FILE* fe = fopen("items.dat", "ab");
    if (fe == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    fwrite(&newItems, sizeof(item), 1, fe);
    fclose(fe);

    time_t now = time(0);
    char* time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = 0;
    char log_str[100];
    sprintf(log_str, "Item %s Added at %s", newItems.name, time_str);
    FILE* fl = fopen(LOG_FILE, "a");
    fprintf(fl, "%s\n", log_str);
    fclose(fl);
    printf("Item Added successfully.\n");



}
int deleteItem(nodeItem** root, int item_number) {

    nodeItem* current = *root;
    nodeItem* parent = NULL;
    item temp_item;
    int found = 0;
    // Search for the item in the binary search tree
    while (current != NULL && current->data.serial_number != item_number) {
        parent = current;
        if (item_number < current->data.serial_number) {
            current = current->left;
        }
        else {
            current = current->right;
        }
    }


    FILE* source = fopen(ITEM_FILE, "rb");
    FILE* temp = fopen("temp.dat", "wb");
    while (fread(&temp_item, sizeof(item), 1, source)) {
        if (temp_item.serial_number != item_number) {
            fwrite(&temp_item, sizeof(item), 1, temp);
        }
        else {
            found = 1;
            printf("Item with Item number %d was successfully deleted.\n", temp_item.serial_number);
        }
    }
    fclose(source);
    fclose(temp);
    remove(ITEM_FILE);
    rename("temp.dat", ITEM_FILE);
    if (!found) {
        printf("Item with Item number %d does not exist.\n", temp_item.serial_number);
        return 0;
    }


    // Case 1: The current node has no children
    if (current->left == NULL && current->right == NULL) {
        if (parent == NULL) {
            // The current node is the root
            *root = NULL;
        }
        else if (parent->left == current) {
            parent->left = NULL;
        }
        else {
            parent->right = NULL;
        }
    }
    // Case 2: The current node has one child
    else if (current->left == NULL) {
        if (parent == NULL) {
            *root = current->right;
        }
        else if (parent->left == current) {
            parent->left = current->right;
        }
        else {
            parent->right = current->right;
        }
    }
    else if (current->right == NULL) {
        if (parent == NULL) {
            *root = current->left;
        }
        else if (parent->left == current) {
            parent->left = current->left;
        }
        else {
            parent->right = current->left;
        }
    }
    //write the log event 
    time_t now = time(0);
    char* time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = 0;
    char log_str[100];
    sprintf(log_str, "Item %s deleted at %s", temp_item.name, time_str);
    FILE* fl = fopen(LOG_FILE, "a");
    fprintf(fl, "%s\n", log_str);
    fclose(fl);
    printf("Item deleted successfully.\n");
}
void sellItem(node** root, int serialnum, cust custnumber) {
    int amount;
    printf("enter amount:");
    scanf("%d", &amount);
    switch (serialnum) {
    case 1:
    {
        printf("Attention: Dollar Gate Today = 3.5\n\n");
        node* current = find_customer(*root, custnumber.account_number);
        if (current == NULL) {
            printf("Customer with account number %d does not exist.\n", custnumber.account_number);
            break;
        }
        current->data.balance -= (amount * 3.5);
        update_customer_balance(*root, custnumber.account_number, current->data.balance);

        time_t now = time(0);
        char* time_str = ctime(&now);
        time_str[strlen(time_str) - 1] = 0;
        char log_str[100];
        sprintf(log_str, "Customer %d Made a Purchase at %s", custnumber.account_number, time_str);
        FILE* fl = fopen(LOG_FILE, "a");
        fprintf(fl, "%s\n", log_str);
        fclose(fl);
        printf("Item Purchase And Money Transfer Made successfully.\n");
        break;
    }

    case 2:
    {
        printf("Attention: Euro Gate Today = 3.8\n\n");
        node* current = find_customer(*root, custnumber.account_number);
        if (current == NULL) {
            printf("Customer with account number %d does not exist.\n", custnumber.account_number);
            break;
        }
        current->data.balance -= (amount * 3.8);
        update_customer_balance(*root, custnumber.account_number, current->data.balance);

        time_t now = time(0);
        char* time_str = ctime(&now);
        time_str[strlen(time_str) - 1] = 0;
        char log_str[100];
        sprintf(log_str, "Customer %d Made a Purchase at %s", custnumber.account_number, time_str);
        FILE* fl = fopen(LOG_FILE, "a");
        fprintf(fl, "%s\n", log_str);
        fclose(fl);
        printf("Item Purchase And Money Transfer Made successfully.\n");
        break;
    }

    case 3:
    {
        printf("Attention: You have reached the shekel trading gate\n\n");
        node* current = find_customer(*root, custnumber.account_number);
        if (current == NULL) {
            printf("Customer with account number %d does not exist.\n", custnumber.account_number);
            break;
        }
        current->data.balance -= amount;
        update_customer_balance(*root, custnumber.account_number, current->data.balance);

        time_t now = time(0);
        char* time_str = ctime(&now);
        time_str[strlen(time_str) - 1] = 0;
        char log_str[100];
        sprintf(log_str, "Customer %d Made a Purchase at %s", custnumber.account_number, time_str);
        FILE* fl = fopen(LOG_FILE, "a");
        fprintf(fl, "%s\n", log_str);
        fclose(fl);
        printf("Item Purchase And Money Transfer Made successfully.\n");
        break;
    }

    }
}
void printItems()
{
    item i;
    FILE* fc = fopen(ITEM_FILE, "rb");
    if (fc == NULL)
    {
        printf("Error opening item file\n");
        return;
    }
    while (fread(&i, sizeof(item), 1, fc) == 1)
        printf("name: %s  type: %c quantity: %lf SerialNumber: %d \n\n", i.name, i.type, i.quantity, i.serial_number);


    fclose(fc);
}


//void print_tree(node* root) {
//    if (root != NULL) {
//        print_tree(root->left);
//        printf("Account Number: %d, Name: %s, Owner Type: %c, Branch Number: %d, Management Fees: %lf, Balance: %d\n",
//            root->data.account_number, root->data.name, root->data.owner_type, root->data.branch_number,
//            root->data.management_fees, root->data.balance);
//        print_tree(root->right);
//    }
//}

void load_log()
{
    // check if log file exists
    FILE* logfile = fopen(LOG_FILE, "r");
    if (logfile == NULL) {
        // Create the file
        logfile = fopen(LOG_FILE, "w");
        fclose(logfile);
    }
    else {
        fclose(logfile);
    }


}
void load_data_to_ItemTree(item** root)
{

    // Check if the item file exists
    FILE* itemFile = fopen(ITEM_FILE, "rb");
    if (itemFile == NULL) {
        // Create the item file
        itemFile = fopen(ITEM_FILE, "wb");
        fclose(itemFile);
    }
    else {
        fclose(itemFile);
    }
    // Root of the item binary search tree
    //nodeItem* itemRoot = NULL;

    // Load all items from the file into the tree
    itemFile = fopen(ITEM_FILE, "rb");
    while (feof(itemFile)) {
        item newItem;
        fread(&newItem, sizeof(item), 1, itemFile);
        if (!feof(itemFile)) {
            *root = addItem(*root, newItem);
        }
    }
    fclose(itemFile);
}
void load_data_to_CustTree(node** root)
{

    FILE* customerFile = fopen(CUSTOMER_FILE, "rb");
    if (customerFile == NULL) {
        // Create the customer file
        customerFile = fopen(CUSTOMER_FILE, "wb");
        fclose(customerFile);
    }
    else {
        fclose(customerFile);
    }

    // Root of the customer binary search tree
    //node* customerRoot = NULL;

    // Load all customers from the file into the tree
    customerFile = fopen(CUSTOMER_FILE, "rb");
    cust newCustomer;
    while (fread(&newCustomer, sizeof(cust), 1, customerFile) == 1) {
        node* newNode = (node*)malloc(sizeof(node));
        newNode->data = newCustomer;
        newNode->left = NULL;
        newNode->right = NULL;
        newNode->parent = NULL;
        *root = newNode;
        num_customers++;
        //print_tree(*root);

    }

    fclose(customerFile);
}
void load_data_to_EmpTree(emp** root)
{
    // Check if the employee file exists
    FILE* employeeFile = fopen(EMPLOYEE_FILE, "rb");
    if (employeeFile == NULL) {
        // Create the employee file and add the default "admin" user
        employeeFile = fopen(EMPLOYEE_FILE, "wb");
        emp defaultUser;
        strcpy(defaultUser.username, "admin");
        strcpy(defaultUser.password, "admin");
        defaultUser.level = 3;
        defaultUser.employee_number = 001;
        strcpy(defaultUser.fullname, "Manager_System");
        fwrite(&defaultUser, sizeof(emp), 1, employeeFile);
        fclose(employeeFile);
    }
    else {
        fclose(employeeFile);
    }
    // void checkIfExists(const char *filename, const char *mode);
    // checkIfExists(CUSTOMER_FILE, "rb");
    // checkIfExists(ITEM_FILE);

    // Check if the customer file exists

    // Root of the employee binary search tree
    //nodeEmployee* employeeRoot = NULL;

    // Load all employees from the file into the tree
    employeeFile = fopen(EMPLOYEE_FILE, "rb");
    while (feof(employeeFile)) {
        emp newEmployee;
        fread(&newEmployee, sizeof(emp), 1, employeeFile);
        if (!feof(employeeFile)) {
            *root = addEmployee(*root, newEmployee);
            num_employees++;
        }
    }
    fclose(employeeFile);

}


int main()
{
    item* root_item = NULL;
    nodeEmployee* root_emp = NULL;
    node* root_cust = NULL;

    char temp;
    int custID;
    int empID;
    load_log();
    load_data_to_CustTree(&root_cust);
    load_data_to_ItemTree(&root_item);
    load_data_to_EmpTree(&root_emp);
    // Login loop
    int loginAttempts = 0;
    emp* loggedInUser = NULL;
    while (loginAttempts < 3 && loggedInUser == NULL)
    {
        printf("\n\n");
        printf("\t\t\t     ****************************************\n");
        printf("\t\t\t     *                                      *\n");
        printf("\t\t\t     *     ----------------------------     *\n");
        printf("\t\t\t     *    Bank Account Management System    *\n");
        printf("\t\t\t     *     ----------------------------     *\n");
        printf("\t\t\t     *                                      *\n");
        printf("\t\t\t     ****************************************\n");
        printf("\n\n");
        printf("\t\t\t     ****************************************\n");
        printf("\t\t\t     *       ------------------------       *\n");
        printf("\t\t\t     * Created by:Damian Fain & Refael Ivgi *\n");
        printf("\t\t\t     *       ------------------------       *\n");
        printf("\t\t\t     *                                      *\n");
        printf("\t\t\t     ****************************************\n");


        char username[15], password[15];
        printf("\t\t\t     Enter username:");
        scanf("%s", &username);

        printf("\t\t\t     Enter password:");
        scanf("%s", &password);

        loggedInUser = searchEmployee(username, password);
        if (loggedInUser == NULL)
        {
            printf("Invalid username or password. Please try again.\n");
            loginAttempts++;
        }
        while (loggedInUser != NULL)
        {
            if (strcmp(loggedInUser->username, username) == 0 && strcmp(loggedInUser->password, password) == 0)
            {
                int choice;
                do
                {
                    printf("\nWelcome, %s.\n\n", loggedInUser->fullname);
                    printf("0. Log out\n");
                    printf("1. View All Customers\n");
                    printf("2. Add Customer\n");
                    printf("3. Add New Item\n");
                    printf("4. Buy Item\n");


                    if (loggedInUser->level > 1)
                    {
                        printf("5. Delete Item\n");
                        printf("6. Add Employee \n");
                        printf("7. Delete Employee\n");


                    }
                    if (loggedInUser->level > 2)
                    {
                        printf("8. Delete Customer\n");
                        printf("9. View All Employess\n");
                        printf("10. View All Items \n");


                    }
                    scanf("%d", &choice);

                    switch (choice)
                    {
                    case 0:
                        printf("Logging out...\n");
                        loggedInUser = NULL;
                        break;
                    case 1:
                        printCustomers();
                        break;

                    case 2:
                    {
                        cust newCust;
                        scanf("%c", &temp);
                        printf("enter account name owner: ");
                        scanf("%[^\n]", &newCust.name);
                        printf("enter account number: ");
                        scanf("%d", &newCust.account_number);
                        printf("enter owner type: ");
                        scanf(" %c", &newCust.owner_type);
                        printf("enter branch number: ");
                        scanf("%d", &newCust.branch_number);
                        printf("enter management fees: ");
                        scanf("%lf", &newCust.management_fees);
                        printf("enter balance: ");
                        scanf("%d", &newCust.balance);

                        addCustomer(&root_cust, newCust);
                        break;

                    }
                    case 3:
                    {
                        item newitem;
                        scanf("%c", &temp);
                        printf("enter Item name: ");
                        scanf("%[^\n]", &newitem.name);
                        printf("enter Serial number: ");
                        scanf("%d", &newitem.serial_number);
                        printf("enter Item type: ");
                        scanf(" %c", &newitem.type);
                        printf("Enter Amount by type: ");
                        scanf("%lf", &newitem.quantity);
                        addItem(&root_item, newitem);
                        break;
                    }
                    case 4:

                        printCustomers();
                        int serialnum;
                        cust custnumber;
                        printf("enter Serial Number: ");
                        scanf("%d", &serialnum);
                        printf("enter Customer Number: ");
                        scanf("%d", &custnumber.account_number);
                        sellItem(&root_cust, serialnum, custnumber);

                        break;

                    case 5:
                    {
                        item newitem2;
                        printf("enter Serial number: ");
                        scanf("%d", &newitem2.serial_number);
                        deleteItem(&root_item, newitem2.serial_number);

                        break;
                    }
                    case 6:
                    {
                        emp newEmp;
                        printf("enter username: ");
                        scanf("%s", &newEmp.username);
                        printf("enter password: ");
                        scanf("%s", &newEmp.password);
                        printf("enter level (1-3): ");
                        scanf("%d", &newEmp.level);
                        printf("enter ID (3 digits): ");
                        scanf("%d", &newEmp.employee_number);
                        scanf("%c", &temp);
                        printf("enter full name: ");
                        scanf("%[^\n]", &newEmp.fullname);


                        addEmployee(&root_emp, newEmp);

                        break;
                    }
                    case 7:

                        printf("enter Employee ID:  ");
                        scanf("%d", &empID);
                        deleteEmployee(&root_emp, empID);

                        break;
                    case 8:

                        printf("enter customer number: ");
                        scanf("%d", &custID);
                        deleteCustomer(&root_cust, custID);
                        break;

                    case 9:
                        printEmp();
                        break;


                    case 10:
                    {

                        printItems();
                        break;

                    }


                    }
                } while (loggedInUser != NULL);

            }
        }
    }


    if (loggedInUser == NULL) {
        printf("\n\nYou have exceeded the maximum number of login attempts. Exiting program.\n");
        return 1;
    }

}