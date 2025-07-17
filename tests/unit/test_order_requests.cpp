/*
/        oqdTradierpp - Full featured Tradier API library 
/       
/        Authors:  Benjamin Cance (kc8bws@kc8bws.com), OQD Developer Team (developers@openquantdesk.com)
/        Version:           v1.1
/        Release Date:  06/30/2025
/        License: Apache 2.0
/        Disclaimer: This software is provided "as-is" without warranties of any kind. 
/                    Use at your own risk. The authors are not liable for any trading losses.
/                    Not financial advice. By using this software, you accept all risks.
/
*/

#include <gtest/gtest.h>
#include "oqdTradierpp/trading/order_requests.hpp"
#include "oqdTradierpp/core/enums.hpp"

using namespace oqd;

class OrderRequestTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(OrderRequestTest, EquityOrderRequestConstruction) {
    EquityOrderRequest request;
    EXPECT_EQ(request.order_class, OrderClass::Equity);
    EXPECT_EQ(request.symbol, "");
    EXPECT_EQ(request.side, OrderSide::Buy);
    EXPECT_EQ(request.quantity, 0);
    EXPECT_EQ(request.type, OrderType::Market);
    EXPECT_EQ(request.duration, OrderDuration::Day);
    EXPECT_FALSE(request.price.has_value());
    EXPECT_FALSE(request.stop.has_value());
    EXPECT_FALSE(request.tag.has_value());
}

TEST_F(OrderRequestTest, EquityOrderRequestFieldAssignment) {
    EquityOrderRequest request;
    request.symbol = "AAPL";
    request.side = OrderSide::Sell;
    request.quantity = 100;
    request.type = OrderType::Limit;
    request.duration = OrderDuration::GTC;
    request.price = 150.50;
    request.tag = "test_order";
    
    EXPECT_EQ(request.symbol, "AAPL");
    EXPECT_EQ(request.side, OrderSide::Sell);
    EXPECT_EQ(request.quantity, 100);
    EXPECT_EQ(request.type, OrderType::Limit);
    EXPECT_EQ(request.duration, OrderDuration::GTC);
    EXPECT_TRUE(request.price.has_value());
    EXPECT_EQ(request.price.value(), 150.50);
    EXPECT_TRUE(request.tag.has_value());
    EXPECT_EQ(request.tag.value(), "test_order");
}

TEST_F(OrderRequestTest, EquityOrderRequestToJson) {
    EquityOrderRequest request;
    request.symbol = "TSLA";
    request.side = OrderSide::Buy;
    request.quantity = 50;
    request.type = OrderType::Limit;
    request.duration = OrderDuration::Day;
    request.price = 200.75;
    
    std::string json = request.to_json();
    
    EXPECT_TRUE(json.find("\"class\":\"equity\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"symbol\":\"TSLA\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"side\":\"buy\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"quantity\":50") != std::string::npos);
    EXPECT_TRUE(json.find("\"type\":\"limit\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"duration\":\"day\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"price\":200.75") != std::string::npos);
}

TEST_F(OrderRequestTest, EquityOrderRequestMarketOrder) {
    EquityOrderRequest request;
    request.symbol = "SPY";
    request.side = OrderSide::Buy;
    request.quantity = 10;
    request.type = OrderType::Market;
    request.duration = OrderDuration::Day;
    
    std::string json = request.to_json();
    
    EXPECT_TRUE(json.find("\"type\":\"market\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"price\"") == std::string::npos);
}

TEST_F(OrderRequestTest, EquityOrderRequestStopOrder) {
    EquityOrderRequest request;
    request.symbol = "QQQ";
    request.side = OrderSide::Sell;
    request.quantity = 25;
    request.type = OrderType::Stop;
    request.duration = OrderDuration::GTC;
    request.stop = 350.25;
    
    std::string json = request.to_json();
    
    EXPECT_TRUE(json.find("\"type\":\"stop\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"stop\":350.25") != std::string::npos);
}

TEST_F(OrderRequestTest, OptionOrderRequestConstruction) {
    OptionOrderRequest request;
    EXPECT_EQ(request.order_class, OrderClass::Option);
    EXPECT_EQ(request.symbol, "");
    EXPECT_EQ(request.option_symbol, "");
    EXPECT_EQ(request.side, OrderSide::Buy);
    EXPECT_EQ(request.quantity, 0);
    EXPECT_EQ(request.type, OrderType::Market);
    EXPECT_EQ(request.duration, OrderDuration::Day);
    EXPECT_FALSE(request.price.has_value());
    EXPECT_FALSE(request.tag.has_value());
}

TEST_F(OrderRequestTest, OptionOrderRequestFieldAssignment) {
    OptionOrderRequest request;
    request.option_symbol = "AAPL240315C00150000";
    request.side = OrderSide::SellToOpen;
    request.quantity = 5;
    request.type = OrderType::Limit;
    request.duration = OrderDuration::Day;
    request.price = 2.50;
    request.tag = "option_test";
    
    EXPECT_EQ(request.option_symbol, "AAPL240315C00150000");
    EXPECT_EQ(request.side, OrderSide::SellToOpen);
    EXPECT_EQ(request.quantity, 5);
    EXPECT_EQ(request.type, OrderType::Limit);
    EXPECT_EQ(request.duration, OrderDuration::Day);
    EXPECT_TRUE(request.price.has_value());
    EXPECT_EQ(request.price.value(), 2.50);
    EXPECT_TRUE(request.tag.has_value());
    EXPECT_EQ(request.tag.value(), "option_test");
}

TEST_F(OrderRequestTest, OptionOrderRequestToJson) {
    OptionOrderRequest request;
    request.option_symbol = "SPY240315P00400000";
    request.side = OrderSide::BuyToOpen;
    request.quantity = 10;
    request.type = OrderType::Limit;
    request.duration = OrderDuration::GTC;
    request.price = 1.25;
    
    std::string json = request.to_json();
    
    EXPECT_TRUE(json.find("\"class\":\"option\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"option_symbol\":\"SPY240315P00400000\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"side\":\"buy_to_open\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"quantity\":10") != std::string::npos);
    EXPECT_TRUE(json.find("\"type\":\"limit\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"duration\":\"gtc\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"price\":1.25") != std::string::npos);
}

TEST_F(OrderRequestTest, OptionOrderRequestSellToClose) {
    OptionOrderRequest request;
    request.option_symbol = "TSLA240315C00200000";
    request.side = OrderSide::SellToClose;
    request.quantity = 3;
    request.type = OrderType::Market;
    request.duration = OrderDuration::Day;
    
    std::string json = request.to_json();
    
    EXPECT_TRUE(json.find("\"side\":\"sell_to_close\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"type\":\"market\"") != std::string::npos);
}

TEST_F(OrderRequestTest, OptionOrderRequestBuyToClose) {
    OptionOrderRequest request;
    request.option_symbol = "NVDA240315P00700000";
    request.side = OrderSide::BuyToClose;
    request.quantity = 2;
    request.type = OrderType::Limit;
    request.duration = OrderDuration::Day;
    request.price = 0.50;
    
    std::string json = request.to_json();
    
    EXPECT_TRUE(json.find("\"side\":\"buy_to_close\"") != std::string::npos);
    EXPECT_TRUE(json.find("\"price\":0.50") != std::string::npos);
}

TEST_F(OrderRequestTest, OptionalFieldsHandling) {
    EquityOrderRequest request;
    request.symbol = "MSFT";
    request.side = OrderSide::Buy;
    request.quantity = 100;
    request.type = OrderType::Market;
    request.duration = OrderDuration::Day;
    
    std::string json = request.to_json();
    
    EXPECT_TRUE(json.find("\"price\"") == std::string::npos);
    EXPECT_TRUE(json.find("\"stop\"") == std::string::npos);
    EXPECT_TRUE(json.find("\"tag\"") == std::string::npos);
}

TEST_F(OrderRequestTest, AllOptionalFieldsPresent) {
    EquityOrderRequest request;
    request.symbol = "GOOGL";
    request.side = OrderSide::Sell;
    request.quantity = 20;
    request.type = OrderType::StopLimit;
    request.duration = OrderDuration::GTC;
    request.price = 2800.00;
    request.stop = 2750.00;
    request.tag = "comprehensive_test";
    
    std::string json = request.to_json();
    
    EXPECT_TRUE(json.find("\"price\":2800") != std::string::npos);
    EXPECT_TRUE(json.find("\"stop\":2750") != std::string::npos);
    EXPECT_TRUE(json.find("\"tag\":\"comprehensive_test\"") != std::string::npos);
}

TEST_F(OrderRequestTest, ZeroQuantity) {
    EquityOrderRequest request;
    request.symbol = "AMD";
    request.side = OrderSide::Buy;
    request.quantity = 0;
    request.type = OrderType::Market;
    request.duration = OrderDuration::Day;
    
    std::string json = request.to_json();
    
    EXPECT_TRUE(json.find("\"quantity\":0") != std::string::npos);
}

TEST_F(OrderRequestTest, LargeQuantity) {
    EquityOrderRequest request;
    request.symbol = "F";
    request.side = OrderSide::Buy;
    request.quantity = 1000000;
    request.type = OrderType::Market;
    request.duration = OrderDuration::Day;
    
    std::string json = request.to_json();
    
    EXPECT_TRUE(json.find("\"quantity\":1000000") != std::string::npos);
}

TEST_F(OrderRequestTest, PrecisionHandling) {
    EquityOrderRequest request;
    request.symbol = "BRK.A";
    request.side = OrderSide::Buy;
    request.quantity = 1;
    request.type = OrderType::Limit;
    request.duration = OrderDuration::Day;
    request.price = 123456.789;
    
    std::string json = request.to_json();
    
    EXPECT_TRUE(json.find("\"price\":123456.79") != std::string::npos);
}