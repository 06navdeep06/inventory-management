#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <cctype>

using namespace std;

// Simple structure to hold item details
struct Item {
    int id;
    string name;
    int quantity;
    double price;
    string category;  // could be more specific if needed

    Item(int _id, string _name, int _qty, double _price, string _cat = "General") 
        : id(_id), name(_name), quantity(_qty), price(_price), category(_cat) {}
};

class Inventory {
private:
    vector<Item> items;
    int nextId = 1;  // simple auto-increment
    
    // Helper to find item by ID
    vector<Item>::iterator findItem(int id) {
        return find_if(items.begin(), items.end(), 
            [id](const Item& i) { return i.id == id; });
    }

public:
    // Add new item to inventory
    void addItem(string name, int qty, double price, string cat = "General") {
        // Quick validation
        if (qty < 0 || price < 0) {
            cout << "Error: Quantity and price must be non-negative\n";
            return;
        }
        
        // Check for existing item with same name
        auto it = find_if(items.begin(), items.end(), 
            [&name](const Item& i) { return i.name == name; });
            
        if (it != items.end()) {
            cout << "Item already exists! Updating instead...\n";
            it->quantity += qty;
            return;
        }
        
        items.emplace_back(nextId++, name, qty, price, cat);
        cout << "Added: " << name << " (ID: " << nextId-1 << ")\n";
    }

    // Remove item by ID
    void removeItem(int id) {
        auto it = findItem(id);
        if (it != items.end()) {
            string name = it->name;
            items.erase(it);
            cout << "Removed: " << name << "\n";
        } else {
            cout << "Item not found!\n";
        }
    }

    // Update item quantity
    void updateStock(int id, int delta) {
        auto it = findItem(id);
        if (it != items.end()) {
            it->quantity += delta;
            if (it->quantity < 0) {
                cout << "Warning: Negative stock detected for " << it->name << endl;
                // Could add more handling here
            }
        } else {
            cout << "Item not found!\n";
        }
    }

    // Display inventory
    void listItems() const {
        if (items.empty()) {
            cout << "Inventory is empty!\n";
            return;
        }

        cout << "\n===== INVENTORY =====\n";
        cout << left << setw(5) << "ID" 
             << setw(20) << "Name" 
             << setw(10) << "Quantity" 
             << setw(10) << "Price" 
             << "Category\n";
        cout << string(60, '-') << "\n";
        
        for (const auto& item : items) {
            cout << left << setw(5) << item.id
                 << setw(20) << item.name
                 << setw(10) << item.quantity
                 << "$" << fixed << setprecision(2) << item.price
                 << "  " << item.category << "\n";
        }
        cout << "=====================\n\n";
    }

    // Helper function for case-insensitive string search
    bool containsIgnoreCase(string str, const string& search) {
        // Convert both strings to lowercase
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        string searchLower = search;
        transform(searchLower.begin(), searchLower.end(), searchLower.begin(), ::tolower);
        
        // Simple substring search
        return str.find(searchLower) != string::npos;
    }

    // Check if inventory is empty
    bool isEmpty() const {
        return items.empty();
    }

    // Enhanced search functionality
    void searchItem(const string& query) {
        if (query.empty()) {
            cout << "\nPlease enter a search term.\n";
            return;
        }

        vector<Item> results;
        string queryLower = query;
        transform(queryLower.begin(), queryLower.end(), queryLower.begin(), ::tolower);
        
        // Search in all relevant fields
        for (const auto& item : items) {
            string idStr = to_string(item.id);
            string nameLower = item.name;
            string catLower = item.category;
            
            transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
            transform(catLower.begin(), catLower.end(), catLower.begin(), ::tolower);
            
            if (containsIgnoreCase(item.name, query) ||
                containsIgnoreCase(item.category, query) ||
                idStr.find(query) != string::npos) {
                results.push_back(item);
            }
        }

        // Display results
        if (!results.empty()) {
            cout << "\n===== SEARCH RESULTS =====";
            cout << "\nFound " << results.size() << " matching item" << (results.size() != 1 ? "s" : "") << "\n";
            
            for (const auto& item : results) {
                cout << "\nID: " << item.id
                     << "\nName: " << item.name
                     << "\nCategory: " << item.category
                     << "\nQuantity: " << item.quantity
                     << "\nPrice: $" << fixed << setprecision(2) << item.price
                     << "\n" << string(30, '-') << "\n";
            }
        } else {
            cout << "\nNo items found matching '\033[1m" << query << "\033[0m'\n";
            cout << "Tip: Try searching by name, category, or ID\n";
        }
    }
};

// Clear screen function for better UX
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Get integer input with validation
int getIntegerInput(const string& prompt, int min = INT_MIN, int max = INT_MAX) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value >= min && value <= max) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            }
            cout << "Please enter a value between " << min << " and " << max << ".\n";
        } else {
            cout << "Invalid input. Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Get double input with validation
double getDoubleInput(const string& prompt, double min = 0.0) {
    double value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value >= min) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return value;
            }
            cout << "Please enter a value greater than or equal to " << min << ".\n";
        } else {
            cout << "Invalid input. Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// Get string input with validation
string getStringInput(const string& prompt, bool allowEmpty = false) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        
        // Trim whitespace
        input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));
        input.erase(input.find_last_not_of(" \t\n\r\f\v") + 1);
        
        if (!input.empty() || allowEmpty) {
            return input;
        }
        cout << "This field cannot be empty. Please try again.\n";
    }
}

// Display the main menu
int showMainMenu() {
    clearScreen();
    cout << "\n       INVENTORY MANAGEMENT SYSTEM\n";
    cout << "       ----------------------\n\n";
    cout << "1. Add new item\n";
    cout << "2. Remove item\n";
    cout << "3. Update stock\n";
    cout << "4. View all items\n";
    cout << "5. Search items\n";
    cout << "6. Exit\n";
    
    return getIntegerInput("\nEnter your choice (1-6): ", 1, 6);
}

int main() {
    Inventory inv;
    bool running = true;
    
    // Add some sample data
    inv.addItem("Laptop", 10, 999.99, "Electronics");
    inv.addItem("Wireless Mouse", 50, 24.99, "Accessories");
    inv.addItem("Mechanical Keyboard", 25, 89.99, "Accessories");
    inv.addItem("Monitor 24\"", 15, 199.99, "Electronics");
    
    cout << "Welcome to the Inventory Management System!\n";
    cout << "Managing your inventory made simple and efficient.\n\n";
    cout << "Press Enter to continue...";
    cin.ignore();
    
    while (running) {
        int choice = showMainMenu();
        
        switch (choice) {
            case 1: {  // Add new item
                clearScreen();
                cout << "\n        ADD NEW ITEM\n";
                cout << "        ------------\n\n";
                
                string name = getStringInput("Item name: ");
                string category = getStringInput("Category (press Enter for 'General'): ", true);
                if (category.empty()) category = "General";
                
                int quantity = getIntegerInput("Quantity: ", 0);
                double price = getDoubleInput("Price per unit: $", 0.0);
                
                // Confirm before adding
                cout << "\nAdd this item to inventory?\n";
                cout << "Name:     " << name << "\n";
                cout << "Category: " << category << "\n";
                cout << "Quantity: " << quantity << "\n";
                cout << "Price:    $" << fixed << setprecision(2) << price << "\n\n";
                
                string confirm = getStringInput("Confirm (y/n)? ");
                if (!confirm.empty() && tolower(confirm[0]) == 'y') {
                    inv.addItem(name, quantity, price, category);
                    cout << "\nItem added successfully!\n";
                } else {
                    cout << "\nItem addition cancelled.\n";
                }
                break;
            }
            case 2: {  // Remove item
                clearScreen();
                cout << "\n        REMOVE ITEM\n";
                cout << "        ------------\n\n";
                inv.listItems();
                
                if (!inv.isEmpty()) {
                    int id = getIntegerInput("\nEnter ID of item to remove (0 to cancel): ", 0);
                    if (id != 0) {
                        string confirm = getStringInput("Are you sure you want to remove this item? (y/n): ");
                        if (tolower(confirm[0]) == 'y') {
                            inv.removeItem(id);
                            cout << "\nItem removed successfully!\n";
                        } else {
                            cout << "\nItem removal cancelled.\n";
                        }
                    }
                }
                
                cout << "\nPress Enter to continue...";
                cin.ignore();
                break;
            }
            case 3: {  // Update stock
                clearScreen();
                cout << "\n        UPDATE STOCK\n";
                cout << "        ------------\n\n";
                inv.listItems();
                
                if (!inv.isEmpty()) {
                    int id = getIntegerInput("\nEnter item ID to update (0 to cancel): ", 0);
                    if (id != 0) {
                        int delta = getIntegerInput("Enter quantity to add (use negative to remove): ");
                        inv.updateStock(id, delta);
                        cout << "\nStock updated successfully!\n";
                    }
                }
                
                cout << "\nPress Enter to continue...";
                cin.ignore();
                break;
            }
            case 4:  // View all items
                clearScreen();
                cout << "\n        INVENTORY OVERVIEW\n";
                cout << "        ----------------\n\n";
                inv.listItems();
                cout << "Press Enter to continue...";
                cin.ignore();
                break;
                
            case 5: {  // Search items
                clearScreen();
                cout << "\n        SEARCH INVENTORY\n";
                cout << "        ----------------\n\n";
                string query = getStringInput("Search by name, category, or ID: ", true);
                if (!query.empty()) {
                    clearScreen();
                    cout << "\n        SEARCH RESULTS\n";
                    cout << "        --------------\n\n";
                    inv.searchItem(query);
                }
                cout << "\nPress Enter to continue...";
                cin.ignore();
                break;
            }
            
            case 6:  // Exit
                clearScreen();
                cout << "\n        GOODBYE!\n";
                cout << "        -------\n\n";
                cout << "Thank you for using the Inventory Management System.\n";
                cout << "Have a great day!\n\n";
                running = false;
                break;
                
            default:
                cout << "\nInvalid choice. Please try again.\n";
                cout << "Press Enter to continue...";
                cin.ignore();
        }
    }
    
    return 0;
}