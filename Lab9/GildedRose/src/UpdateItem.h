#pragma once

#include <string>
#include "GildedRose.h"

const std::string kAgedBrie = "Aged Brie";
const std::string kSulfuras = "Sulfuras, Hand of Ragnaros";
const std::string kBackstagePasses = "Backstage passes to a TAFKAL80ETC concert";

const int kMinQuality = 0;
const int kMaxQuality = 50;
const int kBackstageDoubleRateDays = 10;  
const int kBackstageTripleRateDays = 5;   

inline void increaseQuality(Item& item) {
    if (item.quality < kMaxQuality) {
        ++item.quality;
    }
}

inline void decreaseQuality(Item& item) {
    if (item.quality > kMinQuality) {
        --item.quality;
    }
}

// правила оновлення товару за один день
class ItemUpdater {
public:
    virtual void update(Item& item) const = 0;
    virtual ~ItemUpdater() = default;
};

//  -1 на день, після терміну продажу -2
class NormalItemUpdater : public ItemUpdater {
public:
    void update(Item& item) const override {
        decreaseQuality(item);
        --item.sellIn;
        if (item.sellIn < 0) {
            decreaseQuality(item);
        }
    }
};

//  якість зростає, після терміну продажу вдвічі швидше
class AgedBrieUpdater : public ItemUpdater {
public:
    void update(Item& item) const override {
        increaseQuality(item);
        --item.sellIn;
        if (item.sellIn < 0) {
            increaseQuality(item);
        }
    }
};

class SulfurasUpdater : public ItemUpdater {
public:
    void update(Item&) const override {}
};

class BackstagePassUpdater : public ItemUpdater {
public:
    void update(Item& item) const override {
        increaseQuality(item);
        if (item.sellIn <= kBackstageDoubleRateDays) {
            increaseQuality(item);
        }
        if (item.sellIn <= kBackstageTripleRateDays) {
            increaseQuality(item);
        }
        --item.sellIn;
        if (item.sellIn < 0) {
            item.quality = kMinQuality;
        }
    }
};