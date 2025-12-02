#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>

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

public:
    Electronics(int id, string name, double price, int quantity, string brand, int warranty)
        : InventoryItem(id, name, price, quantity, "Electronics"), 
          brand(brand), warrantyMonths(warranty) {}

    void display() const override {
        cout << left << setw(5) << id 
             << setw(20) << name 
             << setw(10) << "$" + to_string(price).substr(0, 5)
             << setw(10) << quantity
             << setw(15) << type
             << setw(15) << brand
             << warrantyMonths << " months warranty" << endl;
    }

    string getAdditionalInfo() const override {
        return "Brand: " + brand + ", Warranty: " + to_string(warrantyMonths) + " months";
    }
};

// Derived class for Grocery items
class Grocery : public InventoryItem {
    string expiryDate;
    string category;

public:
    Grocery(int id, string name, double price, int quantity, string expiry, string category)
        : InventoryItem(id, name, price, quantity, "Grocery"),
          expiryDate(expiry), category(category) {}

    void display() const override {
        cout << left << setw(5) << id 
             << setw(20) << name 
             << setw(10) << "$" + to_string(price).substr(0, 5)
             << setw(10) << quantity
             << setw(15) << type
             << setw(15) << category
             << "Expires: " << expiryDate << endl;
    }

    string getAdditionalInfo() const override {
        return "Category: " + category + ", Expires: " + expiryDate;
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
    void saveToFile() {
        ofstream outFile(dataFile);
        if (!outFile) {
            cerr << "Error: Could not open file for writing." << endl;
            return;
        }

        for (const auto& item : inventory) {
            outFile << item->getId() << ","
                   << item->getType() << ","
                   << item->getName() << ","
                   << item->getPrice() << ","
                   << item->getQuantity() << ",";
            
            // Save additional fields based on type
            if (item->getType() == "Electronics") {
                Electronics* e = dynamic_cast<Electronics*>(item);
                outFile << e->getAdditionalInfo() << "\n";
            } else if (item->getType() == "Grocery") {
                Grocery* g = dynamic_cast<Grocery*>(item);
                outFile << g->getAdditionalInfo() << "\n";
            }
        }
        outFile.close();
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
            delete item;
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
    void addItem(InventoryItem* item) {
        inventory.push_back(item);
        saveToFile();
    }

    // Update item quantity
    bool updateStock(int id, int amount) {
        InventoryItem* item = findItemById(id);
        if (item) {
            item->updateStock(amount);
            saveToFile();
            return true;
        }
        return false;
    }

    // Remove an item by ID
    bool removeItem(int id) {
        for (auto it = inventory.begin(); it != inventory.end(); ++it) {
            if ((*it)->getId() == id) {
                delete *it;
                inventory.erase(it);
                saveToFile();
                return true;
            }
        }
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
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Function to display the main menu
void displayMenu() {
    cout << "\n\n=== INVENTORY MANAGEMENT SYSTEM ===" << endl;
    cout << "1. Add New Item" << endl;
    cout << "2. Update Stock" << endl;
    cout << "3. Remove Item" << endl;
    cout << "4. View All Items" << endl;
    cout << "5. Generate Low Stock Report" << endl;
    cout << "6. Exit" << endl;
    cout << "\nEnter your choice (1-6): ";
}

// Function to add a new item
void addNewItem(InventorySystem& invSys) {
    cout << "\n=== ADD NEW ITEM ===" << endl;
    cout << "1. Electronics" << endl;
    cout << "2. Grocery" << endl;
    cout << "3. Back to Main Menu" << endl;
    cout << "\nSelect item type (1-3): ";
    
    int typeChoice;
    cin >> typeChoice;
    clearInput();

    if (typeChoice == 3) return;

    string name, brand, expiry, category;
    double price;
    int quantity, warranty;

    cout << "\nEnter item name: ";
    getline(cin, name);

    cout << "Enter price: $";
    while (!(cin >> price) || price < 0) {
        cout << "Invalid price. Please enter a positive number: $";
        clearInput();
    }
    clearInput();

    cout << "Enter quantity: ";
    while (!(cin >> quantity) || quantity < 0) {
        cout << "Invalid quantity. Please enter a positive integer: ";
        clearInput();
    }
    clearInput();

    int id = invSys.getNextId();
    InventoryItem* newItem = nullptr;

    switch (typeChoice) {
        case 1: // Electronics
            cout << "Enter brand: ";
            getline(cin, brand);
            cout << "Enter warranty period (months): ";
            while (!(cin >> warranty) || warranty < 0) {
                cout << "Invalid warranty period. Please enter a positive integer: ";
                clearInput();
            }
            clearInput();
            newItem = new Electronics(id, name, price, quantity, brand, warranty);
            break;

        case 2: // Grocery
            cout << "Enter category (e.g., Dairy, Snacks, etc.): ";
            getline(cin, category);
            cout << "Enter expiry date (YYYY-MM-DD): ";
            getline(cin, expiry);
            newItem = new Grocery(id, name, price, quantity, expiry, category);
            break;

        default:
            cout << "Invalid choice. Returning to main menu." << endl;
            return;
    }

    invSys.addItem(newItem);
    cout << "\nItem added successfully! ID: " << id << endl;
}

// Function to update stock
void updateStock(InventorySystem& invSys) {
    cout << "\n=== UPDATE STOCK ===" << endl;
    int id, amount;
    char action;

    cout << "Enter item ID: ";
    while (!(cin >> id) || id < 1) {
        cout << "Invalid ID. Please enter a positive number: ";
        clearInput();
    }
    clearInput();

    cout << "Add (A) or remove (R) stock? (A/R): ";
    while (!(cin >> action) || (toupper(action) != 'A' && toupper(action) != 'R')) {
        cout << "Invalid choice. Please enter 'A' to add or 'R' to remove: ";
        clearInput();
    }
    clearInput();

    cout << "Enter quantity to " << (toupper(action) == 'A' ? "add" : "remove") << ": ";
    while (!(cin >> amount) || amount <= 0) {
        cout << "Invalid amount. Please enter a positive number: ";
        clearInput();
    }
    clearInput();

    if (toupper(action) == 'R') {
        amount = -amount; // Make amount negative for removal
    }

    if (invSys.updateStock(id, amount)) {
        cout << "\nStock updated successfully!" << endl;
    } else {
        cout << "\nError: Item with ID " << id << " not found." << endl;
    }
}

// Function to remove an item
void removeItem(InventorySystem& invSys) {
    cout << "\n=== REMOVE ITEM ===" << endl;
    int id;

    cout << "Enter item ID to remove: ";
    while (!(cin >> id) || id < 1) {
        cout << "Invalid ID. Please enter a positive number: ";
        clearInput();
    }
    clearInput();

    if (invSys.removeItem(id)) {
        cout << "\nItem with ID " << id << " has been removed." << endl;
    } else {
        cout << "\nError: Item with ID " << id << " not found." << endl;
    }
}

// Main function
int main() {
    InventorySystem inventorySystem;
    int choice;

    cout << "========================================" << endl;
    cout << "   INVENTORY MANAGEMENT SYSTEM" << endl;
    cout << "========================================" << endl;

    do {
        displayMenu();
        
        while (!(cin >> choice) || choice < 1 || choice > 6) {
            cout << "Invalid choice. Please enter a number between 1 and 6: ";
            clearInput();
        }
        clearInput();

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
                break;
            case 5:
                inventorySystem.generateLowStockReport();
                break;
            case 6:
                cout << "\nThank you for using the Inventory Management System. Goodbye!" << endl;
                break;
        }

        if (choice != 6) {
            cout << "\nPress Enter to continue...";
            cin.ignore();
        }

    } while (choice != 6);

    return 0;
}
