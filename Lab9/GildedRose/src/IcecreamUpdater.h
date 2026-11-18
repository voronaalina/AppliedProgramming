#pragma once

#include "UpdateItem.h"

const std::string kIceCream = "Ice Cream";
const int kIceCreamNormalDecay = 1;  
const int kIceCreamHighDecay = 3;    

enum class Temperature { Normal, High };

class IceCreamUpdater : public ItemUpdater {
public:
    explicit IceCreamUpdater(Temperature temperature) : temperature_(temperature) {}

    void update(Item& item) const override {
        const int decay = (temperature_ == Temperature::High)
            ? kIceCreamHighDecay
            : kIceCreamNormalDecay;
        for (int i = 0; i < decay; ++i) {
            decreaseQuality(item);
        }
        --item.sellIn;
        if (item.sellIn < 0) {
            item.quality = kMinQuality;
        }
    }

private:
    Temperature temperature_;
};