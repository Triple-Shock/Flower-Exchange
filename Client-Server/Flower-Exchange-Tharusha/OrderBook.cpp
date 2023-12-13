#include "OrderBook.h"

OrderBook::OrderBook(std::string Instrument)
{
    instrument = Instrument;
}

bool OrderBook::buy_compare(Order a, Order b)
{
    return a.price > b.price;
}
bool OrderBook::sell_compare(Order a, Order b)
{
    return a.price < b.price;
}

void OrderBook::print()
{
	std::cout << "Buy:" << std::endl;
    for (int i = 0; i < buy.size(); i++)
    {
		std::cout << buy[i].price << " " << buy[i].quantity << std::endl;
	}
	std::cout << "Sell:" << std::endl;
    for (int i = 0; i < sell.size(); i++)
    {
		std::cout << sell[i].price << " " << sell[i].quantity << std::endl;
	}
}

void OrderBook::trade(Order& order)
{
    if (order.side == 2) // sell
    {
        if (buy.empty())
        {
            sell.push_back(order);
            sort(sell.begin(), sell.end(), sell_compare);
            if (order.exec_status != 3)
                order.write_csv();
            return;
        }
        for (int j = 0; j < buy.size(); j++)
        {
            Order& buys = buy[j];
            if (buys.price >= order.price)
            {
                if (buys.quantity == order.quantity)
                {
                    order.exec_status = 2;
                    order.write_csv(buys.price);

                    buys.exec_status = 2;
                    buys.write_csv();
                    buy.erase(buy.begin() + j);

                    return;
                }
                if (buys.quantity < order.quantity)
                {
                    order.exec_status = 3;
                    order.quantity = order.quantity - buys.quantity;
                    order.write_csv(buys.quantity, buys.price);

                    buys.exec_status = 2;
                    buys.write_csv();
                    buy.erase(buy.begin() + j);

                    if (!buy.empty() || order.quantity != 0)
                        trade(order);

                    return;
                }
                else // buys.quantity > order.quantity
                {
                    order.exec_status = 2;
                    order.write_csv(buys.price);

                    buys.exec_status = 3;
                    buys.quantity = buys.quantity - order.quantity;
                    buys.write_csv(order.quantity);

                    return;
                }
            }
            else // buys.price < order.price
            {
                sell.push_back(order);
                sort(sell.begin(), sell.end(), sell_compare);
                if (order.exec_status != 3)
                    order.write_csv();
                return;
            }
        }
        sell.push_back(order);
        sort(sell.begin(), sell.end(), sell_compare);
        if (order.exec_status != 3)
            order.write_csv();
    }
    else // buy
    {
        if (sell.empty())
        {
            buy.push_back(order);
            sort(buy.begin(), buy.end(), buy_compare);
            if (order.exec_status != 3)
                order.write_csv();
            return;
        }
        for (int j = 0; j < sell.size(); j++)
        {
            Order& sells = sell[j];
            if (sells.price > order.price)
            {
                buy.push_back(order);
                sort(buy.begin(), buy.end(), buy_compare);
                if (order.exec_status != 3)
                    order.write_csv();
                return;
            }
            else if (sells.price <= order.price)
            {
                if (sells.quantity == order.quantity)
                {
                    order.exec_status = 2;
                    order.write_csv(sells.price);

                    sells.exec_status = 2;
                    sells.write_csv();
                    sell.erase(sell.begin() + j);

                    return;
                }
                if (sells.quantity < order.quantity)
                {
                    order.exec_status = 3;
                    order.quantity = order.quantity - sells.quantity;
                    order.write_csv(sells.quantity, sells.price);

                    sells.exec_status = 2;
                    sells.write_csv();
                    sell.erase(sell.begin() + j);

                    if (!sell.empty() || order.quantity != 0)
                        trade(order);

                    return;
                }
                else // sells.quantity > order.quantity
                {
                    order.exec_status = 2;
                    order.write_csv(sells.price);

                    sells.exec_status = 3;
                    sells.quantity = sells.quantity - order.quantity;
                    sells.write_csv(order.quantity);

                    return;
                }
            }
        }
        buy.push_back(order);
        sort(buy.begin(), buy.end(), buy_compare);
        if (order.exec_status != 3)
            order.write_csv();
    }
}
