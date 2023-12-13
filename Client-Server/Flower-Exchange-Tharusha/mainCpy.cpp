#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <map>
#include <chrono>
#include <iomanip>

using namespace std;

///////////////////// Creating the structure of the order (input file)/////////////////////////////
class Order {
public:
    string Client_Order_ID;
    string Instrument;
    string Ord_id;
    int side;
    int Quantity;
    double Price;
};

///////////////////////// Order book (BUY) ///////////////////////

class Order_Book {
public:
    string id;
    int qty;
    string Order_ID;
    double price; // Changed to double
};

////////////////////////// Creating structure of the execution report /////////////////////////

class Execution {
public:
    string client_Or_ID;
    string order_ID;
    string instrument;
    int side;
    double price;
    int quantity;
    string status;
    string reason;
    string time;
};

class Flower_Trading {
public:
    vector<string> instrument_list = { "Rose", "Lavender", "Lotus", "Tulip", "Orchid" };
    vector<Order*> orders;
    vector<Execution*> report;

    vector<Order_Book*> Buy_Rose;
    vector<Order_Book*> Sell_Rose;

    vector<Order_Book*> Buy_Lavender;
    vector<Order_Book*> Sell_Lavender;

    vector<Order_Book*> Buy_Lotus;
    vector<Order_Book*> Sell_Lotus;

    vector<Order_Book*> Buy_Tulip;
    vector<Order_Book*> Sell_Tulip;

    vector<Order_Book*> Buy_Orchid;
    vector<Order_Book*> Sell_Orchid;

    //////////////////////////////// Function to get the time ///////////////////////////////////////////

    string get_time() {
        // Get the current time
        auto currentTime = chrono::system_clock::now();
        time_t time = chrono::system_clock::to_time_t(currentTime);

        // Convert the time to a tm structure
        struct tm* tmInfo = localtime(&time);

        // Get the milliseconds
        auto milliseconds = chrono::duration_cast<chrono::milliseconds>(currentTime.time_since_epoch()) % 1000;

        // Format the time as YYYYMMDD-HHMMSS.sss
        ostringstream oss;
        oss << put_time(tmInfo, "%Y%m%d-%H%M%S") << "." << setfill('0') << setw(3) << milliseconds.count();

        return oss.str();
    }

    /////////////////////////////// Function to read the input csv file //////////////////////////////////////

    void Read_CSV(const string& csv_path) {
        ifstream file(csv_path);

        if (!file.is_open()) {
            cout << "Failed to open the file" << endl;
            return;
        }

        string line, field;

        getline(file, line);     // Neglecting the first line which contains headers
        int ord = 1;
        // Creating order object for each order
        while (getline(file, line)) {
            istringstream iss(line);
            Order* order = new Order;

            getline(iss, order->Client_Order_ID, ',');
            getline(iss, order->Instrument, ',');

            getline(iss, field, ',');
            order->side = stoi(field);

            getline(iss, field, ',');
            order->Quantity = stoi(field);

            getline(iss, field, ',');
            order->Price = stod(field);

            order->Ord_id = "ord" + to_string(ord);

            orders.push_back(order);
            ord++;

        }
        file.close();

    }

    /////////////////////////////// Function to check the validity of the order /////////////////////////

    bool is_Invalid_Order(const Order* order, Execution*& transaction) {
        if (!std::count(instrument_list.begin(), instrument_list.end(), order->Instrument)) {
            transaction->status = "Rejected";
            transaction->reason = "Instrument is not supported";
            transaction->time = get_time();
            transaction->side = order->side;
            return true;
        }
        else if (order->side != 1 && order->side != 2) {
            transaction->status = "Rejected";
            transaction->reason = "Invalid side";
            transaction->side = order->side;
            transaction->time = get_time();
            return true;
        }
        else if (order->Price < 0) {
            transaction->status = "Rejected";
            transaction->reason = "Invalid price";
            transaction->time = get_time();
            transaction->side = order->side;
            return true;
        }
        else if (order->Quantity < 10 || order->Quantity > 1000) {
            transaction->status = "Rejected";
            transaction->reason = "Quantity is out of the range.";
            transaction->time = get_time();
            transaction->side = order->side;
            return true;
        }
        else if (order->Quantity % 10 != 0) {
            transaction->status = "Rejected";
            transaction->reason = "Quantity must be a multiple of 10.";
            transaction->time = get_time();
            transaction->side = order->side;
            return true;
        }
        return false;

    }

    //////////////////// Function to match the order ////////////////////////

    vector<Order_Book*> Matching(Order* order, Execution*& transaction, vector<Order_Book*>& Buy_Book, vector<Order_Book*>& Sell_Book, int i) {
        auto ascending = [](Order_Book* a, Order_Book* b) {
            return a->price < b->price;
            };

        auto descending = [](Order_Book* a, Order_Book* b) {
            return a->price > b->price;
            };

        if (order->side == 1) { // Buy order
            Order_Book* Rose_b = new Order_Book;
            Rose_b->id = order->Client_Order_ID;
            Rose_b->qty = order->Quantity;
            Rose_b->price = order->Price;
            Rose_b->Order_ID = "ord" + to_string(i);
            Buy_Book.push_back(Rose_b);
            //sort(Buy_Book.begin(), Buy_Book.end(), descending);

        }
        else { // Sell order
            Order_Book* Rose_s = new Order_Book;
            Rose_s->id = order->Client_Order_ID;
            Rose_s->qty = order->Quantity;
            Rose_s->price = order->Price;
            Rose_s->Order_ID = "ord" + to_string(i);
            Sell_Book.push_back(Rose_s);
            //sort(Sell_Book.begin(), Sell_Book.end(), ascending);
        }
        if (order->side == 1) {
            bool matched = false;
            bool loop_needed1 = false;
            sort(Sell_Book.begin(), Sell_Book.end(), ascending);

            for (Order_Book* sell_order : Sell_Book) {

                if (loop_needed1) {
                    Sell_Book.erase(Sell_Book.begin());
                    loop_needed1 = false;
                }

                if (order->Price >= sell_order->price) {

                    matched = true;

                    transaction->client_Or_ID = order->Client_Order_ID;
                    transaction->instrument = order->Instrument;  // Set the instrument correctly
                    transaction->price = order->Price; // Set the price correctly

                    if (order->Quantity == sell_order->qty) {
                        Execution* transaction = new Execution;
                        transaction->side = 1;
                        transaction->client_Or_ID = order->Client_Order_ID;
                        transaction->instrument = order->Instrument;
                        transaction->quantity = sell_order->qty; // Set the quantity correctly
                        transaction->price = order->Price;
                        transaction->status = "Fill";
                        transaction->order_ID = order->Ord_id;
                        transaction->time = get_time();
                        Sell_Book.erase(Sell_Book.begin());
                        //Buy_Book.erase(Buy_Book.begin());
                        Buy_Book.pop_back();
                        report.push_back(transaction);

                        Execution* transaction1 = new Execution;
                        transaction1->client_Or_ID = sell_order->id;
                        transaction1->side = 2;
                        transaction1->instrument = order->Instrument;  // Set the instrument correctly
                        transaction1->quantity = sell_order->qty;
                        transaction1->price = order->Price;
                        transaction1->status = "Fill";
                        transaction1->order_ID = sell_order->Order_ID;
                        transaction1->time = get_time();
                        report.push_back(transaction1);
                        matched = true;

                        break;
                    }
                    else if (order->Quantity < sell_order->qty) {
                        Execution* transaction = new Execution;
                        transaction->side = 1;
                        transaction->client_Or_ID = order->Client_Order_ID;
                        transaction->instrument = order->Instrument;
                        transaction->quantity = order->Quantity; // Set the quantity correctly
                        transaction->status = "Fill";
                        transaction->order_ID = order->Ord_id;
                        sell_order->qty -= order->Quantity;
                        transaction->price = order->Price;
                        transaction->time = get_time();
                        //Buy_Book.erase(Buy_Book.begin());
                        Buy_Book.pop_back();
                        report.push_back(transaction);

                        Execution* sell_side = new Execution;
                        sell_side->client_Or_ID = sell_order->id;
                        sell_side->status = "PFill";
                        sell_side->quantity = order->Quantity;
                        sell_side->instrument = order->Instrument;  // Set the instrument correctly
                        sell_side->price = order->Price;
                        sell_side->order_ID = sell_order->Order_ID;
                        sell_side->side = 2;
                        sell_side->time = get_time();
                        report.push_back(sell_side);
                        matched = true;

                        break;
                    }
                    else { // buy_order->qty > sell_order->qty
                        Execution* transaction = new Execution;
                        transaction->client_Or_ID = order->Client_Order_ID;
                        transaction->instrument = order->Instrument;
                        transaction->status = "PFill";
                        transaction->side = 1;
                        order->Quantity -= sell_order->qty;
                        transaction->price = order->Price;
                        transaction->quantity = sell_order->qty;
                        transaction->order_ID = order->Ord_id; // Set the quantity correctly
                        transaction->time = get_time();
                        Buy_Book.back()->qty -= sell_order->qty;
                        //sort(Buy_Book.begin(), Buy_Book.end(), descending);                  ///////////////////Changed/////////
                        report.push_back(transaction);

                        Execution* transaction1 = new Execution;
                        transaction1->side = 2;
                        transaction1->status = "Fill";
                        transaction1->quantity = sell_order->qty;
                        transaction1->instrument = order->Instrument;  // Set the instrument correctly
                        transaction1->price = order->Price;
                        transaction1->client_Or_ID = sell_order->id;
                        transaction1->order_ID = sell_order->Order_ID;
                        transaction1->time = get_time();
                        report.push_back(transaction1);
                        //Sell_Book.erase(Sell_Book.begin());
                        matched = true;
                        if (Sell_Book.size() == 1) {
                            Sell_Book.erase(Sell_Book.begin());
                        }
                        else {
                            loop_needed1 = true;
                        }
                    }
                }
            }

            if (!matched) {
                // If no match occurred for the current buy order, set status as "New"
                transaction->order_ID = order->Ord_id;;
                transaction->client_Or_ID = order->Client_Order_ID;
                transaction->instrument = order->Instrument;  // Set the instrument correctly
                transaction->price = order->Price; // Set the price correctly
                transaction->quantity = order->Quantity; // Set the quantity correctly
                transaction->status = "New";
                transaction->side = 1;
                transaction->time = get_time();
                report.push_back(transaction);
                //Buy_Rose.erase(Buy_Rose.begin());
            }

        }
        else {
            bool matched = false;
            bool loop_needed = false;
            sort(Buy_Book.begin(), Buy_Book.end(), descending);

            for (Order_Book* buy_order : Buy_Book) {
                if (loop_needed) {
                    Buy_Book.erase(Buy_Book.begin());
                    loop_needed = false;
                }

                if (order->Price <= buy_order->price) {

                    if (order->Quantity == buy_order->qty) {

                        Execution* sell_side = new Execution;
                        sell_side->side = 2;
                        sell_side->client_Or_ID = order->Client_Order_ID;
                        sell_side->status = "Fill";
                        sell_side->order_ID = order->Ord_id;;
                        sell_side->quantity = buy_order->qty; // Set the quantity correctly
                        sell_side->instrument = order->Instrument;  // Set the instrument correctly
                        sell_side->price = order->Price; // Set the price correctly
                        sell_side->order_ID = order->Ord_id;
                        sell_side->time = get_time();
                        report.push_back(sell_side);

                        Execution* buy_side = new Execution;
                        buy_side->side = 1;
                        buy_side->status = "Fill";
                        buy_side->quantity = buy_order->qty;
                        buy_side->instrument = order->Instrument;  // Set the instrument correctly
                        buy_side->price = order->Price;
                        buy_side->client_Or_ID = buy_order->id;
                        buy_side->order_ID = buy_order->Order_ID;
                        buy_side->time = get_time();
                        report.push_back(buy_side);
                        matched = 1;
                        Sell_Book.pop_back();
                        Buy_Book.erase(Buy_Book.begin());
                        //sort(Buy_Book.begin(), Buy_Book.end(), descending);
                        break;
                    }
                    else if (order->Quantity < buy_order->qty) {

                        Execution* transaction = new Execution;
                        transaction->side = 2;
                        transaction->instrument = order->Instrument;
                        transaction->status = "Fill";
                        transaction->quantity = order->Quantity; // Set the quantity correctly
                        buy_order->qty -= order->Quantity;
                        transaction->client_Or_ID = order->Client_Order_ID;
                        transaction->price = order->Price;
                        transaction->order_ID = order->Ord_id;
                        transaction->time = get_time();
                        //cout << buy_order->qty << endl;
                        Sell_Book.pop_back();
                        report.push_back(transaction);

                        Execution* buy_side = new Execution;
                        buy_side->side = 1;
                        buy_side->status = "PFill";
                        buy_side->quantity = order->Quantity;
                        buy_side->instrument = order->Instrument;  // Set the instrument correctly
                        buy_side->price = order->Price;
                        buy_side->client_Or_ID = buy_order->id;
                        buy_side->order_ID = buy_order->Order_ID;
                        buy_side->time = get_time();
                        report.push_back(buy_side);
                        matched = true;
                        //sort(Buy_Book.begin(), Buy_Book.end(), descending);
                        break;
                    }
                    else { // buy_order->qty < sell_order->qty

                        Execution* transaction = new Execution;
                        transaction->instrument = order->Instrument;
                        transaction->side = 2;
                        transaction->status = "PFill";
                        transaction->order_ID = order->Ord_id;
                        transaction->quantity = buy_order->qty; // Set the quantity correctly
                        order->Quantity -= buy_order->qty;
                        Sell_Book.back()->qty -= buy_order->qty;
                        //sort(Sell_Book.begin(), Sell_Book.end(), ascending);
                        transaction->client_Or_ID = order->Client_Order_ID;
                        transaction->price = order->Price;
                        transaction->time = get_time();
                        report.push_back(transaction);

                        Execution* buy_side = new Execution;
                        buy_side->side = 1;
                        buy_side->status = "Fill";
                        buy_side->quantity = buy_order->qty;
                        buy_side->instrument = order->Instrument;  // Set the instrument correctly
                        buy_side->price = order->Price;
                        buy_side->client_Or_ID = buy_order->id;
                        buy_side->order_ID = buy_order->Order_ID;
                        buy_side->time = get_time();
                        report.push_back(buy_side);
                        matched = true;
                        if (Buy_Book.size() == 1) {
                            Buy_Book.erase(Buy_Book.begin());
                        }
                        else {
                            loop_needed = true;
                        }
                    }
                }
            }

            if (matched == 0) {

                // If no match occurred for the current buy order, set status as "New"
                transaction->client_Or_ID = order->Client_Order_ID;
                transaction->order_ID = order->Ord_id;
                transaction->instrument = order->Instrument;  // Set the instrument correctly
                transaction->price = order->Price; // Set the price correctly
                transaction->quantity = order->Quantity; // Set the quantity correctly
                transaction->status = "New";
                transaction->side = 2;
                transaction->time = get_time();
                report.push_back(transaction);
            }
        }

        return Buy_Book, Sell_Book;

    }

    /////////////////////////////////////// Function to generate the execution report ////////////////////////////////////////

    void Write_CSV(const string& csv_path) {
        ofstream file(csv_path);

        if (!file.is_open()) {
            cout << "Failed to open the file" << endl;
            return;
        }

        // Write headers
        file << "Order_ID,Client_Order_ID,Instrument,Side,Price,Quantity,Status,Reason,Transaction_Time" << endl;

        // Write transaction details
        for (const Execution* transaction : report) {
            file << transaction->order_ID << ","
                << transaction->client_Or_ID << ","
                << transaction->instrument << ","
                << transaction->side << ","
                << transaction->price << ","
                << transaction->quantity << ","
                << transaction->status << ","
                << transaction->reason << ","
                << transaction->time << endl;
        }

        file.close();
    }

    //////////////////////////////////// Function to process the order /////////////////////////////////////

    void Process_Order() {

        int  i = 1;

        auto ascending = [](Order_Book* a, Order_Book* b) {
            return a->price < b->price;
            };

        auto descending = [](Order_Book* a, Order_Book* b) {
            return a->price > b->price;
            };

        for (Order* order : orders) {
            Execution* transaction = new Execution;
            transaction->client_Or_ID = order->Client_Order_ID;
            transaction->order_ID = order->Ord_id;
            transaction->instrument = order->Instrument;
            transaction->price = order->Price;
            transaction->quantity = order->Quantity;


            if (is_Invalid_Order(order, transaction)) {
                report.push_back(transaction);
            }
            else {
                if (order->Instrument == "Rose") {
                    Buy_Rose, Sell_Rose = Matching(order, transaction, Buy_Rose, Sell_Rose, i);
                }
                else if (order->Instrument == "Lavender") {
                    Buy_Lavender, Sell_Lavender = Matching(order, transaction, Buy_Lavender, Sell_Lavender, i);
                }
                else if (order->Instrument == "Lotus") {
                    Buy_Lotus, Sell_Lotus = Matching(order, transaction, Buy_Lotus, Sell_Lotus, i);
                }
                else if (order->Instrument == "Tulip") {
                    Buy_Tulip, Sell_Tulip = Matching(order, transaction, Buy_Tulip, Sell_Tulip, i);
                }
                else if (order->Instrument == "Orchid") {
                    Buy_Orchid, Sell_Orchid = Matching(order, transaction, Buy_Orchid, Sell_Orchid, i);
                }
            }
            i++;
        }

        // Clean up dynamically allocated memory
        for (Order_Book* buy_order : Buy_Rose) {
            delete buy_order;
        }
        for (Order_Book* sell_order : Sell_Rose) {
            delete sell_order;
        }
    }
};

int main11() {
    Flower_Trading Trading_app;
    string File_Path = "C:\\Users\\user\\Desktop\\Gippa\\Bigdataset.csv";;

    Trading_app.Read_CSV(File_Path);
    Trading_app.Process_Order();
    string File_Path_write = "C:\\Users\\user\\Desktop\\Gippa\\output.csv";;
    Trading_app.Write_CSV(File_Path_write);

    return 0;
}