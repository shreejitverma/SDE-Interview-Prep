/**
 * Author: Shreejit Verma
 * GitHub: https://github.com/shreejitverma
 * 
 * Topic: Limit Order Book (Simplified)
 * Description: A basic implementation of a Limit Order Book using std::map for price-time priority.
 *           In a real HFT system, std::map is often replaced by flat arrays or pool allocators for cache locality.
 */

#include <iostream>
#include <map>
#include <list>
#include <string>

enum class Side { BUY, SELL };

struct Order {
    int id;
    double price;
    int quantity;
    Side side;
};

class OrderBook {
private:
    // Price -> List of Orders (Time Priority)
    // std::greater for Bids (Highest price first), std::less for Asks (Lowest price first)
    std::map<double, std::list<Order>, std::greater<double>> bids;
    std::map<double, std::list<Order>, std::less<double>> asks;

public:
    void addOrder(int id, double price, int quantity, Side side) {
        Order newOrder = {id, price, quantity, side};

        if (side == Side::BUY) {
            bids[price].push_back(newOrder);
        } else {
            asks[price].push_back(newOrder);
        }
        matchOrders();
    }

    void matchOrders() {
        while (!bids.empty() && !asks.empty()) {
            auto bestBidIter = bids.begin();
            auto bestAskIter = asks.begin();

            if (bestBidIter->first >= bestAskIter->first) {
                // Match Found
                Order& bid = bestBidIter->second.front();
                Order& ask = bestAskIter->second.front();

                int matchQty = std::min(bid.quantity, ask.quantity);
                
                std::cout << "MATCH: " << matchQty << " units @ $" << ask.price << "\n";

                bid.quantity -= matchQty;
                ask.quantity -= matchQty;

                if (bid.quantity == 0) bestBidIter->second.pop_front();
                if (ask.quantity == 0) bestAskIter->second.pop_front();

                if (bestBidIter->second.empty()) bids.erase(bestBidIter);
                if (bestAskIter->second.empty()) asks.erase(bestAskIter);
            } else {
                break; // No overlap
            }
        }
    }

    void printBook() {
        std::cout << "\n--- Order Book ---\n";
        std::cout << "ASKS (Sellers):\n";
        for (auto it = asks.rbegin(); it != asks.rend(); ++it) { // Print high to low
             for(auto& o : it->second) printf("  $%.2f : %d\n", o.price, o.quantity);
        }
        std::cout << "BIDS (Buyers):\n";
        for (auto& pair : bids) {
             for(auto& o : pair.second) printf("  $%.2f : %d\n", o.price, o.quantity);
        }
        std::cout << "------------------\n";
    }
};

int main() {
    OrderBook book;
    
    // Scenario: Market making
    book.addOrder(1, 100.0, 10, Side::BUY);
    book.addOrder(2, 99.0, 5, Side::BUY);
    book.addOrder(3, 101.0, 10, Side::SELL);
    
    book.printBook();

    // Aggressive Buy (Crosses Spread)
    std::cout << "\nIncoming Aggressive Buy Order...\n";
    book.addOrder(4, 101.5, 5, Side::BUY);

    book.printBook();

    return 0;
}
