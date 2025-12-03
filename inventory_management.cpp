// Inventory management system 
// Banaeko: 2025/03/12

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <cctype>

using namespace std;

// Euta saman (item) ko details rakhne struct
struct Item {
    int id;            // Unique ID number
    string name;       // Saman ko naam
    int quantity;      // Kati ota cha stock ma
    double price;      // Euta ko price
    string category;   // Kasto type ko saman ho

    // Naya saman thapne constructor
    Item(int itemId, string itemName, int qty, double itemPrice, string cat = "General") 
        : id(itemId), name(itemName), quantity(qty), price(itemPrice), category(cat) {}
};

// Saman haru lai manage garne class
class Inventory {
private:
    vector<Item> items;  // Sabai saman haru yaha store huncha
    int nextId = 1;      // Aarko naya ID k hune bhanera track garcha
    
    // ID accordingly saman khojne, payena bhane end() return garcha
    vector<Item>::iterator findItem(int id) {
        return find_if(items.begin(), items.end(), 
            [id](const Item& i) { 
                return i.id == id; 
            });
    }

public:
    // Naya saman inventory ma thapne function
    void addItem(string name, int qty, double price, string cat = "General") {
        // Negative quantity ra price check garcha
        if (qty < 0 || price < 0) {
            cout << "Error: Can't have negative quantities or prices!\n";
            return;
        }
        
        // Pahila nai yo item cha ki check garcha (naam herera)
        auto existing = find_if(items.begin(), items.end(), 
            [&name](const Item& i) { 
                return i.name == name; 
            });
            
        if (existing != items.end()) {
            // Pahila nai cha, quantity matra update garcha
            cout << "Found existing item - adding to stock...\n";
            existing->quantity += qty;
            return;
        }
        
        // Naya ID diyera naya saman thapcha
        int newId = nextId++;
        items.emplace_back(newId, name, qty, price, cat);
        cout << "Added new item: " << name << " (ID: " << newId << ")\n";
    }

    // ID diyera saman hataune
    void removeItem(int id) {
        auto itemToRemove = findItem(id);
        if (itemToRemove != items.end()) {
            string itemName = itemToRemove->name;
            items.erase(itemToRemove);
            cout << "Successfully removed: " << itemName << "\n";
        } else {
            cout << "Oops! Couldn't find an item with ID " << id << "\n";
        }
    }

    // Stock ko quantity update garna
    void updateStock(int itemId, int amount) {
        auto item = findItem(itemId);
        if (item != items.end()) {
            item->quantity += amount;
            
            // Stock negative bhayeko kura user lai inform garcha
            if (item->quantity < 0) {
                cout << "Warning: " << item->name << " now has negative stock! (" 
                     << item->quantity << ")\n";
            }
        } else {
            cout << "Couldn't find item with ID " << itemId << "\n";
        }
    }

    // Sabai saman haru dekhaune function
    void listItems() const {
        if (items.empty()) {
            cout << "The inventory is currently empty.\n";
            return;
        }

        // print the heading
        cout << "\n=== CURRENT INVENTORY ===\n";
        cout << left << setw(6) << "ID" 
             << setw(25) << "PRODUCT NAME" 
             << setw(12) << "QUANTITY" 
             << setw(12) << "PRICE" 
             << "CATEGORY\n";
        cout << string(60, '-') << "\n";
        
        // Sabai saman haru dekhaucha
        for (const auto& item : items) {
            cout << left << setw(6) << item.id
                 << setw(25) << (item.name.length() > 22 ? item.name.substr(0, 19) + "..." : item.name)
                 << setw(12) << item.quantity
                 << "$" << fixed << setprecision(2) << setw(10) << item.price
                 << item.category << "\n";
        }
        cout << string(60, '=') << "\n\n";
    }

    // Euta string arko string ma cha ki check garcha (capital small farak gardaina)
    bool containsIgnoreCase(string text, const string& searchTerm) {
        // Donai string lai lowercase ma convert garcha
        transform(text.begin(), text.end(), text.begin(), ::tolower);
        string lowerSearch = searchTerm;
        transform(lowerSearch.begin(), lowerSearch.end(), lowerSearch.begin(), ::tolower);
        
        return text.find(lowerSearch) != string::npos;
    }

    // Inventory khali cha ki chaina check garcha
    bool isEmpty() const {
        return items.empty();
    }

    // Saman haru khojne function (naam, category, or ID le)
    void searchItem(const string& searchTerm) {
        if (searchTerm.empty()) {
            cout << "\nPlease enter something to search for.\n";
            return;
        }

        vector<Item> matches;
        
        // ID le khojnu pareko ho ki check garcha
        bool isIdSearch = !searchTerm.empty() && all_of(searchTerm.begin(), searchTerm.end(), ::isdigit);
        
        // Sabai saman haru ma loop chalau
        for (const auto& item : items) {
            // ID le khojnu pareko bhane tyo check garcha
            if (isIdSearch && to_string(item.id) == searchTerm) {
                matches.push_back(item);
                continue;
            }
            
            // Name ra category ma khojcha
            if (containsIgnoreCase(item.name, searchTerm) || 
                containsIgnoreCase(item.category, searchTerm)) {
                matches.push_back(item);
            }
        }

        // Kati ota payeo bhanera dekhaucha
        if (matches.empty()) {
            cout << "\nNo matches found for '" << searchTerm << "'\n";
            cout << "Try searching by product name, category, or ID\n";
            return;
        }
        
        cout << "\n=== SEARCH RESULTS (" << matches.size() << " found) ===\n";
        for (const auto& item : matches) {
            cout << "\nID: " << item.id
                 << "\nName: " << item.name
                 << "\nCategory: " << item.category
                 << "\nIn stock: " << item.quantity
                 << "\nPrice: $" << fixed << setprecision(2) << item.price
                 << "\n" << string(30, '-') << "\n";
        }
    }
};

// Screen clear garne function
void clearScreen() {
        system("cls");     // For Windows
  
}

// User bata number lini with validation
int getIntegerInput(const string& prompt, int min = INT_MIN, int max = INT_MAX) {
    int num;
    while (true) {
        cout << prompt;
        
        if (cin >> num) {
            // Range ma cha ki check garcha
            if (num >= min && num <= max) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return num;
            }
            cout << "Please enter a number between " << min << " and " << max << ".\n";
        } else {
            // wrong input diyo bhane
            cout << "That's not a valid number. Try again.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// User bata decimal number lini
double getDoubleInput(const string& prompt, double min = 0.0) {
    double num;
    while (true) {
        cout << prompt;
        
        if (cin >> num) {
            if (num >= min) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return num;
            }
            cout << "Please enter at least " << min << ".\n";
        } else {
            cout << "Please enter a valid number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

// User bata text input lini
string getStringInput(const string& prompt, bool canBeEmpty = false) {
    string input;
    
    while (true) {
        cout << prompt;
        getline(cin, input);
        
        // Input lai clean garcha
        input.erase(0, input.find_first_not_of(" \t\n\r\f\v"));  // Trim left
        input.erase(input.find_last_not_of(" \t\n\r\f\v") + 1);  // Trim right
        
        if (!input.empty() || canBeEmpty) {
            return input;
        }
        
        cout << "You need to enter something!\n";
    }
}

// Menu dekhaune ra user ko selection line
int showMainMenu() {
    clearScreen();
    
    // Menu ko options haru print garcha
    cout << "\n=== INVENTORY MANAGEMENT ===\n\n"
         << "1. Add a new item\n"
         << "2. Remove an item\n"
         << "3. Update stock level\n"
         << "4. View all items\n"
         << "5. Search for items\n"
         << "6. Exit\n\n";
    
    // User le select gareko option return garcha
    return getIntegerInput("Enter your choice (1-6): ", 1, 6);
}

int main() {
    // Create our inventory system
    Inventory inventory;
    bool keepRunning = true;
    
    // Add some sample items to start with
    inventory.addItem("Laptop", 5, 1299.99, "Electronics");
    inventory.addItem("Wireless Mouse", 10, 29.99, "Accessories");
    inventory.addItem("Mechanical Keyboard", 8, 89.99, "Accessories");
    inventory.addItem("27\" 4K Monitor", 3, 349.99, "Monitors");
    
    // Welcome message
    cout << "=== WELCOME TO INVENTORY MANAGER ===\n";
    cout << "Your one-stop solution for inventory tracking!\n\n";
    cout << "Press Enter to begin...";
    cin.ignore();
    
    // Main program loop
    while (keepRunning) {
        int choice = showMainMenu();
        
        switch (choice) {
            case 1: {  // Add item
                clearScreen();
                cout << "\n--- ADD NEW ITEM ---\n\n";
                
                // Saman ko details haru soddha
                string name = getStringInput("What's the item called? ");
                string category = getStringInput("Category (press Enter for 'General'): ", true);
                if (category.empty()) category = "General";
                
                int qty = getIntegerInput("How many do you have? ", 0);
                double price = getDoubleInput("Price per item: $");
                
                // Confirm garna lai details dekhaucha
                cout << "\nAdd this item?\n";
                cout << "Name:     " << name << "\n";
                cout << "Category: " << category << "\n";
                cout << "Quantity: " << qty << "\n";
                cout << "Price:    $" << fixed << setprecision(2) << price << "\n\n";
                
                string confirm = getStringInput("Add to inventory? (y/n) ");
                if (!confirm.empty() && tolower(confirm[0]) == 'y') {
                    inventory.addItem(name, qty, price, category);
                    cout << "\n✓ Item added!\n";
                } else {
                    cout << "\n✗ Cancelled.\n";
                }
                
                cout << "\nPress Enter to continue...";
                cin.ignore();
                break;
            }
            
            case 2: {  // Remove item
                clearScreen();
                cout << "\n--- REMOVE ITEM ---\n\n";
                
                if (inventory.isEmpty()) {
                    cout << "The inventory is empty!\n";
                } else {
                    cout << "Current inventory:\n";
                    inventory.listItems();
                    
                    int id = getIntegerInput("\nEnter ID of item to remove (0 to cancel): ", 0);
                    if (id != 0) {
                        string confirm = getStringInput("Are you sure? This can't be undone! (y/n) ");
                        if (!confirm.empty() && tolower(confirm[0]) == 'y') {
                            inventory.removeItem(id);
                        } else {
                            cout << "\nPhew! Item was not removed.\n";
                        }
                    }
                }
                
                cout << "\nPress Enter to continue...";
                cin.ignore();
                break;
            }
            
            case 3: {  // Update stock
                clearScreen();
                cout << "\n--- UPDATE STOCK LEVEL ---\n\n";
                
                if (inventory.isEmpty()) {
                    cout << "No items in inventory yet!\n";
                } else {
                    cout << "Current inventory:\n";
                    inventory.listItems();
                    
                    int id = getIntegerInput("\nWhich item ID to update? (0 to cancel) ", 0);
                    if (id != 0) {
                        cout << "\nEnter positive number to add stock, negative to remove\n";
                        int change = getIntegerInput("How many to add/remove? ");
                        inventory.updateStock(id, change);
                        cout << "\n✓ Stock updated!\n";
                    }
                }
                
                cout << "\nPress Enter to continue...";
                cin.ignore();
                break;
            }
            
            case 4:  // View all items
                clearScreen();
                cout << "\n--- FULL INVENTORY ---\n\n";
                inventory.listItems();
                cout << "Press Enter to go back...";
                cin.ignore();
                break;
                
            case 5: {  // Search
                clearScreen();
                cout << "\n--- SEARCH INVENTORY ---\n\n";
                
                string query = getStringInput("Search by name, category, or ID: ", true);
                if (!query.empty()) {
                    clearScreen();
                    cout << "\n--- SEARCH RESULTS ---\n\n";
                    inventory.searchItem(query);
                }
                
                cout << "\nPress Enter to continue...";
                cin.ignore();
                break;
            }
            
            case 6:  // Exit
                clearScreen();
                cout << "\n=== THANKS FOR USING INVENTORY MANAGER! ===\n\n";
                cout << "Your inventory has been saved.\n";
                cout << "Have a great day!\n\n";
                keepRunning = false;
                break;
                
            default:
                cout << "\nHmm, that's not a valid option. Try again!\n";
                cout << "Press Enter to continue...";
                cin.ignore();
        }
    }
    
    return 0;
}