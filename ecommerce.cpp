#include <iostream>
#include <string>
using namespace std;

/* ---------------- PRODUCT (Doubly Linked List) ---------------- */
struct Product {
    int id;
    string name;
    string category;
    float price;
    int stock;
    Product* prev;
    Product* next;
    int recommendedId;
};

Product* head = NULL;
Product* tail = NULL;

/* ---------------- CATEGORY (BST) ---------------- */
struct Category {
    string name;
    Category* left;
    Category* right;
};

Category* root = NULL;

/* ---------------- HISTORY (STACK) ---------------- */
struct History {
    int productId;
    History* next;
};

History* topHistory = NULL;

/* ---------------- ORDER (QUEUE) ---------------- */
struct Order {
    int productId;
    string customerName;
    string mobileNumber;
    string address;
    int status;   // 1 = New, 2 = Processing, 3 = Delivered
    Order* next;
};

Order* frontOrder = NULL;
Order* rearOrder = NULL;

/* ---------------- CART (Linked List) ---------------- */
struct CartItem {
    int productId;
    int quantity;
    CartItem* next;
};

CartItem* cartHead = NULL;
CartItem* cartTail = NULL;

/* ---------------- CATEGORY FUNCTIONS ---------------- */
Category* insertCategory(Category* node, string name) {
    if (!node) return new Category{ name, NULL, NULL };
    if (name < node->name) node->left = insertCategory(node->left, name);
    else if (name > node->name) node->right = insertCategory(node->right, name);
    return node;
}

void displayCategories(Category* node) {
    if (!node) return;
    displayCategories(node->left);
    cout << "- " << node->name << endl;
    displayCategories(node->right);
}

bool categoryExists(Category* node, const string& name) {
    if (!node) return false;
    if (node->name == name) return true;
    if (name < node->name) return categoryExists(node->left, name);
    return categoryExists(node->right, name);
}

/* ---------------- PRODUCT FUNCTIONS ---------------- */
Product* searchProduct(int id) {
    Product* temp = head;
    while (temp) {
        if (temp->id == id) return temp;
        temp = temp->next;
    }
    return NULL;
}

void addProduct(int id, string name, string cat, float price, int stock, int recId = -1) {
    if (searchProduct(id)) { cout << "Product ID already exists.\n"; return; }
    if (!categoryExists(root, cat)) { cout << "Category does not exist.\n"; return; }
    Product* p = new Product{ id, name, cat, price, stock, tail, NULL, recId };
    if (!head) head = p;
    if (tail) tail->next = p;
    tail = p;
    cout << "Product added successfully.\n";
}

void updateProduct(int id) {
    Product* p = searchProduct(id);
    if (!p) { cout << "Product not found.\n"; return; }
    cin.ignore();
    cout << "Enter new name: "; getline(cin, p->name);
    string newCat; cout << "Enter new category: "; getline(cin, newCat);
    if (!categoryExists(root, newCat)) { cout << "Category does not exist.\n"; return; }
    p->category = newCat;
    cout << "Enter new price and stock: "; cin >> p->price >> p->stock;
    cout << "Product updated.\n";
}

void deleteProduct(int id) {
    Product* p = searchProduct(id);
    if (!p) { cout << "Product not found.\n"; return; }
    if (p->prev) p->prev->next = p->next; else head = p->next;
    if (p->next) p->next->prev = p->prev; else tail = p->prev;
    delete p;
    cout << "Product deleted.\n";
}

void displayProducts() {
    Product* temp = head;
    cout << "\n--- Product List ---\n";
    while (temp) {
        cout << "ID: " << temp->id
            << ", Name: " << temp->name
            << ", Category: " << temp->category
            << ", Price: " << temp->price
            << ", Stock: " << temp->stock;
        if (temp->recommendedId != -1) cout << ", Recommended ID: " << temp->recommendedId;
        cout << endl;
        temp = temp->next;
    }
}

/* ---------------- HISTORY FUNCTIONS ---------------- */
void pushHistory(int id) {
    History* h = new History{ id, topHistory };
    topHistory = h;
}

void showHistory() {
    if (!topHistory) { cout << "No recently viewed products.\n"; return; }
    cout << "\n--- Recently Viewed ---\n";
    History* temp = topHistory;
    while (temp) {
        Product* p = searchProduct(temp->productId);
        if (p) cout << "- " << p->name << endl;
        temp = temp->next;
    }
}

/* ---------------- CART FUNCTIONS ---------------- */
void addToCart(int id, int qty) {
    Product* p = searchProduct(id);
    if (!p || p->stock < qty) { cout << "Product unavailable.\n"; return; }
    CartItem* item = new CartItem{ id, qty, NULL };
    if (!cartHead) cartHead = cartTail = item;
    else { cartTail->next = item; cartTail = item; }
    cout << "Added to cart.\n";
}

/* ---------------- CHECKOUT + RECEIPT ---------------- */
void checkout() {
    if (!cartHead) { cout << "Cart is empty.\n"; return; }
    string name, mobile, address; cin.ignore();
    cout << "Enter name: "; getline(cin, name);
    cout << "Enter mobile number: "; getline(cin, mobile);
    cout << "Enter delivery address: "; getline(cin, address);

    float total = 0;
    cout << "\n========== RECEIPT ==========\n";
    cout << "Customer: " << name << "\nMobile: " << mobile << "\nAddress: " << address << endl;
    cout << "-----------------------------\n";
    cout << "Product\tQty\tPrice\tSubtotal\n";
    cout << "-----------------------------\n";

    CartItem* temp = cartHead;
    while (temp) {
        Product* p = searchProduct(temp->productId);
        if (p) {
            float subtotal = p->price * temp->quantity;
            total += subtotal;
            cout << p->name << "\t" << temp->quantity << "\t" << p->price << "\t" << subtotal << endl;
            // Create order in queue
            Order* o = new Order{ p->id, name, mobile, address, 1, NULL };
            if (!rearOrder) frontOrder = rearOrder = o;
            else { rearOrder->next = o; rearOrder = o; }
            p->stock -= temp->quantity;
        }
        temp = temp->next;
    }

    cout << "-----------------------------\nTotal Amount: " << total << endl;
    cout << "=============================\nOrder placed successfully!\n";

    while (cartHead) { CartItem* tmp = cartHead; cartHead = cartHead->next; delete tmp; }
    cartTail = NULL;
}

/* ---------------- ADMIN FUNCTIONS ---------------- */
bool adminLogin() {
    string pass; cout << "Enter Admin Password: "; cin >> pass;
    return pass == "1234";
}

void viewOrders() {
    if (!frontOrder) { cout << "No orders placed yet.\n"; return; }
    cout << "\n--- Orders (FIFO) ---\n";
    Order* temp = frontOrder; int index = 1;
    while (temp) {
        Product* p = searchProduct(temp->productId);
        cout << index++ << ". Product: " << (p ? p->name : "Deleted")
            << ", Customer: " << temp->customerName
            << ", Mobile: " << temp->mobileNumber
            << ", Address: " << temp->address
            << ", Status: ";
        switch (temp->status) { case 1: cout << "New"; break; case 2: cout << "Processing"; break; case 3: cout << "Delivered"; break; }
                                      cout << "\n----------------------\n";
                                      temp = temp->next;
    }
}

void updateOrderStatus() {
    if (!frontOrder) { cout << "No orders to update.\n"; return; }
    viewOrders();
    int choice, newStatus;
    cout << "Enter order number to update: "; cin >> choice;
    cout << "Choose new status: 1=New, 2=Processing, 3=Delivered: "; cin >> newStatus;
    if (newStatus < 1 || newStatus > 3) { cout << "Invalid status!\n"; return; }
    Order* temp = frontOrder; int count = 1;
    while (temp && count < choice) { temp = temp->next; count++; }
    if (temp) { temp->status = newStatus; cout << "Order status updated successfully.\n"; }
    else cout << "Order not found.\n";
}

/* ---------------- ADMIN PANEL ---------------- */
void adminPanel() {
    int choice;
    do {
        cout << "\n--- Admin Panel ---\n";
        cout << "1.Add Product 2.Update Product 3.Delete Product\n";
        cout << "4.Add Category 5.Display Categories 6.Display Products\n";
        cout << "7.View Orders 8.Update Order Status 9.Exit\n";
        cout << "Enter choice: "; cin >> choice;
        if (choice == 1) {
            int id, stock, rec; float price; string name, cat; cin.ignore();
            cout << "ID: "; cin >> id; cin.ignore();
            cout << "Name: "; getline(cin, name);
            cout << "Category: "; getline(cin, cat);
            cout << "Price & Stock: "; cin >> price >> stock;
            cout << "Recommended ID (-1 if none): "; cin >> rec;
            addProduct(id, name, cat, price, stock, rec);
        }
        else if (choice == 2) { int id; cout << "Product ID: "; cin >> id; updateProduct(id); }
        else if (choice == 3) { int id; cout << "Product ID: "; cin >> id; deleteProduct(id); }
        else if (choice == 4) { string cat; cin.ignore(); cout << "Category: "; getline(cin, cat); root = insertCategory(root, cat); }
        else if (choice == 5) displayCategories(root);
        else if (choice == 6) displayProducts();
        else if (choice == 7) viewOrders();
        else if (choice == 8) updateOrderStatus();
    } while (choice != 9);
}

/* ---------------- CUSTOMER PANEL ---------------- */
void customerPanel() {
    int choice;
    do {
        cout << "\n--- Customer Panel ---\n";
        cout << "1.View Products 2.View Product 3.Add to Cart\n";
        cout << "4.Checkout 5.View History 6.Exit\n";
        cout << "Enter choice: "; cin >> choice;
        if (choice == 1) displayProducts();
        else if (choice == 2) { int id; cout << "Product ID: "; cin >> id; pushHistory(id); }
        else if (choice == 3) { int id, qty; cout << "ID & Qty: "; cin >> id >> qty; addToCart(id, qty); }
        else if (choice == 4) checkout();
        else if (choice == 5) showHistory();
    } while (choice != 6);
}

/* ---------------- MAIN ---------------- */
int main() {
    int choice;
    do {
        cout << "\n=== E-Commerce System ===\n";
        cout << "1.Admin Panel" << endl; cout << "2.Customer Panel " << endl;cout<<"3.Exit\nEnter choice : "; cin >> choice;
        if (choice == 1 && adminLogin()) adminPanel();
        else if (choice == 2) customerPanel();
    } while (choice != 3);
    return 0;
}
