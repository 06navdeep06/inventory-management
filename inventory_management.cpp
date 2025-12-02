#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <algorithm>  // For transform
#include <cctype>     // For tolower
#include <sstream>    // For string stream

using namespace std;

// Base class for all inventory items
class InventoryItem {
protected:
    int id;
    string name;
    double price;
    int quantity;
    string type;

public:
    InventoryItem(int id, string name, double price, int quantity, string type)
        : id(id), name(name), price(price), quantity(quantity), type(type) {}

    virtual ~InventoryItem() {}

    // Pure virtual function to display item details (to be overridden by derived classes)
    virtual void display() const = 0;
    
    // Virtual function to get item details as string (for file operations)
    virtual string getDetailsForFile() const = 0;

    // Getters
    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    string getType() const { return type; }

    // Setters
    void setPrice(double newPrice) { price = newPrice; }
    void setQuantity(int newQuantity) { quantity = newQuantity; }
    void updateStock(int amount) { quantity += amount; }

    // Virtual function for polymorphic behavior
    virtual string getAdditionalInfo() const { return ""; }
};

// Derived class for Electronics items
class Electronics : public InventoryItem {
    string brand;
    int warrantyMonths;

    // Helper to format price with 2 decimal places
    string formatPrice(double price) const {
        stringstream ss;
        ss << fixed << setprecision(2) << price;
        return "$" + ss.str();
    }

public:
    Electronics(int id, string name, double price, int quantity, string brand, int warranty)
        : InventoryItem(id, name, price, quantity, "Electronics"), 
          brand(brand), warrantyMonths(max(0, warranty)) {}

    void display() const override {
        cout << left << setw(5) << id 
             << setw(25) << (name.length() > 23 ? name.substr(0, 20) + "..." : name) 
             << setw(12) << formatPrice(price)
             << setw(8) << quantity
             << setw(15) << type
             << setw(20) << (brand.length() > 17 ? brand.substr(0, 15) + "..." : brand)
             << warrantyMonths << " months" << endl;
    }

    string getAdditionalInfo() const override {
        return "Brand: " + brand + ", Warranty: " + to_string(warrantyMonths) + " months";
    }
    
    string getDetailsForFile() const override {
        return brand + "," + to_string(warrantyMonths);
    }
};

// Derived class for Grocery items
class Grocery : public InventoryItem {
    string expiryDate;
    string category;

    // Helper to format price with 2 decimal places
    string formatPrice(double price) const {
        stringstream ss;
        ss << fixed << setprecision(2) << price;
        return "$" + ss.str();
    }

    // Helper to validate date format (YYYY-MM-DD)
    bool isValidDate(const string& date) const {
        if (date.length() != 10) return false;
        if (date[4] != '-' || date[7] != '-') return false;
        // Basic check - in a real app, you'd want more thorough validation
        return true;
    }

public:
    Grocery(int id, string name, double price, int quantity, string expiry, string category)
        : InventoryItem(id, name, price, quantity, "Grocery"),
          expiryDate(expiry), category(category) {
        if (!isValidDate(expiry)) {
            cerr << "Warning: Invalid date format for item " << id << ". Using default date.\n";
            expiryDate = "2023-12-31";
        }
    }

    void display() const override {
        cout << left << setw(5) << id 
             << setw(25) << (name.length() > 23 ? name.substr(0, 20) + "..." : name)
             << setw(12) << formatPrice(price)
             << setw(8) << quantity
             << setw(15) << type
             << setw(20) << (category.length() > 17 ? category.substr(0, 15) + "..." : category)
             << "Exp: " << expiryDate << endl;
    }

    string getAdditionalInfo() const override {
        return "Category: " + category + ", Expires: " + expiryDate;
    }
    
    string getDetailsForFile() const override {
        return expiryDate + "," + category;
    }
};

// Inventory management system class
class InventorySystem {
private:
    vector<InventoryItem*> inventory;
    string dataFile = "inventory_data.txt";
    int nextId = 1;

    // Helper function to find an item by ID
    InventoryItem* findItemById(int id) {
        for (auto& item : inventory) {
            if (item->getId() == id) {
                return item;
            }
        }
        return nullptr;
    }

    // Save inventory to file
    bool saveToFile() {
        ofstream outFile(dataFile);
        if (!outFile.is_open()) {
            cerr << "\nError: Could not open file '" << dataFile << "' for writing." << endl;
            return false;
        }

        bool success = true;
        for (const auto& item : inventory) {
            try {
                outFile << item->getId() << ","
                       << item->getType() << ","
                       << item->getName() << ","
                       << item->getPrice() << ","
                       << item->getQuantity() << ","
                       << item->getDetailsForFile() << "\n";
            } catch (const exception& e) {
                cerr << "Error saving item " << item->getId() << ": " << e.what() << endl;
                success = false;
            }
        }
        
        outFile.close();
        if (success) {
            cout << "\n✓ Inventory saved successfully!" << endl;
        }
        return success;
    }

    // Load inventory from file
    void loadFromFile() {
        ifstream inFile(dataFile);
        if (!inFile) {
            cout << "No existing data file found. Starting with empty inventory.\n";
            return;
        }

        string line;
        while (getline(inFile, line)) {
            // Parse the line and create appropriate objects
            // This is a simplified version - in a real app, you'd need more robust parsing
            // and error handling
            size_t pos = 0;
            string token;
            vector<string> tokens;
            
            while ((pos = line.find(',')) != string::npos) {
                token = line.substr(0, pos);
                tokens.push_back(token);
                line.erase(0, pos + 1);
            }
            tokens.push_back(line); // Add the last part

            if (tokens.size() >= 5) {
                int id = stoi(tokens[0]);
                string type = tokens[1];
                string name = tokens[2];
                double price = stod(tokens[3]);
                int quantity = stoi(tokens[4]);

                if (type == "Electronics") {
                    // Parse additional info (simplified)
                    inventory.push_back(new Electronics(id, name, price, quantity, "Brand", 12));
                } else if (type == "Grocery") {
                    inventory.push_back(new Grocery(id, name, price, quantity, "2024-12-31", "Category"));
                }

                if (id >= nextId) {
                    nextId = id + 1;
                }
            }
        }
        inFile.close();
    }

    // Clear all items from memory
    void clearInventory() {
        for (auto& item : inventory) {
            if (item != nullptr) {
                delete item;
                item = nullptr;
            }
        }
        inventory.clear();
    }

public:
    InventorySystem() {
        loadFromFile();
    }

    ~InventorySystem() {
        clearInventory();
    }

    // Add a new item to inventory
    bool addItem(InventoryItem* item) {
        if (item == nullptr) {
            cerr << "Error: Cannot add null item to inventory." << endl;
            return false;
        }
        
        // Check for duplicate IDs
        if (findItemById(item->getId()) != nullptr) {
            cerr << "Error: Item with ID " << item->getId() << " already exists." << endl;
            delete item;
            return false;
        }
        
        inventory.push_back(item);
        return saveToFile();
    }

    // Update item quantity
    bool updateStock(int id, int amount) {
        InventoryItem* item = findItemById(id);
        if (item) {
            int newQuantity = item->getQuantity() + amount;
            if (newQuantity < 0) {
                cout << "\nWarning: Cannot have negative quantity. Operation cancelled." << endl;
                return false;
            }
            
            item->updateStock(amount);
            bool success = saveToFile();
            if (success) {
                cout << "\n✓ Stock updated successfully! New quantity: " << newQuantity << endl;
            }
            return success;
        }
        cout << "\nError: Item with ID " << id << " not found." << endl;
        return false;
    }

    // Remove an item by ID
    bool removeItem(int id) {
        for (auto it = inventory.begin(); it != inventory.end(); ++it) {
            if ((*it)->getId() == id) {
                string itemName = (*it)->getName();
                delete *it;
                inventory.erase(it);
                bool success = saveToFile();
                if (success) {
                    cout << "\n✓ Item '" << itemName << "' (ID: " << id << ") has been removed." << endl;
                }
                return success;
            }
        }
        cout << "\nError: Item with ID " << id << " not found." << endl;
        return false;
    }

    // Display all items in inventory
    void displayInventory() const {
        if (inventory.empty()) {
            cout << "\nNo items in inventory.\n";
            return;
        }

        cout << "\n" << string(90, '=') << endl;
        cout << "                                 INVENTORY" << endl;
        cout << string(90, '=') << endl;
        cout << left << setw(5) << "ID" 
             << setw(20) << "Name" 
             << setw(10) << "Price" 
             << setw(10) << "Qty" 
             << setw(15) << "Type" 
             << setw(15) << "Details" 
             << "Additional Info" << endl;
        cout << string(90, '-') << endl;

        for (const auto& item : inventory) {
            item->display();
        }
        cout << string(90, '=') << endl;
    }

    // Generate a simple low stock report
    void generateLowStockReport(int threshold = 5) const {
        cout << "\n\n" << string(50, '=') << endl;
        cout << "           LOW STOCK REPORT (Below " << threshold << " items)" << endl;
        cout << string(50, '=') << endl;
        cout << left << setw(5) << "ID" 
             << setw(20) << "Name" 
             << setw(10) << "Qty" 
             << setw(15) << "Type" << endl;
        cout << string(50, '-') << endl;

        bool found = false;
        for (const auto& item : inventory) {
            if (item->getQuantity() < threshold) {
                cout << left << setw(5) << item->getId()
                     << setw(20) << item->getName()
                     << setw(10) << item->getQuantity()
                     << setw(15) << item->getType() << endl;
                found = true;
            }
        }

        if (!found) {
            cout << "No items below threshold of " << threshold << " units.\n";
        }
        cout << string(50, '=') << endl;
    }

    // Get next available ID
    int getNextId() {
        return nextId++;
    }
};

// Function to clear input buffer
void clearInput() {
    if (cin.fail()) {
        cin.clear();
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Function to get a string input with validation
string getStringInput(const string& prompt) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        
        // Trim leading/trailing whitespace
        input.erase(0, input.find_first_not_of(" \t"));
        input.erase(input.find_last_not_of(" \t") + 1);
        
        if (!input.empty()) {
            return input;
        }
        cout << "Error: Input cannot be empty. Please try again.\n";
    }
}

// Function to get a positive number input
template<typename T>
T getPositiveNumber(const string& prompt) {
    T value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= 0) {
            clearInput();
            return value;
        }
        cout << "Error: Please enter a valid positive number.\n";
        clearInput();
    }
}

// Function to get a yes/no response
bool getYesNo(const string& prompt) {
    char response;
    while (true) {
        cout << prompt << " (y/n): ";
        cin >> response;
        clearInput();
        
        response = tolower(response);
        if (response == 'y' || response == 'n') {
            return (response == 'y');
        }
        cout << "Please enter 'y' for yes or 'n' for no.\n";
    }
}

// Function to display the main menu
int displayMenu() {
    int choice;
    cout << "\n" << string(50, '=') << endl;
    cout << "       INVENTORY MANAGEMENT SYSTEM" << endl;
    cout << string(50, '=') << endl;
    cout << "1. Add New Item" << endl;
    cout << "2. Update Stock" << endl;
    cout << "3. Remove Item" << endl;
    cout << "4. View All Items" << endl;
    cout << "5. Generate Low Stock Report" << endl;
    cout << "6. Exit" << endl;
    
    while (true) {
        cout << "\nEnter your choice (1-6): ";
        if (cin >> choice && choice >= 1 && choice <= 6) {
            clearInput();
            return choice;
        }
        cout << "Invalid choice. Please enter a number between 1 and 6.\n";
        clearInput();
    }
}

// Function to add a new item
void addNewItem(InventorySystem& invSys) {
    while (true) {
        cout << "\n" << string(30, '=') << endl;
        cout << "      ADD NEW ITEM" << endl;
        cout << string(30, '=') << endl;
        cout << "1. Add Electronics" << endl;
        cout << "2. Add Grocery Item" << endl;
        cout << "3. Back to Main Menu" << endl;
        
        int typeChoice;
        cout << "\nSelect item type (1-3): ";
        if (!(cin >> typeChoice) || typeChoice < 1 || typeChoice > 3) {
            cout << "\nInvalid choice. Please enter a number between 1 and 3." << endl;
            clearInput();
            continue;
        }
        clearInput();

        if (typeChoice == 3) return;

    string name, brand, expiry, category;
    double price;
    int quantity, warranty;
    
    // Get common item details
    name = getStringInput("\nEnter item name: ");
    price = getPositiveNumber<double>("Enter price: $");
    quantity = getPositiveNumber<int>("Enter initial quantity: ");
    
    int id = invSys.getNextId();
    InventoryItem* newItem = nullptr;
    bool success = false;

    try {
        switch (typeChoice) {
            case 1: { // Electronics
                brand = getStringInput("Enter brand: ");
                warranty = getPositiveNumber<int>("Enter warranty period (months): ");
                newItem = new Electronics(id, name, price, quantity, brand, warranty);
                break;
            }

            case 2: { // Grocery
                category = getStringInput("Enter category (e.g., Dairy, Snacks, etc.): ");
                
                // Get and validate expiry date
                while (true) {
                    expiry = getStringInput("Enter expiry date (YYYY-MM-DD): ");
                    if (expiry.length() == 10 && expiry[4] == '-' && expiry[7] == '-') {
                        break;
                    }
                    cout << "Invalid date format. Please use YYYY-MM-DD format.\n";
                }
                
                newItem = new Grocery(id, name, price, quantity, expiry, category);
                break;
            }
            
            default:
                return; // Shouldn't happen due to previous validation
        }

        // Add the item to inventory
        success = invSys.addItem(newItem);
        if (success) {
            cout << "\n✓ Item added successfully!" << endl;
            cout << "  ID: " << id << endl;
            cout << "  Name: " << name << endl;
            cout << "  Type: " << (typeChoice == 1 ? "Electronics" : "Grocery") << endl;
        }
    } catch (const exception& e) {
        cerr << "\nError adding item: " << e.what() << endl;
        if (newItem != nullptr) {
            delete newItem;
        }
    }
    
    // Pause before returning to menu
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Function to update stock
void updateStock(InventorySystem& invSys) {
    cout << "\n" << string(30, '=') << endl;
    cout << "      UPDATE STOCK" << endl;
    cout << string(30, '=') << endl;
    
    // First show current inventory so user can see IDs
    cout << "\nCurrent Inventory:" << endl;
    invSys.displayInventory();
    
    // Get item ID
    int id = getPositiveNumber<int>("\nEnter item ID to update: ");
    
    // Get action (add/remove)
    char action;
    while (true) {
        cout << "Add (A) or remove (R) stock? (A/R): ";
        cin >> action;
        action = toupper(action);
        if (action == 'A' || action == 'R') break;
        cout << "Invalid choice. Please enter 'A' to add or 'R' to remove.\n";
        clearInput();
    }
    clearInput();
    
    // Get amount
    string actionText = (action == 'A') ? "add" : "remove";
    int amount = getPositiveNumber<int>("Enter quantity to " + actionText + ": ");
    
    if (action == 'R') {
        amount = -amount; // Make amount negative for removal
    }
    
    // Update stock
    invSys.updateStock(id, amount);
}

// Function to remove an item
void removeItem(InventorySystem& invSys) {
    cout << "\n" << string(30, '=') << endl;
    cout << "      REMOVE ITEM" << endl;
    cout << string(30, '=') << endl;
    
    // First show current inventory so user can see IDs
    cout << "\nCurrent Inventory:" << endl;
    invSys.displayInventory();
    
    // Get item ID
    int id = getPositiveNumber<int>("\nEnter item ID to remove: ");
    
    // Confirm deletion
    if (getYesNo("Are you sure you want to remove this item?")) {
        invSys.removeItem(id);
    } else {
        cout << "\nOperation cancelled." << endl;
    }
}

// Main function
int main() {
    // Set console output to show 2 decimal places for currency
    cout << fixed << setprecision(2);
    
    // Create inventory system
    InventorySystem inventorySystem;
    
    cout << string(50, '=') << endl;
    cout << "   INVENTORY MANAGEMENT SYSTEM" << endl;
    cout << "   Version 1.0" << endl;
    cout << string(50, '=') << endl;
    
    // Check if inventory was loaded
    cout << "\nType 'help' at any time to see available commands.\n";
    
    // Main loop
    int choice;
    do {
        try {
            choice = displayMenu();
            
            switch (choice) {
                case 1:
                    addNewItem(inventorySystem);
                    break;
                    
                case 2:
                    updateStock(inventorySystem);
                    break;
                    
                case 3:
                    removeItem(inventorySystem);
                    break;
                    
                case 4:
                    inventorySystem.displayInventory();
                    cout << "\nPress Enter to continue...";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                    
                case 5: {
                    int threshold = getPositiveNumber<int>("\nEnter low stock threshold (default 5): ");
                    inventorySystem.generateLowStockReport(threshold);
                    cout << "\nPress Enter to continue...";
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                
                case 6:
                    if (getYesNo("\nAre you sure you want to exit?")) {
                        cout << "\nThank you for using the Inventory Management System. Goodbye!" << endl;
                    } else {
                        choice = 0; // Stay in the menu
                    }
                    break;
            }
        } catch (const exception& e) {
            cerr << "\nError: " << e.what() << endl;
            cout << "Press Enter to continue...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        
    } while (choice != 6);

    return 0;
}
